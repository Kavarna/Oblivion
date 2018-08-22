tree = {
	type = "model",
	path = "Resources/LowPolyTree",
	instances = 1,
	mass = 0,
	onLoad = function(Entity)
			--Entity:Scale1(0, 0.50)
			--Entity:Translate(0, 0, 0, -50);
		end,
	onRenderCall = function(Entity)
			Entity:setCamera(Oblivion.World);
			Entity:setPipeline(Oblivion.Pipeline.TextureLight)
		end
}