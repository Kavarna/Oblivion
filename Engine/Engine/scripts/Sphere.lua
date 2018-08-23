sphere = {
	type = "Model";
	path = "cube";
	mass = 1;

	onLoad = function(Entity)
		Entity:Translate(0, 25, 50, 0);
	end,
	onRenderCall = function(Entity)
		Entity:setCamera(Oblivion.World);
		Entity:setPipeline(Oblivion.Pipeline.DisplacementTextureLight);
	end
}