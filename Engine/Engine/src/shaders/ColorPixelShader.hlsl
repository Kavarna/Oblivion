
struct PSIn
{
	float4 PositionH : SV_POSITION;
	float4 Color : TEXCOORD;
};

float4 main(PSIn input) : SV_TARGET
{
	return input.Color;
}