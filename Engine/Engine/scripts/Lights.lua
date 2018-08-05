lights = {

	type = "game script";

	onLoad = function()
		angle = 0;
		r = 0.5; deltar = 1;
		g = 0.5; deltag = -1;
		b = 0.5; deltab = 1;
		sun = Oblivion.Sun();
		x = math.sin(angle);
		y = math.cos(angle);
		sun:setDirection(x,-y,0.0);
		sun:setDiffuseColor(r,g,b);
		sun:setAmbientColor(0.2,0.2,0.2);
		Oblivion.SetSun(sun);
	end,

	onUpdate = function(frameTime)
		
		r = r + deltar * frameTime * 0.2;
		if r <= 0 or r >= 1 then
			deltar = deltar * (-1);
		end

		g = g + deltag * frameTime * 0.1;
		if g <= 0 or g >= 1 then
			deltag = deltag * (-1);
		end

		b = b + deltab * frameTime * 0.01;
		if b <= 0 or b >= 1 then
			deltab = deltab * (-1);
		end

		angle = angle + frameTime;
		x = math.sin(angle);
		x = math.abs(x);
		y = math.cos(angle);
		y = -math.abs(y);
		sun:setDirection(x,y,0.0);
		sun:setDiffuseColor(r,g,b);
		Oblivion.SetSun(sun);
	end
}
