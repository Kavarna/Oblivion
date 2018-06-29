

cbuffer cbCamera : register( b0 )
{
	float4x4 VP;
};


float4 main( float3 pos : POSITION, row_major float4x4 world : WORLDMATRIX ) : SV_POSITION
{
	float4x4 WVP = mul(world, VP);
	float4 posH = mul(float4(pos,1.0f),WVP);
	return posH;
}