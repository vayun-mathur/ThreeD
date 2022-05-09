#include "PhongLighting.hlsl"

struct VS_INPUT
{
	float4 position : POSITION0;
	float3 color: TEXCOORD0;
	float3 normal: NORMAL0;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 normal: NORMAL0;
	float3 world_pos: TEXCOORD1;
	float3 color: TEXCOORD0;
	float visibility : COLOR0;
};

PS_INPUT vsmain(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	//WORLD SPACE
	output.position = mul(input.position, m_transform);
	output.world_pos = output.position.xyz;
	//VIEW SPACE
	output.position = mul(output.position, m_view);
	float4 viewSpace = output.position;
	//SCREEN SPACE
	output.position = mul(output.position, m_projection);


	output.normal = input.normal;
	output.color = input.color;

	float density = 0.007;
	float gradient = 1.5;

	float distance = length(viewSpace.xyz);
	output.visibility = exp(-pow(distance * density, gradient));
	output.visibility = clamp(output.visibility, 0.0, 1.0);

	return output;
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
	clip(dot(m_clip, float4(input.world_pos, 1)));

	float4 tex_color = float4(input.color, 1);

	float3 ambient_light = ambientLight(material.ka, tex_color);

	float3 light = ambient_light;

	for (int i = 0; i < m_dlight_count; i++) {
		light += calculateDirectional(material, dlight[i], input.world_pos, input.normal) * 0.5;
	}
	for (int i = 0; i < m_plight_count; i++) {
		light += calculatePoint(material, plight[i], input.world_pos, input.normal);
	}

	return float4(light, 1.0) * input.visibility + fog_color * (1 - input.visibility);
}