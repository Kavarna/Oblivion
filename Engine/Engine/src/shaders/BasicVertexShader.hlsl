

cbuffer cbCamera : register( b0 )
{
	float4x4 WVP;
};


float4 main( float3 pos : POSITION ) : SV_POSITION
{
	float4 posH = mul(float4(pos,1.0f),WVP);
	return posH;
}