Texture2D tex1 : register(t0);
SamplerState samp1 : register(s0);

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	return tex1.Sample(samp1, input.texcoord);
}