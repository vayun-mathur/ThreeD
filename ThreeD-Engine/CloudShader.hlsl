// Reference:
// http://graphicsrunner.blogspot.de/2009/01/volume-rendering-101.html

// Textures and samplers
Texture3D<float> txVolume : register(t0);

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
	float f1;
	float3 bounds_min;
	float f2;
	float3 bounds_max;
	float f3;
	float3 cam_pos;
	float f4;
	float3 cam_dirs;
	float f5;
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

float rfunc(float2 uv)
{
	float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
	return abs(noise.x + noise.y) * 0.5;
}

float cloudDepth(float3 start_pos, float3 light_dir) {
	float2 boxDst = rayBoxDst(bounds_min, bounds_max, start_pos, 1 / light_dir);
	float3 end_pos = start_pos + light_dir * boxDst.y;

	uint iterations = 32;

	float3 step = (end_pos - start_pos) / iterations;

	for (uint i = 0; i < iterations; ++i)
	{
		float3 v = start_pos + step * (i + 0.3*rfunc(float2(i, i)));
		
		if (sampleDensity(v) < 0.01) {
			end_pos = v;
			break;
		}
	}
	return length(end_pos - start_pos) / 15;
}

float PI_r = 0.3183098;

float hg(float a, float g) {
	float g2 = g * g;
	return (1 - g2) / (4 * 3.1415 * pow(1 + g2 - 2 * g * (a), 1.5));
}

float phase(float3 v1, float3 v2) {
	float4 phaseParams = float4(.8f, .3f, .8f, .15f);
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

// Pixel shader
float4 RayCastPS(PSInput input) : SV_TARGET
{
	
	float3 cam_dir = normalize(input.world_pos - cam_pos);

	float2 intersections = rayBoxDst(bounds_min, bounds_max, cam_pos, 1 / cam_dir);
	// Get the current pixel location on the screen.
	// This is used to sample the front and back textures mapped to the cube

	float3 pos_front = cam_pos + cam_dir * intersections.x;
	float3 pos_back = pos_front + cam_dir * intersections.y;

	// Calculate the direction the ray is cast
	float3 dir = normalize(pos_back - pos_front);

	float stepSize = length(pos_back - pos_front) / g_iMaxIterations;

	// Single step: direction times delta step - stepSize is precaluclated
	float3 step = stepSize * dir;

	// The current position - remember we start from the front
	//float3 v = pos_front;

	// Accumulate result: value and transparency (alpha)
	float4 result = float4(0, 0, 0, 0);

	float3 light_color = float3(1, 1, 1);
	float3 light_dir = float3(0, -1, 0);

	// iterate for the volume, sampling along the way at equidistant steps 
	[loop]
	for (uint i = 0; i < g_iMaxIterations; ++i)
	{
		float3 v = pos_front + step * (i + 0.1*rfunc(float2(i, i)));
		// sample the texture accumlating the result as we step through the texture
		float density = sampleDensity(v);

		if (result.a == 1) {
			break;
		}

		result.a += density / 5;
		result.a = clamp(result.a, 0, 1);

		float b = brightness(cloudDepth(v, light_dir)) * phase(-cam_dir, light_dir);

		result.rgb += b * light_color * result.a;
	}

	return result;
}
