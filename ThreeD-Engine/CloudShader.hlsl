// Reference:
// http://graphicsrunner.blogspot.de/2009/01/volume-rendering-101.html

// Textures and samplers
Texture3D<float> txVolume : register(t0);
Texture2D<float4> BlueNoise : register(t1);
sampler samplerBlueNoise : register(s1);
Texture2D Depth : register(t2);
sampler samplerDepth : register(s2);
Texture2D Main : register(t3);
sampler samplerMain : register(s3);

SamplerState samplerLinear : register(s0);

// Constants and constant buffer variables
static const uint g_iMaxIterations = 128;

// for vertex shader
cbuffer cbEveryFrame : register(b0)
{
	row_major float4x4 m_transform;
	row_major float4x4 m_view;
	row_major float4x4 m_projection;
	row_major float4x4 m_inv_transform;
	float3 cam_box_coords;
	float lightAbsorptionThroughCloud;
	float3 bounds_min;
	float lightAbsorptionTowardSun;
	float3 bounds_max;
	float f3;
	float3 cam_pos;
	float f4;
	float3 cam_dirs;
	float f5;
	float2 screenSize;
}

cbuffer cloud : register(b1)
{
	float4 shapeNoiseWeights;
	float3 shapeOffset;
	float scale;
	float3 detailOffset;
	float detailNoiseScale;
	float3 detailWeights;
	float detailNoiseWeight;
	float densityOffset;
	float densityMultiplier;
	float timeScale;
	float baseSpeed;
	float detailSpeed;
	float rayOffsetStrength;
}

// Structures
struct VSInput
{
	float4 pos : POSITION;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float3 world_pos : TEXCOORD0;
};


// Vertex shader
PSInput RayCastVS(VSInput input)
{
	PSInput output;
	output.pos = mul(input.pos, m_transform);
	output.world_pos = output.pos.xyz;
	output.pos = mul(output.pos, m_view);
	output.pos = mul(output.pos, m_projection);
	return output;
}

float2 rayBoxDst(float3 boundsMin, float3 boundsMax, float3 rayOrigin, float3 invRaydir) {
	// Adapted from: http://jcgt.org/published/0007/03/04/
	float3 t0 = (boundsMin - rayOrigin) * invRaydir;
	float3 t1 = (boundsMax - rayOrigin) * invRaydir;
	float3 tmin = min(t0, t1);
	float3 tmax = max(t0, t1);

	float dstA = max(max(tmin.x, tmin.y), tmin.z);
	float dstB = min(tmax.x, min(tmax.y, tmax.z));

	// CASE 1: ray intersects box from outside (0 <= dstA <= dstB)
	// dstA is dst to nearest intersection, dstB dst to far intersection

	// CASE 2: ray intersects box from inside (dstA < 0 < dstB)
	// dstA is the dst to intersection behind the ray, dstB is dst to forward intersection

	// CASE 3: ray misses box (dstA > dstB)

	float dstToBox = max(0, dstA);
	float dstInsideBox = max(0, dstB - dstToBox);
	return float2(dstToBox, dstInsideBox);
}

float sampleDensity(float3 pos) {
	float3 textureSpace = mul(float4(pos, 1), m_inv_transform).xyz / 2 + 0.5;
	return txVolume.Sample(samplerLinear, textureSpace).r;
}

float remap(float In, float inMin, float inMax, float outMin, float outMax)
{
	return outMin + (In - inMin) * (outMax - outMin) / (inMax - inMin);
}

float rfunc(float2 uv)
{
	float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
	return abs(noise.x + noise.y) * 0.5;
}

float lightmarch(float3 p, float3 light_dir) {
	float dstInsideBox = rayBoxDst(bounds_min, bounds_max, p, 1 / light_dir).y;
	const int numStepsLight = 8;
	const float darknessThreshold = 0.6;

	float transmittance = 1;
	float stepSize = dstInsideBox / numStepsLight;
	p += light_dir * stepSize * .5;
	float totalDensity = 0;

	for (int step = 0; step < numStepsLight; step++) {
		float density = sampleDensity(p);
		totalDensity += max(0, density * stepSize);
		p += light_dir * stepSize;
	}

	transmittance = exp(-totalDensity * lightAbsorptionTowardSun);

	float clampedTransmittance = darknessThreshold + transmittance * (1 - darknessThreshold);
	return clampedTransmittance;
}

float PI_r = 0.3183098;

float hg(float a, float g) {
	float g2 = g * g;
	return (1 - g2) / (4 * 3.1415 * pow(1 + g2 - 2 * g * (a), 1.5));
}

float phase(float3 v1, float3 v2) {
	float4 phaseParams = float4(.83f, .3f, .8f, .15f);
	float a = dot(v1, v2) / length(v1) / length(v2);
	float blend = .5;
	float hgBlend = hg(a, phaseParams.x) * (1 - blend) + hg(a, -phaseParams.y) * blend;
	return phaseParams.z + hgBlend * phaseParams.w;
}


float brightness(float depth) {
	float m = log(2);
	float c = log(2);
	float t = exp(-m * depth);
	float s = 1 - exp(-c * depth);
	return t * s;
}

float linearDepth(float depthSample)
{
	float zNear = 0.1f;
	float zFar = 1000;
	depthSample = 2.0 * depthSample - 1.0;
	float zLinear = 2.0 * zNear * zFar / (zFar + zNear - depthSample * (zFar - zNear));
	return zLinear;
}

// Pixel shader
float4 RayCastPS(PSInput input) : SV_TARGET
{
	
	float3 rayDir = normalize(input.world_pos - cam_pos);

	float2 intersections = rayBoxDst(bounds_min, bounds_max, cam_pos, 1 / rayDir);

	float3 pos_front = cam_pos + rayDir * intersections.x;
	float3 pos_back = pos_front + rayDir * intersections.y;

	float stepSize = 0.1;

	float3 light_dir = normalize(float3(1, 3, 1));

	// Accumulate result: value and transparency (alpha)
	float3 light_color = float3(1, 1, 1);
	float3 light_energy = float3(0, 0, 0);
	float transmittance = 1;

	float randomOffset = BlueNoise.SampleLevel(samplerBlueNoise, pos_front.xz, 0).r;
	randomOffset *= rayOffsetStrength;

	float dstTravelled = randomOffset;
	float dstLimit = intersections.y;

	float phaseVal = phase(rayDir, light_dir);

	float2 realpos = (input.pos.xy) / screenSize;

	float depth = linearDepth(Depth.Sample(samplerDepth, realpos).r);

	// iterate for the volume, sampling along the way at equidistant steps 
	[loop]
	while(dstTravelled < dstLimit)
	{
		float3 v = pos_front + rayDir * dstTravelled;

		if (length(v - cam_pos) >= depth) {
			break;
		}
		// sample the texture accumlating the result as we step through the texture
		float density = sampleDensity(v);

		if (density > 0) {
			float lightTransmittance = lightmarch(v, light_dir);
			light_energy += density * stepSize * transmittance * lightTransmittance * phaseVal;
			transmittance *= exp(-density * stepSize * lightAbsorptionThroughCloud);

			// Early exit
			if (transmittance < 0.01) {
				break;
			}
		}
		dstTravelled += stepSize;
	}

	float3 params = float3(1, 0, 0);
	float3 backgroundCol = Main.Sample(samplerMain, realpos).rgb;

	// Add clouds
	float3 cloudCol = light_energy * light_color;
	float3 col = backgroundCol * transmittance + cloudCol;

	return float4(col, 1);
}
