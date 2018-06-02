

cbuffer cbCamera : register( b0 )
{
	float4x4 WVP;
};


float4 main( float4 pos : POSITION ) : SV_POSITION
{
	float4 posH = mul(pos,WVP);
	return posH;
}