Texture2D tex1 : register(t0);
SamplerState samp1 : register(s0);

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	const float3 sun_vector = {1, 1, 1};
	const float4 sun_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	const float ambient_strength = 0.5f;

	// normalize interpolated normal vector
	float3 norm = normalize(input.normal);

	// ambient
	float4 ambient = ambient_strength * sun_color;

	// diffuse
	float diff = max(dot(norm, sun_vector), 0.0f);
	float4 diffuse = diff * sun_color;

	// combine
	float4 lighting_color = ambient + diffuse;
	lighting_color.w = 1;

	return tex1.Sample(samp1, input.texcoord) * lighting_color;
}