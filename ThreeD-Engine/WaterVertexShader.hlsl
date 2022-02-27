struct VS_INPUT
{
	float2 position : POSITION0;
};
struct VS_OUTPUT
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
};


VS_OUTPUT vsmain(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	//WORLD SPACE
	output.position = mul(float4(input.position.x, 0.0, input.position.y, 1.0), m_transform);
	output.world_pos = output.position.xyz;
	//VIEW SPACE
	output.position = mul(output.position, m_view);
	//SCREEN SPACE
	output.position = mul(output.position, m_projection);

	output.clip_space = output.position;

	return output;
}