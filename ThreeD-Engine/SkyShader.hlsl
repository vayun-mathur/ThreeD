struct VS_INPUT
{
	float4 position : POSITION;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 world_pos : TEXCOORD0;
};

Texture2D Depth : register(t0);
sampler samplerDepth : register(s0);
Texture2D Main : register(t1);
sampler samplerMain : register(s1);
Texture2D Stars : register(t2);
sampler samplerStars : register(s2);
Texture3D<float> Volume : register(t3);
sampler samplerVolume : register(s3);


// for vertex shader
cbuffer cbEveryFrame : register(b0)
{
	row_major float4x4 m_transform;
	row_major float4x4 m_view;
	row_major float4x4 m_projection;
	row_major float4x4 m_inv_transform;
	float3 cam_pos;
	float f;
	float2 screenSize;
	float f4, f5;
	float3 sun_dir;
	float f2;
	float3 bounds_min;
	float f3;
	float3 bounds_max;
}

PS_INPUT vsmain(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.position = input.position;
	output.position = mul(output.position, m_transform);
	output.world_pos = output.position.xyz;
	output.position = mul(output.position, m_view);
	output.position = mul(output.position, m_projection);

	return output;
}

float sampleDensity(float3 pos) {
	float3 textureSpace = mul(float4(pos, 1), m_inv_transform).xyz / 2 + 0.5;
	return any(textureSpace != saturate(textureSpace)) ? 0 : Volume.SampleLevel(samplerVolume, textureSpace, 0).r;
}

float dist_from_center(float3 p) {
	float3 c = float3(0, -6360000, 0);
	return length(p - c);
}

float collisionDist(float3 o, float3 u) {
	//return 10000;
	float3 c = float3(0, -6360000, 0);
	float r = 6460000;

	float uoc = dot(u, o - c);
	float del = uoc * uoc - (length(o - c) * length(o - c) - r * r);
	//if (del < 0) return 1000000;
	return -uoc + sqrt(del);
}

float hg(float a, float g) {
	float g2 = g * g;
	return (1 - g2) / (4 * 3.1415 * pow(1 + g2 - 2 * g * (a), 1.5));
}

float phase_m(float3 v1, float3 v2) {
	float a = dot(v1, v2) / length(v1) / length(v2);
	return hg(a, 0.995);
}

float phase_r(float3 v, float3 li) {
	float cosangle = dot(v, li);
	return 3 * (1 + cosangle * cosangle) / (16 * 3.1415926);
}

float phase_c(float3 v, float3 li) {
	float4 phaseParams = float4(.83f, .3f, .8f, .15f);
	float a = dot(v, li) / length(v) / length(li);
	float blend = .5;
	float hgBlend = hg(a, phaseParams.x) * (1 - blend) + hg(a, -phaseParams.y) * blend;
	return phaseParams.z + hgBlend * phaseParams.w;
}

float density_r(float3 p) {
	float dist = dist_from_center(p);
	dist -= 6360000;
	if (dist < 0) return 0;
	return exp(-dist / 8000);
}

float density_m(float3 p) {
	float dist = dist_from_center(p);
	dist -= 6360000;
	if (dist < 0) return 0;
	return exp(-dist / 1200);
}

float density_o(float3 p) {
	float dist = dist_from_center(p);
	dist -= 6360000;
	return max(0, 1 - abs(dist - 25000) / 15000);
}

float3 o_s_c(float3 x) {
	return float3(0.05, 0.05, 0.05) * sampleDensity(x);
}

float3 o_t_c(float3 x) {
	return float3(0.05, 0.05, 0.05) * sampleDensity(x);
}

float3 o_s_r(float3 x) {
	return float3(5.802, 13.558, 33.1) / 1e6 * density_r(x);
}

float3 o_t_r(float3 x) {
	return float3(5.802, 13.558, 33.1) / 1e6 * density_r(x);
}

float3 o_s_m(float3 x) {
	return float3(3.996, 3.996, 3.996) / 1e6 * density_m(x);
}

float3 o_t_m(float3 x) {
	return float3(8.396, 8.396, 8.396) / 1e6 * density_m(x);
}

float3 o_s_o(float3 x) {
	return float3(0, 0, 0) / 1e6 * density_o(x);
}

float3 o_t_o(float3 x) {
	return float3(0.650, 1.881, 0.085) / 1e6 * density_o(x);
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

float3 T(float3 Xa, float3 dx, float ttot) {
	const uint it_cnt = 10;

	float3 scattering = float3(0, 0, 0);
	float step_size = ttot / it_cnt;
	float t = 0;

	for (int i = 0; i < it_cnt*2;i++) {
		float3 x = Xa + t * dx;
		scattering += (o_t_r(x) + o_t_m(x) + o_t_o(x)) * step_size;

		t += step_size;
	}
	float2 boxdst = rayBoxDst(bounds_min, bounds_max, Xa, 1 / dx);
	if (boxdst.y > 0) {
		float3 box_min = Xa + boxdst.x * dx;
		for (int i = 0; i < it_cnt; i++) {
			float3 x = box_min + i * boxdst.y / it_cnt * dx;

			scattering += o_t_c(x) * boxdst.y / it_cnt;
		}
	}
	return exp(-scattering);
}

float3 S(float3 x, float3 li) {
	float t = 0;
	return T(x, li, collisionDist(x, li));
}


float3 L(float3 c, float3 v, float3 L0, float depth) {

	const uint it_cnt = 20;

	float3 p = c - v * min(collisionDist(c, -v), depth);
	float3 ret = T(c, normalize(p-c), length(p-c)) * L0;
	float step_size = length(p - c) / it_cnt;

	float3 E = float3(1, 1, 1) * 20;
	
	float3 scattering = float3(0, 0, 0);

	for (float t = 0; t <= length(p - c);) {
		float3 x = c - t * v;

		float2 boxdst = rayBoxDst(bounds_min, bounds_max, x, 1 / -v);
		if (boxdst.y > 0) {
			if (boxdst.x == 0) {
				step_size = length(bounds_max - bounds_min) / it_cnt;
			}
			else if (boxdst.x < length(p - c) / it_cnt) {
				step_size = boxdst.x + 0.1;
			}
			else {
				step_size = length(p - c) / it_cnt;
			}
		}
		else {
			step_size = length(p - c) / it_cnt;
		}

		scattering += (o_t_r(x) + o_t_m(x) + o_t_o(x) + o_t_c(x)) * step_size;

		ret +=
			(o_s_r(x) * phase_r(-v, sun_dir) + o_s_m(x) * phase_m(-v, sun_dir) + o_t_c(x) * phase_c(-v, sun_dir)) * exp(-scattering) * S(x, sun_dir) * E
			*step_size;
		//ret += Lscat(c, c - t * v, v) * step_size;

		t += step_size;
	}
	return ret;
}

float3 calculateSkyColor(float3 pos, float3 dir, float3 L0, float depth) {
	float3 c = pos;
	float3 v = -dir;
	if (depth > 980) depth = 1e15;
	return L(c, v, L0, depth);
}

float linearDepth(float depthSample)
{
	float zNear = 0.1f;
	float zFar = 1000;
	depthSample = 2.0 * depthSample - 1.0;
	float zLinear = 2.0 * zNear * zFar / (zFar + zNear - depthSample * (zFar - zNear));
	return zLinear;
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
	float3 rayDir = normalize(input.world_pos - cam_pos);
	float3 colA = float3(226, 236, 235) / 255;
	float3 colB = float3(135, 206, 235) / 255;
	float3 sun_color = float3(1, 1, 1);

	float2 realpos = (input.position.xy) / screenSize;

	float3 skyColBase;

	skyColBase = calculateSkyColor(cam_pos, rayDir, Main.Sample(samplerMain, realpos).rgb, linearDepth(Depth.Sample(samplerDepth, realpos).r));
	float darkness_thres = 0.05;
	float star_factor = 1/ darkness_thres * saturate(darkness_thres - (skyColBase.x + skyColBase.y + skyColBase.z) / 3);
	return float4(skyColBase * (1 - star_factor) + Stars.Sample(samplerStars, realpos).rgb * star_factor, 1);

}