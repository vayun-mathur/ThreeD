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

float4 psmain(PS_INPUT input) : SV_TARGET
{
	float3 rayDir = normalize(input.world_pos - cam_pos);
	float3 colA = float3(226, 236, 235) / 255;
	float3 colB = float3(135, 206, 235) / 255;
	float3 sun_color = float3(1, 1, 1);
	float3 sun_dir = normalize(float3(1, 3, 1));

	float2 realpos = (input.position.xy) / screenSize;

	float3 skyColBase = lerp(colA, colB, sqrt(abs(saturate(rayDir.y))));

	float cosAngle = dot(rayDir, sun_dir);
	float phaseVal = phase(rayDir, sun_dir);

	float focusedEyeCos = pow(saturate(cosAngle), 1);
	float sun = saturate(hg(focusedEyeCos, .9995));

	float3 col = skyColBase * (1 - sun) + sun_color * sun;

	return float4(col, 1);
}