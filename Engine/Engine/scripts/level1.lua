converter = require("scripts/Conversions");

--coords2D = converter.convert3Dto2D;
function coords2D(x,y)

	scaleX = Oblivion.WindowWidth / 640;
	scaleY = Oblivion.WindowHeight / 360;
	--scaleX = 1;
	--scaleY = 1;

	x = x * scaleX;
	y = y * scaleY;

	return converter.convert3Dto2D(x,y);
	--return Oblivion.float3(x,y,0);

end

function translatePoint(point, where)
	local locPoint = Oblivion.float3(0,0,0);
	locPoint.x = point.x + where.x;
	locPoint.y = point.y + where.y;
	return locPoint;
end

local level1 = {
	-- Ideal for 640 x 360
	points = {};
	lines = {};
}

function level1.Load()

	for k,v in pairs(level1.lines) do level1.lines[k]=nil end
	for k,v in pairs(level1.points) do level1.points[k]=nil end

	local border = {
		coords2D(0,0),
		coords2D(640,0),
		coords2D(640,360),
		coords2D(0,360),
		coords2D(0,0)
	};

	local L = {
		coords2D(0, 0),
		coords2D(0, 150),
		coords2D(100, 150),
		coords2D(100, 130),
		coords2D(25, 130),
		coords2D(25, 0),
		coords2D(0, 0)
	};

	local M = {
		coords2D(0, 150),
		coords2D(0, 0),
		coords2D(30, 0),
		coords2D(70, 120),
		coords2D(120, 0),
		coords2D(150, 0),
		coords2D(150, 150),
		coords2D(130, 150),
		coords2D(130, 20),
		coords2D(80, 150),
		coords2D(60, 150),
		coords2D(20, 30),
		coords2D(20, 150),
		coords2D(0, 150)
	};

	local A = {
		coords2D(0,150),
		coords2D(70,0),
		coords2D(150,150),
		coords2D(135,150),
		coords2D(90,60),
		coords2D(55,60),
		coords2D(15,150),
		coords2D(0,150)
	};

	for i = 1, (#border - 1) do
		local line = {
			from = border[i],
			to = border[i + 1]
		};
		table.insert(level1.lines, line);
		local point = border[i];
		table.insert(level1.points, point);
	end

	--[[for i = 1, (#L - 1) do
		local line = {
			from = L[i],
			to = L[i + 1]
		};
		table.insert(level1.lines, line);
		local point = L[i];
		table.insert(level1.points, point);
	end]]

	for i = 1, (#L - 1) do
		
		where = {
			x = 100,
			y = -100
		}
		local point0 = translatePoint(L[i],where);
		local point1 = translatePoint(L[i + 1],where);
		local line = {
			from = point0,
			to = point1
		};
		table.insert(level1.lines, line);
		table.insert(level1.points, point0);
	end

	for i = 1, (#M - 1) do
		
		where = {
			x = 260,
			y = -100
		}
		local point0 = translatePoint(M[i],where);
		local point1 = translatePoint(M[i + 1],where);
		local line = {
			from = point0,
			to = point1
		};
		table.insert(level1.lines, line);
		table.insert(level1.points, point0);
	end

	for i = 1, (#A - 1) do
		
		where = {
			x = 470,
			y = -100
		}
		local point0 = translatePoint(A[i],where);
		local point1 = translatePoint(A[i + 1],where);
		local line = {
			from = point0,
			to = point1
		};
		table.insert(level1.lines, line);
		table.insert(level1.points, point0);
	end

end

return level1