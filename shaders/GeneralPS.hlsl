Texture2D tex1 : register(t0);
SamplerState samp1 : register(s0);

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float3 wpos : WORLD_POS;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;

	float3 camera_pos : CAMERA_POS;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	const float3 sun_vector = { 1, 1, 1 };
	const float3 sun_diff = { 1.0f, 1.0f, 1.0f };
	const float3 sun_spec = { 1.0f, 1.0f, 1.0f };
	const float3 sun_ambi = { 0.0f, 0.0f, 0.0f };

	const float3 mat0_ambi = { 0.5f, 0.5f, 0.5f };
	const float3 mat0_diff = { 1.0f, 1.0f, 1.0f };
	const float3 mat0_spec = { 1.0f, 1.0f, 1.0f };
	const float3 mat0_emis = { 0.0f, 0.0f, 0.0f };
	const float mat0_shine = 1.0f;

	// normalize interpolated normal vector
	float3 norm = normalize(input.normal);

	// diffuse
	float diff = max(dot(norm, sun_vector), 0.0f);
	float3 diffuse = diff * sun_diff * mat0_diff;
	
	// specular
	float3 reflection = 2.0f * norm * dot(norm, sun_vector) - sun_vector;
	float3 view = normalize(input.camera_pos - input.wpos);
	float spec = pow(max(dot(reflection, view), 0.0f), mat0_shine);
	if (diff <= 0.0f) spec = 0.0f;
	float3 specular = spec * sun_spec * mat0_spec;

	// ambient
	float3 ambient = mat0_ambi * sun_ambi;
	
	// emissive
	float3 emissive = mat0_emis;

	// combine
	float3 lighting_color = ambient + diffuse + specular + emissive;

	float4 result = tex1.Sample(samp1, input.texcoord)* float4(lighting_color.xyz, 1);

	if (result.w == 0.0) discard;

	return result;
}