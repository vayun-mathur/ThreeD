Texture2D reflection : register(t0);
sampler reflection_sampler : register(s0);
Texture2D refraction : register(t1);
sampler refraction_sampler : register(s1);

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 clip_space : TEXCOORD0;
	float3 world_pos: TEXCOORD1;
};

cbuffer constant: register(b0)
{
	row_major float4x4 m_transform;
	row_major float4x4 m_view;
	row_major float4x4 m_projection;
	float4 m_camera_position;
	float4 m_clip;
};

float4 psmain(PS_INPUT input) : SV_TARGET
{
	clip(dot(m_clip, float4(input.world_pos, 1))-0.1);
	float2 ndc = input.clip_space.xy/input.clip_space.w / 2 + 0.5;
	float2 refraction_coord = float2(ndc.x, 1-ndc.y);
	float2 reflection_coord = float2(ndc.x, ndc.y);

	float4 reflection_color = float4(reflection.Sample(reflection_sampler, reflection_coord).xyz, 1.0);
	float4 refraction_color = float4(refraction.Sample(refraction_sampler, refraction_coord).xyz, 1.0);

	return reflection_color * 0.5 + refraction_color * 0.5;
}