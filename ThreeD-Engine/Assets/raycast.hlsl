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
	return txVolume.Sample(samplerLinear, pos).r;
}

float beer(float d) {
	float beer = exp(-d);
	return beer;
}

float lightMarch(float3 pos) {
	const int numStepsLight = 64;
	const float lightAbsorptionTowardSun = 1;
	const float darknessThreshold = 0.2;

	float3 dirToLight = float3(0, 1, 0);
	float dstInsideBox = rayBoxDst(float3(-1, -1, -1), float3(1, 1, 1), pos, 1 / dirToLight).y;

	float transmittance = 1;
	float stepSize = dstInsideBox / numStepsLight;
	pos += dirToLight * stepSize * .5;
	float totalDensity = 0;

	for (int step = 0; step < numStepsLight; step++) {
		float density = sampleDensity(pos);
		totalDensity += max(0, density * stepSize);
		pos += dirToLight * stepSize;
	}

	transmittance = beer(totalDensity * lightAbsorptionTowardSun);

	float clampedTransmittance = darknessThreshold + transmittance * (1 - darknessThreshold);
	return clampedTransmittance;
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

	pos_front = mul(float4(pos_front, 1), m_inv_transform).xyz / 2 +0.5;
	pos_back = mul(float4(pos_back, 1), m_inv_transform).xyz / 2+0.5;

	// Calculate the direction the ray is cast
	float3 dir = normalize(pos_back - pos_front);

	float g_fStepSize = length(pos_back - pos_front) / g_iMaxIterations;

	// Single step: direction times delta step - g_fStepSize is precaluclated
	float3 step = g_fStepSize * dir;

	// The current position - remember we start from the front
	float3 v = pos_front;

	// Accumulate result: value and transparency (alpha)
	float2 result = float2(0, 0);

	// iterate for the volume, sampling along the way at equidistant steps 
	for (uint i = 0; i < g_iMaxIterations; ++i)
	{
		// sample the texture accumlating the result as we step through the texture
		float density = sampleDensity(v);

		const float b = 1;

		float brightness_factor = exp(-b * density * (1-v.y));

		// Front to back blending
		result.x += (1 - result.y) * density * brightness_factor * 0.5f * density;
		result.y += (1 - result.y) * density * 0.5f * density * 0.5f;

		// Advance the current position
		v += step;
	}

	return float4(result.r, result.r, result.r, result.y);
}
