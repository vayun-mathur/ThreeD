Texture2D Texture : register(t0);
sampler TextureSampler : register(s0);

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 world_pos: TEXCOORD1;
};

cbuffer constant: register(b0)
{
	row_major float4x4 m_transform;
	row_major float4x4 m_view;
	row_major float4x4 m_projection;
	float4 m_light_direction;
	float4 m_camera_position;
	float4 m_light_position;
	float m_light_radius;
};

float3 ambientLight(float ka, float3 ia, float4 tex_color) {
	return ka * ia * tex_color.rgb;
}

float3 diffuseLight(float kd, float3 id, float4 tex_color, float3 light_dir, float3 normal, float attenuation) {
	float amount_diffuse_light = max(0, dot(light_dir, normal));
	return (kd * id * tex_color.rgb * amount_diffuse_light) / attenuation;
}

float3 specularLight(float ks, float3 is, float4 tex_color, float3 light_dir, float3 normal, float3 direction_to_camera, float shininess, float attenuation) {
	float3 reflected_light = reflect(light_dir, normal);
	float amount_specular_light = pow(max(0.0, dot(reflected_light, direction_to_camera)), shininess);
	return (ks * is * amount_specular_light) / attenuation;
}

float4 calculateDirectional(PS_INPUT input, float4 light_direction)
{
	float4 tex_color = Texture.Sample(TextureSampler, input.texcoord);

	//AMBIENT LIGHT
	float ka = 1.5;
	float3 ia = float3(0.09, 0.082, 0.082);

	float3 ambient_light = ambientLight(ka, ia, tex_color);

	//DIFFUSE LIGHT
	float kd = 0.7;

	float3 id = float3(1, 1, 1);

	float3 diffuse_light = diffuseLight(kd, id, tex_color, light_direction, input.normal, 1.0);

	//SPECULAR LIGHT
	float ks = 1.0;
	float3 direction_to_camera = normalize(input.world_pos.xyz - m_camera_position.xyz);
	float3 is = float3(1.0, 1.0, 1.0);
	float shininess = 30.0;

	float3 specular_light = specularLight(ks, is, tex_color, light_direction, input.normal, direction_to_camera, shininess, 1.0);

	float3 final_light = ambient_light + diffuse_light + specular_light;

	return float4(final_light, 1.0);
}

float4 calculatePoint(PS_INPUT input, float4 light_position, float light_radius)
{
	float4 tex_color = Texture.Sample(TextureSampler, input.texcoord);

	//AMBIENT LIGHT
	float ka = 1.5;
	float3 ia = float3(0.09, 0.082, 0.082);

	float3 ambient_light = ambientLight(ka, ia, tex_color);

	//DIFFUSE LIGHT
	float kd = 0.7;
	float3 light_dir = normalize(light_position.xyz - input.world_pos.xyz);
	float distance_light_object = length(light_position.xyz - input.world_pos.xyz);

	float fade_area = max(0,distance_light_object - light_radius);

	float constant_func = 1.0;
	float linear_func = 1.0;
	float quadratic_func = 1.0;

	float attenuation = constant_func + linear_func * fade_area + quadratic_func * fade_area * fade_area;

	float3 id = float3(1,1,1);

	float3 diffuse_light = diffuseLight(kd, id, tex_color, light_dir, input.normal, attenuation);

	//SPECULAR LIGHT
	float ks = 1.0;
	float3 direction_to_camera = normalize(input.world_pos.xyz - m_camera_position.xyz);
	float3 is = float3(1.0, 1.0, 1.0);
	float shininess = 30.0;

	float3 specular_light = specularLight(ks, is, tex_color, light_dir, input.normal, direction_to_camera, shininess, attenuation);

	float3 final_light = ambient_light + diffuse_light + specular_light;

	return float4(final_light,1.0);
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
	return calculatePoint(input, m_light_position, m_light_radius);
	//return calculateDirectional(input, m_light_direction);
}