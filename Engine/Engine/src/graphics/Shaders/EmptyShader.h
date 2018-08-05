#pragma once



#include "../../common/common.h"
#include "../Helpers/Lights.h"
#include "../interfaces/Shader.h"
#include "../interfaces/ICamera.h"


// Used to clear the pipeline (after using Displacement Mapping for example)
class EmptyShader :
	public IShader
{
public:
	EmptyShader() {};
	~EmptyShader() {};

	MAKE_SINGLETONE(EmptyShader);

public:
	void Create() override
	{}
	const Pipeline GetPipelineType() const	override
	{
		return Pipeline::PipelineNone;
	}
};