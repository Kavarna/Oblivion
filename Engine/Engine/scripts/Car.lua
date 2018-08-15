car = {
	type = "model",
	path = "Resources/Truck",
	instances = 1,
	onLoad = function(Entity)
			Entity:Scale1(0, 0.50)
			Entity:Translate(0, 0, 0, -50);
		end,
	onRenderCall = function(Entity)
			Entity:setCamera(Oblivion.World);
			Entity:setPipeline(Oblivion.Pipeline.Texture)
		end
}