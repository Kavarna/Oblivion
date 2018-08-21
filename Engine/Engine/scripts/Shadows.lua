level1 = require("scripts/level1");
converter = require("scripts/Conversions");

function getIntersection(ray,segment)
	-- RAY in parametric: Point + Direction*T1
	local r_px = ray.from.x;
	local r_py = ray.from.y;
	local r_dx = ray.to.x-ray.from.x;
	local r_dy = ray.to.y-ray.from.y;
	-- SEGMENT in parametric: Point + Direction*T2
	local s_px = segment.from.x;
	local s_py = segment.from.y;
	local s_dx = segment.to.x-segment.from.x;
	local s_dy = segment.to.y-segment.from.y;
	-- Are they parallel? If so, no intersect
	local r_mag = math.sqrt(r_dx*r_dx+r_dy*r_dy);
	local s_mag = math.sqrt(s_dx*s_dx+s_dy*s_dy);
	if(r_dx/r_mag==s_dx/s_mag and r_dy/r_mag==s_dy/s_mag) then -- Directions are the same.
		return nil;
	end
	-- SOLVE FOR T1 & T2
	-- r_px+r_dx*T1 = s_px+s_dx*T2 && r_py+r_dy*T1 = s_py+s_dy*T2
	-- ==> T1 = (s_px+s_dx*T2-r_px)/r_dx = (s_py+s_dy*T2-r_py)/r_dy
	-- ==> s_px*r_dy + s_dx*T2*r_dy - r_px*r_dy = s_py*r_dx + s_dy*T2*r_dx - r_py*r_dx
	-- ==> T2 = (r_dx*(s_py-r_py) + r_dy*(r_px-s_px))/(s_dx*r_dy - s_dy*r_dx)
	local T2 = (r_dx*(s_py-r_py) + r_dy*(r_px-s_px))/(s_dx*r_dy - s_dy*r_dx);
	local T1 = (s_px+s_dx*T2-r_px)/r_dx;
	-- Must be within parametic whatevers for RAY/SEGMENT
	if(T1<0) then return nil end;
	if(T2<0 or T2>1) then return nil end;
	-- Return the POINT OF INTERSECTION
	return {
		x = r_px+r_dx*T1,
		y = r_py+r_dy*T1,
		param = T1
	};
end

game = {
	type = "game script",

	onSize = function()
		halfWidth = Oblivion.WindowWidth / 2;
		halfHeight = Oblivion.WindowHeight / 2;
		level1.Load();
		Log("Oblivion.WindowWidth = " .. Oblivion.WindowWidth .. "; Oblivion.WindowHeight = " .. Oblivion.WindowHeight .. "\n");
		center = converter.convert3Dto2D(halfWidth, halfHeight);
		screenSpaceCenter = converter.convert3Dto2D(halfWidth,halfHeight);
	end,

	onLoad = function()
		epsilon = 0.00001;
		renderer = Oblivion.BatchRenderer();
		renderer:Create();
		white = Oblivion.float4(1,1,1,1);
		red = Oblivion.float4(1,0,0,1);
		yellow = Oblivion.float4(1,1,0,1);
		green = Oblivion.float4(0,1,0,1);
		black = Oblivion.float4(0,0,0,1);
		game.onSize();
		mouseCoords = converter.convert3Dto2D(0, 0);
	end,

	onUpdate = function()
		mouseCoords = converter.convert3Dto2D(Oblivion.MouseX, Oblivion.MouseY);
		intersectionPoints = {};

		local uniquePoints = {};
		for i = 1, #level1.points do
			local locX = level1.points[i].x - mouseCoords.x;
			local locY = level1.points[i].y - mouseCoords.y;
			local angle = math.atan2(locY, locX);
			table.insert(uniquePoints,angle - epsilon);
			table.insert(uniquePoints,angle);
			table.insert(uniquePoints,angle + epsilon);
		end

		table.sort(uniquePoints, function (p1, p2)
										return p1 > p2;
									end);

		for i = 1, #uniquePoints do
			local angle = uniquePoints[i];

			local dx = math.cos(angle);
			local dy = math.sin(angle);

			local ray = {
				from = {
					x = mouseCoords.x,
					y = mouseCoords.y
				},
				to = {
					x = mouseCoords.x + dx,
					y = mouseCoords.y + dy
				}
			};
			local closestIntersect = {
				x = 0,
				y = 0,
				param = nil
			}
			for j = 1, #level1.lines do
				local intersection = getIntersection(ray,level1.lines[j]);
				if intersection then
					if (closestIntersect.param == nil or intersection.param < closestIntersect.param)then
						closestIntersect = intersection;
					end
				end
			end


			table.insert(intersectionPoints, closestIntersect);

		end
	end,

	onRender = function()
		renderer:Begin();

		for i = 1, (#level1.lines) do
			renderer:Point(level1.lines[i].from,white);
			renderer:Point(level1.lines[i].to,white);
		end

		renderer:End(Oblivion.Screen,Oblivion.TopologyLineList);

		if (intersectionPoints) then
			
			renderer:Begin();


			for i = 1, (#intersectionPoints - 1) do
				local intersection0 = Oblivion.float3(intersectionPoints[i].x, intersectionPoints[i].y, 0);
				renderer:Point(intersection0,white);
				local intersection1 = Oblivion.float3(intersectionPoints[i + 1].x, intersectionPoints[i + 1].y, 0);
				renderer:Point(intersection1,white);
				renderer:Point(mouseCoords,white);
				--Log("intersectionPoints[ ".. i .. " ] = (" .. intersectionPoints[i].x .. ", " .. intersectionPoints[i].y .. ", " .. intersectionPoints[i].param .. ")\n");
			end
			local intersection0 = Oblivion.float3(intersectionPoints[#intersectionPoints].x, intersectionPoints[#intersectionPoints].y, 0);
			renderer:Point(intersection0,white);
			local intersection1 = Oblivion.float3(intersectionPoints[1].x, intersectionPoints[1].y, 0);
			renderer:Point(intersection1,white);
			renderer:Point(mouseCoords,white);

			renderer:End(Oblivion.Screen,Oblivion.TopologyTriangleList);
			--Oblivion.Sleep(1000);
		end


	end

}
