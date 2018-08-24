sphere = {
	type = "Model";
	path = "sphere";
	instances = 2;
	mass = 1;

	onLoad = function(Entity)
		Entity:Translate(0, 0, 50, 0);
		Entity:Translate(1, 0, 55, 0);
	end,
	onUpdate = function(Entity, frametime)
	end,
	onRenderCall = function(Entity)
		Entity:setCamera(Oblivion.World);
		Entity:setPipeline(Oblivion.Pipeline.DisplacementTextureLight);
	end
}