Texture2D reflection : register(t0);
sampler reflection_sampler : register(s0);
Texture2D refraction : register(t1);
sampler refraction_sampler : register(s1);
Texture2D dudv : register(t2);
sampler dudv_sampler : register(s2);
Texture2D normal_map : register(t3);
sampler normal_map_sampler : register(s3);
Texture2D depth_map : register(t4);
sampler depth_map_sampler : register(s4);

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float4 clip_space : TEXCOORD1;
	float3 world_pos: TEXCOORD2;
	float3 to_cam_vec : POSITION1;
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

cbuffer water: register(b1)
{
	float move_factor;
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
	clip(dot(m_clip, float4(input.world_pos, 1))-0.1);
	float2 ndc = input.clip_space.xy/input.clip_space.w / 2 + 0.5;
	float2 refraction_coord = float2(ndc.x, 1-ndc.y);
	float2 reflection_coord = float2(ndc.x, ndc.y);

	float near = 0.1;
	float far = 1000;
	float depth = depth_map.Sample(depth_map_sampler, refraction_coord).x;
	float floor_distance = 2 * near * far / (far + near - (2 * depth - 1) * (far - near));
	depth = input.position.z;
	float water_distance = 2 * near * far / (far + near - (2 * depth - 1) * (far - near));
	float water_depth = floor_distance - water_distance;

	float2 distortedTexCoords = dudv.Sample(dudv_sampler, float2(input.texcoord.x - floor(input.texcoord.x) + move_factor, input.texcoord.y - floor(input.texcoord.y))).xy * 0.1;
	distortedTexCoords = input.texcoord + float2(distortedTexCoords.x, distortedTexCoords.y + move_factor);
	distortedTexCoords -= floor(distortedTexCoords);
	float2 totalDistortion = (dudv.Sample(dudv_sampler,distortedTexCoords).xy * 2.0 - 1.0) * 0.03;

	refraction_coord += totalDistortion;
	reflection_coord += totalDistortion;

	refraction_coord = clamp(refraction_coord, 0.01, 0.99);
	reflection_coord = clamp(reflection_coord, 0.01, 0.99);

	float4 reflection_color = float4(reflection.Sample(reflection_sampler, reflection_coord).xyz, 1.0);
	float4 refraction_color = float4(refraction.Sample(refraction_sampler, refraction_coord).xyz, 1.0);

	float refractive_factor = dot(normalize(input.to_cam_vec), float3(0, 1, 0));
	refractive_factor = pow(refractive_factor, 1);

	float4 normal_color = normal_map.Sample(normal_map_sampler, distortedTexCoords);
	float3 normal = float3(normal_color.r * 2 - 1, normal_color.b, normal_color.g * 2 - 1);
	normal = normalize(normal);

	const float shineDamper = 20;
	const float reflectivity = 0.6;

	float3 specularHighlightsTotal = float3(0, 0, 0);

	for (int i = 0; i < m_dlight_count; i++) {
		float3 from_light_vec = dlight[i].light_direction;
		float3 reflectedLight = reflect(normalize(from_light_vec), normal);
		float specular = max(dot(reflectedLight, normalize(input.to_cam_vec)), 0.0);
		specular = pow(specular, shineDamper);
		float3 specularHighlights = float3(1, 1, 1) * specular * reflectivity;
		specularHighlightsTotal += specularHighlights;
	}
	for (int i = 0; i < m_plight_count; i++) {
		float3 light_dir = normalize(plight[i].light_position.xyz - input.world_pos.xyz);
		float distance_light_object = length(plight[i].light_position.xyz - input.world_pos.xyz);

		float fade_area = max(0, distance_light_object - plight[i].light_radius);

		float constant_func = plight[i].attenuation.x;
		float linear_func = plight[i].attenuation.y;
		float quadratic_func = plight[i].attenuation.z;

		float attenuation = constant_func + linear_func * fade_area + quadratic_func * fade_area * fade_area;

		float3 from_light_vec = -light_dir;
		float3 reflectedLight = reflect(normalize(from_light_vec), normal);
		float specular = max(dot(reflectedLight, normalize(input.to_cam_vec)), 0.0);
		specular = pow(specular, shineDamper);
		float3 specularHighlights = float3(1, 1, 1) * specular * reflectivity / attenuation;
		specularHighlightsTotal += specularHighlights;
	}
	

	float4 out_color = reflection_color * (1 - refractive_factor) + refraction_color * refractive_factor;

	out_color = out_color * 0.8 + float4(0.0, 0.3, 0.5, 1.0) * 0.2 + float4(specularHighlightsTotal, 0);

	out_color.a = clamp(water_depth/0.1, 0, 1);

	return out_color;
}