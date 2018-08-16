local Conversions = {}

function Conversions.convert3Dto2D(x,y)
	local newX = halfWidth * -1 + x;
	local newY = halfHeight - y;
	local ret = Oblivion.float3(newX,newY,0);
	return ret;
end;

function Conversions.convert2Dto3D(x,y)
	local newX = x + halfWidth;
	local newY = y - halfHeight;
	return Oblivion.float3(newX,newY,0);
end;

return Conversions;