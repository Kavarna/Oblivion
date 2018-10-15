#pragma once

#include "../../common/interfaces/Singletone.h"
#include "../interfaces/ICamera.h"

enum PipelineEnum
{
	PipelineNone = 0,
	PipelineBatchShader = -1,
	PipelineTextureBatchShader = -2,
	PipelineSimpleTexture = -3,
	PipelineBasic = 1,
	PipelineTexture = 2,
	PipelineTextureLight = 3,
	PipelineDisplacementTextureLight = 4,
	PipelineCompute2D = 5,
	PipelineCustom = INT_MAX
};

class IPipeline
{
public:
	virtual void __vectorcall bind(ICamera * cam) const = 0;
	virtual void __vectorcall bind(DirectX::FXMMATRIX& world, ICamera * cam) const = 0;
};

