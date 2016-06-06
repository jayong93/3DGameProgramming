cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
}

cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
}

cbuffer cbColor : register(b0)
{
	float4 gcColor : packoffset(c0);
}

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

VS_OUTPUT VS(float4 pos : POSITION)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(pos, gmtxWorld);
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	return output;
}

float4 PS(float4 pos : SV_POSITION) : SV_Target
{
	return gcColor;
}