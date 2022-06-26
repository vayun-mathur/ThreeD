#include "PhongLighting.hlsl"

struct VS_INPUT
{
	float4 position : POSITION0;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 world_pos: TEXCOORD1;
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


	output.texcoord = input.texcoord;
	output.normal = input.normal;

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

	float4 tex_color = (material.has_tex & 1) == 0 ? float4(0.2, 0.2, 0.2, 1) : map_ka.Sample(map_ka_sampler, 1 - input.texcoord);

	float3 ambient_light = ambientLight(material.ka, tex_color);

	float3 light = ambient_light;

	for (int i = 0; i < m_dlight_count; i++) {
		light += calculateDirectional(material, dlight[i], input.world_pos, input.normal);
	}
	for (int i = 0; i < m_plight_count; i++) {
		light += calculatePoint(material, plight[i], input.world_pos, input.normal);
	}

	return float4(fog(light, length(input.world_pos- m_camera_position.xyz), input.position.xy / screen_size), 1.0);
}