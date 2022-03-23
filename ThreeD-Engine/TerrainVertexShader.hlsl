struct VS_INPUT
{
	float4 position : POSITION0;
	float3 color: TEXCOORD0;
	float3 normal: NORMAL0;
};
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 normal: NORMAL0;
	float3 world_pos: TEXCOORD1;
	float3 color: TEXCOORD0;
	float visibility : COLOR0;
};

cbuffer constant: register(b0)
{
	row_major float4x4 m_transform;
	row_major float4x4 m_view;
	row_major float4x4 m_projection;
	float4 m_camera_position;
};

VS_OUTPUT vsmain(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

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