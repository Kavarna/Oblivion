sphere = {
	type = "Model";
	path = "sphere";
	instances = 3;
	collisionsEnabled = true;

	onLoad = function(Entity)
		Entity:Translate(0, 0, 100, -50);
		Entity:Translate(1, 0, 70, -51);
		Entity:Translate(2, 0, 80, -52);
		--tex = Oblivion.Texture("Resources/grass.dds");
		--Oblivion.TextureViewFunction(Oblivion.ToTextureView(tex));
	end,
	onUpdate = function(Entity, frametime)
	end,
	onRenderCall = function(Entity)
		Entity:setCamera(Oblivion.World);
		Entity:setPipeline(Oblivion.Pipeline.DisplacementTextureLight);
	end
}