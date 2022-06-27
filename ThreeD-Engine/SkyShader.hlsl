struct VS_INPUT
{
	float4 position : POSITION;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 world_pos : TEXCOORD0;
};


// for vertex shader
cbuffer cbEveryFrame : register(b0)
{
	row_major float4x4 m_transform;
	row_major float4x4 m_view;
	row_major float4x4 m_projection;
	float3 cam_pos;
	float f;
	float2 screenSize;
	float3 sun_dir;
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

float dist_from_center(float3 p) {
	float3 c = float3(0, -6360000, 0);
	return length(p - c);
}

float density_r(float3 p) {
	float dist = dist_from_center(p);
	dist -= 6360000;
	if (dist < 0) return 0;
	return exp(-dist / 8000);
}

float3 T(float3 Xa, float3 Xb) {
	float3 scattering = float3(0, 0, 0);
	float3 dx = (Xb - Xa) / 30;
	for (int i = 0; i < 30; i++) {
		float3 x = Xa + dx * i;
		scattering += float3(5.802, 13.558, 33.1) / 1e6 * density_r(x) * length(dx);
	}
	return exp(-scattering);
}

float ph(float3 v, float3 li) {
	float cosangle = dot(v, li);
	return 3 * (1 + cosangle * cosangle) / (16 * 3.1415926);
}

float collisionDist(float3 o, float3 u) {
	//return 10000;
	float3 c = float3(0, -6360000, 0);
	float r = 6460000;

	float uoc = dot(u, o - c);
	float del = uoc * uoc - (length(o - c) * length(o - c) - r * r);
	if (del < 0) return 1000000;
	return -uoc + sqrt(del);
}

float3 S(float3 x, float3 li) {
	float t = 0;
	return T(x, x + collisionDist(x, li) * li);
}

float3 Lscat(float3 c, float3 x, float3 v) {
	float3 E = float3(1, 1, 1) * 10;
	return float3(5.802, 13.558, 33.1) / 1e6 * T(c, x) * S(x, sun_dir) * ph(v, sun_dir) * E * density_r(x);
}

float3 L(float3 c, float3 v) {
	float3 luminance_0 = float3(0, 0, 0);
	float3 p = c - v * collisionDist(c, -v);
	float3 ret = T(c, p) * luminance_0;
	float step_size = length(p - c) / 20;
	for (float t = 0; t <= length(p - c); t += step_size) {
		ret += Lscat(c, c - t * v, v) * step_size;
	}
	return ret;
}

float3 calculateSkyColor(float3 pos, float3 dir) {
	float3 c = pos;
	float3 v = -dir;
	return L(c, v);
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
	float3 rayDir = normalize(input.world_pos - cam_pos);
	float3 colA = float3(226, 236, 235) / 255;
	float3 colB = float3(135, 206, 235) / 255;
	float3 sun_color = float3(1, 1, 1);

	float2 realpos = (input.position.xy) / screenSize;

	float3 skyColBase = calculateSkyColor(cam_pos, rayDir);

	//float3 skyColBase = lerp(colA, colB, sqrt(abs(saturate(rayDir.y))));

	float cosAngle = dot(rayDir, sun_dir);
	float phaseVal = phase(rayDir, sun_dir);

	float focusedEyeCos = pow(saturate(cosAngle), 1);
	float sun = saturate(hg(focusedEyeCos, .9995)) * normalize(T(cam_pos, cam_pos + collisionDist(cam_pos, rayDir) * rayDir));

	float3 col = skyColBase * (1 - sun) + sun_color * sun;

	if (rayDir.y < 0) return float4(0, 0, 0, 1);

	return float4(col, 1);
}