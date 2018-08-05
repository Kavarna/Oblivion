defaultscript = {
	type = "Model";
	path = "Resources/OffRoadCar";


	onLoad = function(Entity)
		angle = 0.0
		--Log("\n\nEntity " .. Oblivion.Pipeline.TextureLight .. "\n\n\n");
		--Log("\n\n" .. angle .. "\n\n\n");
	end;
	onUpdate = function(Entity,frameTime)
		--Entity:RotateY(10 * frameTime);
		angle = angle + 1.0 * frameTime;
		Entity:Identity();
		Entity:RotateY(angle);
		if (angle > 5.0 and angle < 15.0) then
			render = false;
		else
			render = true;
		end
	end;
	onRender = function(Entity)
		return render;
	end;
	onRenderCall = function(Entity) -- Could be used to set render states
	end;
}
