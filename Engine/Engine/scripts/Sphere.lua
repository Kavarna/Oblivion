sphere = {
	type = "Model";
	path = "sphere";
	instances = 2;
	collisionsEnabled = true;

	onLoad = function(Entity)
		Entity:Translate(0, 0, 100, -50);
		Entity:Translate(1, 0, 70, -51);
	end,
	onUpdate = function(Entity, frametime)
	end,
	onRenderCall = function(Entity)
		Entity:setCamera(Oblivion.World);
		Entity:setPipeline(Oblivion.Pipeline.DisplacementTextureLight);
	end
}