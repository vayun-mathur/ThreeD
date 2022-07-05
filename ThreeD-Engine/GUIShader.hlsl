struct VS_INPUT
{
	float2 position : POSITION0;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 pos_init : POSITION1;
	float2 texcoord: TEXCOORD0;
};

cbuffer cbuf : register(b0) {
	float2 center;
	float2 size;
	float4 color;
	float z;
	float radius;
};

PS_INPUT vsmain(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	//WORLD SPACE
	output.position = float4(input.position * size + center, 1-z, 1);
	output.pos_init = input.position;

	output.texcoord = input.position / 2 + 0.5;

	return output;
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
	float2 de = 1 - abs(input.pos_init);
	float2 dr = radius - de;
	if (dr.x < 0) dr.x = 0;
	if (dr.y < 0) dr.y = 0;
	if (length(dr) > radius)
		clip(-1);
	return color;
}