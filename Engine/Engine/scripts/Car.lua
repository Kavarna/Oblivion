car = {
	type = "model",
	path = "Resources/Truck",
	instances = 1,
	onLoad = function(Entity)
			Entity:Scale1(0, 0.50)
			Entity:Translate(0, 0, 0, -50);
			--Entity:setCamera(Oblivion.Camera);
			--Entity:setPipeline(Oblivion.Pipeline.DisplacementTextureLight);
		end,
	onRenderCall = function(Entity)
			--Entity:setCamera(Oblivion.Camera);
			--Entity:setPipeline(Oblivion.Pipeline.Texture)
		end
}