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
};

cbuffer constant: register(b0)
{
	row_major float4x4 m_transform;
	row_major float4x4 m_view;
	row_major float4x4 m_projection;
};

Texture2D l : register(t0);
sampler lsampler : register(s0);

PS_INPUT vsmain(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	//WORLD SPACE
	output.position = mul(input.position, m_transform);
	//VIEW SPACE
	output.position = mul(output.position, m_view);
	//SCREEN SPACE
	output.position = mul(output.position, m_projection);


	output.texcoord = input.texcoord;

	return output;
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
	return float4(l.Sample(lsampler, 1 - input.texcoord).rgb, 1.0);
}