// Structures
struct VSInput
{
	float4 pos : POSITION0;
};


struct PSInput
{
	float4 pos : SV_POSITION;
	float4 tex : TEXCOORD0;		// Position in model coordinates
};

// Constant buffer variables
cbuffer cbEveryFrame : register(b0)
{
	row_major float4x4 m_transform;
	row_major float4x4 m_view;
	row_major float4x4 m_projection;
}


// Vertex shader
PSInput ModelPositionVS(VSInput input)
{
	PSInput output;
	output.pos = mul(input.pos, m_transform);
	output.pos = mul(output.pos, m_view);
	output.pos = mul(output.pos, m_projection);

	// Pass position (in model coordinates) to texture coordinate register
	output.tex = 0.5*(input.pos + 1);

	return output;
}

// Pixel shaders
float4 ModelPositionPS(PSInput input) : SV_TARGET
{
	// Make use of linear interpolation
	return input.tex;
}
