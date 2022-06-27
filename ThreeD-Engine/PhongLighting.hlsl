Texture2D map_ka : register(t0);
sampler map_ka_sampler : register(s0);
Texture2D map_kd : register(t1);
sampler map_kd_sampler : register(s1);
Texture2D map_ks : register(t2);
sampler map_ks_sampler : register(s2);

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
	float2 screen_size;
	DLIGHT dlight[5];
	PLIGHT plight[5];
	float4 fog_color;
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

float3 calculateDirectional(MATERIAL material, DLIGHT dlight, float3 world_pos, float3 normal)
{
	float4 d_tex_color = float4(1, 1, 1, 1);
	float4 s_tex_color = float4(1, 1, 1, 1);

	//DIFFUSE LIGHT

	float3 diffuse_light = diffuseLight(material.kd, d_tex_color, dlight.light_direction, normal, 1.0);

	//SPECULAR LIGHT
	float3 direction_to_camera = -normalize(world_pos.xyz - m_camera_position.xyz);

	float3 specular_light = specularLight(material.ks, s_tex_color, dlight.light_direction, normal, direction_to_camera, material.shininess, 1.0);

	float3 final_light = diffuse_light + specular_light;

	return final_light;
}

float3 calculatePoint(MATERIAL material, PLIGHT plight, float3 world_pos, float3 normal)
{
	float4 d_tex_color = float4(1, 1, 1, 1);
	float4 s_tex_color = float4(1, 1, 1, 1);

	//DIFFUSE LIGHT
	float3 light_dir = normalize(plight.light_position.xyz - world_pos.xyz);
	float distance_light_object = length(plight.light_position.xyz - world_pos.xyz);

	float fade_area = max(0, distance_light_object - plight.light_radius);

	float constant_func = plight.attenuation.x;
	float linear_func = plight.attenuation.y;
	float quadratic_func = plight.attenuation.z;

	float attenuation = constant_func + linear_func * fade_area + quadratic_func * fade_area * fade_area;

	float3 diffuse_light = diffuseLight(material.kd, d_tex_color, light_dir, normal, attenuation);

	//SPECULAR LIGHT
	float3 direction_to_camera = normalize(world_pos.xyz - m_camera_position.xyz);

	float3 specular_light = specularLight(material.ks, s_tex_color, light_dir, normal, direction_to_camera, material.shininess, attenuation);

	float3 final_light = diffuse_light + specular_light;

	return final_light;
}
