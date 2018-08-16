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

	local polygon1 = {
		coords2D(100, 150),
		coords2D(120, 50),
		coords2D(200, 80),
		coords2D(140, 210),
		coords2D(100, 150)
	};

	local polygon2 = {
		coords2D(100, 200),
		coords2D(120, 250),
		coords2D(60, 300),
		coords2D(100, 200)
	};

	local polygon3 = {
		coords2D(200, 260),
		coords2D(220, 150),
		coords2D(300, 200),
		coords2D(350, 320),
		coords2D(200, 260)
	};

	local polygon4 = {
		coords2D(340, 60),
		coords2D(360, 40),
		coords2D(370, 70),
		coords2D(340, 60)
	};

	local polygon5 = {
		coords2D(450, 190),
		coords2D(560, 170),
		coords2D(540, 270),
		coords2D(430, 290),
		coords2D(450, 190)
	};

	local polygon6 = {
		coords2D(400, 95),
		coords2D(580, 50),
		coords2D(480, 150),
		coords2D(400, 95)
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

	for i = 1, (#polygon1 - 1) do
		local line = {
			from = polygon1[i],
			to = polygon1[i + 1]
		};
		table.insert(level1.lines, line);
		local point = polygon1[i];
		table.insert(level1.points, point);
	end

	for i = 1, (#polygon2 - 1) do
		local line = {
			from = polygon2[i],
			to = polygon2[i + 1]
		};
		table.insert(level1.lines, line);
		local point = polygon2[i];
		table.insert(level1.points, point);
	end
	
	for i = 1, (#polygon3 - 1) do
		local line = {
			from = polygon3[i],
			to = polygon3[i + 1]
		};
		table.insert(level1.lines, line);
		local point = polygon3[i];
		table.insert(level1.points, point);
	end

	for i = 1, (#polygon4 - 1) do
		local line = {
			from = polygon4[i],
			to = polygon4[i + 1]
		};
		table.insert(level1.lines, line);
		local point = polygon4[i];
		table.insert(level1.points, point);
	end

	for i = 1, (#polygon5 - 1) do
		local line = {
			from = polygon5[i],
			to = polygon5[i + 1]
		};
		table.insert(level1.lines, line);
		local point = polygon5[i];
		table.insert(level1.points, point);
	end

	for i = 1, (#polygon6 - 1) do
		local line = {
			from = polygon6[i],
			to = polygon6[i + 1]
		};
		table.insert(level1.lines, line);
		local point = polygon6[i];
		table.insert(level1.points, point);
	end

end

return level1