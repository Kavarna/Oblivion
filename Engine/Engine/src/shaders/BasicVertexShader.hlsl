

cbuffer cbCamera : register( b0 )
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
};


float4 main( float4 pos : POSITION ) : SV_POSITION
{
	float4x4 WVP = mul(World,View);
	WVP = mul(WVP, Projection);
	return mul(pos, WVP);
}