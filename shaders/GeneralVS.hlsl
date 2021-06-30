struct VS_INPUT {
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
};

cbuffer ConstantBuffer : register(b0) {
	row_major float4x4 m;
	row_major float4x4 mvp;
};

float3x3 ExtractRotMat(float4x4 input) {
	float3x3 result;
	result[0] = input[0].xyz;
	result[1] = input[1].xyz;
	result[2] = input[2].xyz;
	return result;
}

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;
	output.pos = mul(mvp, float4(input.pos, 1));
	output.normal = mul(ExtractRotMat(m), input.normal);
	output.texcoord = input.texcoord;
	return output;
}