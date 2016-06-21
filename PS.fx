struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	float4 normal : NORMAL;
};

static float4 lightDir = { 1,-1,0,0 };
static float4 lightDiffuse = { 1,1,1,1 };
static float4 lightAmbient = { 0.6f,0.6f,0.6f,1.0f };

float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 lightVector = -normalize(lightDir);
	float diffuseFactor = saturate(dot(lightVector, input.normal));

	float4 outColor = input.color * (lightDiffuse * diffuseFactor) + input.color*lightAmbient;
	return outColor;
}