Texture2D Texture : register(t0);
sampler TextureSampler : register(s0);

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

cbuffer constant
{
	row_major float4x4 m_TVM;
};


PS_INPUT vsmain(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.position = mul(input.position, m_TVM);
	output.texcoord = input.texcoord;
	return output;
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
	return Texture.Sample(TextureSampler, 1.0 - input.texcoord);
}