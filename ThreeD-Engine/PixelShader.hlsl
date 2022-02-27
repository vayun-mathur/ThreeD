Texture2D map_ka : register(t0);
sampler map_ka_sampler : register(s0);
Texture2D map_kd : register(t1);
sampler map_kd_sampler : register(s1);
Texture2D map_ks : register(t2);
sampler map_ks_sampler : register(s2);

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 world_pos: TEXCOORD1;
};

//Directional Light
struct DLIGHT {
	float4 light_direction;
};

//Point Light
struct PLIGHT {
	float4 light_position;
	float light_radius;
	float3 attenuation;
};

cbuffer constant: register(b0)
{
	row_major float4x4 m_transform;
	row_major float4x4 m_view;
	row_major float4x4 m_projection;
	float4 m_camera_position;
	float4 m_clip;
	int m_dlight_count;
	int m_plight_count;
	int v;
	int x;
	DLIGHT dlight[5];
	PLIGHT plight[5];
};

struct MATERIAL
{
	float3 ka;
	int has_tex;
	float3 kd;
	float shininess;
	float3 ks;
};

cbuffer material: register(b1)
{
	MATERIAL material;
};

float3 ambientLight(float3 ka, float4 tex_color) {
	return ka * tex_color.rgb;
}

float3 diffuseLight(float3 kd, float4 tex_color, float3 light_dir, float3 normal, float attenuation) {
	float amount_diffuse_light = max(0, dot(light_dir, normal));
	return (kd * tex_color.rgb * amount_diffuse_light) / attenuation;
}

float3 specularLight(float3 ks, float4 tex_color, float3 light_dir, float3 normal, float3 direction_to_camera, float shininess, float attenuation) {
	float3 reflected_light = reflect(light_dir, normal);
	float amount_specular_light = pow(max(0.0, dot(reflected_light, direction_to_camera)), shininess);
	return (ks * amount_specular_light) / attenuation;
}

float3 calculateDirectional(PS_INPUT input, MATERIAL material, DLIGHT dlight)
{
	float4 d_tex_color = (material.has_tex & 2) == 0 ? float4(1, 1, 1, 1) : map_kd.Sample(map_kd_sampler, 1 - input.texcoord);
	float4 s_tex_color = (material.has_tex & 4) == 0 ? float4(1, 1, 1, 1) : map_ks.Sample(map_ks_sampler, 1 - input.texcoord);

	//DIFFUSE LIGHT

	float3 diffuse_light = diffuseLight(material.kd, d_tex_color, dlight.light_direction, input.normal, 1.0);

	//SPECULAR LIGHT
	float3 direction_to_camera = -normalize(input.world_pos.xyz - m_camera_position.xyz);

	float3 specular_light = specularLight(material.ks, s_tex_color, dlight.light_direction, input.normal, direction_to_camera, material.shininess, 1.0);

	float3 final_light = diffuse_light + specular_light;

	return final_light;
}

float3 calculatePoint(PS_INPUT input, MATERIAL material, PLIGHT plight)
{
	float4 d_tex_color = (material.has_tex & 2) == 0 ? float4(1, 1, 1, 1) : map_kd.Sample(map_kd_sampler, 1 - input.texcoord);
	float4 s_tex_color = (material.has_tex & 4) == 0 ? float4(1, 1, 1, 1) : map_ks.Sample(map_ks_sampler, 1 - input.texcoord);

	//DIFFUSE LIGHT
	float3 light_dir = normalize(plight.light_position.xyz - input.world_pos.xyz);
	float distance_light_object = length(plight.light_position.xyz - input.world_pos.xyz);

	float fade_area = max(0, distance_light_object - plight.light_radius);

	float constant_func = plight.attenuation.x;
	float linear_func = plight.attenuation.y;
	float quadratic_func = plight.attenuation.z;

	float attenuation = constant_func + linear_func * fade_area + quadratic_func * fade_area * fade_area;

	float3 diffuse_light = diffuseLight(material.kd, d_tex_color, light_dir, input.normal, attenuation);

	//SPECULAR LIGHT
	float3 direction_to_camera = normalize(input.world_pos.xyz - m_camera_position.xyz);

	float3 specular_light = specularLight(material.ks, s_tex_color, light_dir, input.normal, direction_to_camera, material.shininess, attenuation);

	float3 final_light = diffuse_light + specular_light;

	return final_light;
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
	clip(dot(m_clip, float4(input.world_pos, 1)));

	float4 tex_color = (material.has_tex & 1) == 0 ? float4(0.2, 0.2, 0.2, 1) : map_ka.Sample(map_ka_sampler, 1 - input.texcoord);

	float3 ambient_light = ambientLight(material.ka, tex_color);

	float3 light = ambient_light;

	for (int i = 0; i < m_dlight_count; i++) {
		light += calculateDirectional(input, material, dlight[i]) * 0.5;
	}
	for (int i = 0; i < m_plight_count; i++) {
		light += calculatePoint(input, material, plight[i]);
	}

	return float4(light, 1.0);
}