#include "Shader.h"
#include "../../scripting/LuaManager.h"

US_NS_LUA;

Pipeline IShader::m_currentlyBoundPipeline = Pipeline::PipelineNone;

float kPipelineNone = PipelineNone;
float kPipelineBatchShader = PipelineBatchShader;
float kPipelineTextureBatchShader = PipelineTextureBatchShader;
float kPipelineBasic = PipelineBasic;
float kPipelineTexture = PipelineTexture;
float kPipelineTextureLight = PipelineTextureLight;
float kPipelineDisplacementTextureLight = PipelineDisplacementTextureLight;

void IShader::LuaRegister()
{
	getGlobalNamespace(g_luaState.get())
		.beginNamespace("Oblivion")
			.beginNamespace("Pipeline")
				.addVariable("None",&kPipelineNone,false)
				.addVariable("BatchShader",&kPipelineBatchShader,false)
				.addVariable("TextureBatchShader", &kPipelineBatchShader, false)
				.addVariable("Basic",&kPipelineBasic,false)
				.addVariable("Texture", &kPipelineTexture,false)
				.addVariable("TextureLight", &kPipelineTextureLight,false)
				.addVariable("DisplacementTextureLight", &kPipelineDisplacementTextureLight,false)
			.endNamespace()
		.endNamespace();
		;
}
