defaultscript = {
	type = "Model";
	--path = "Resources/Truck";
	path = "grid";
	instances = 4;
	onLoad = function(Entity)
			Entity:Identity(0);
			Entity:Translate(0, 50, 0, 0);
			Entity:Identity(1);
			Entity:Translate(1, -50, 0, 0);
			Entity:Identity(2);
			Entity:Translate(2, -50, 0, -100);
			Entity:Identity(3);
			Entity:Translate(3, 50, 0, - 100)
			--Log("Oblivion.Screen = " .. Oblivion.Screen);
		end,
	onUpdate = function(Entity, frameTime)
		end,
	onRenderCall = function(Entity)
			--Entity:setCamera(Oblivion.Camera);
			Entity:setPipeline(Oblivion.Pipeline.DisplacementTextureLight)			
		end
}
