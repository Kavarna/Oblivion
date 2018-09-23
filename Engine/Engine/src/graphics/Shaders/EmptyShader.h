#pragma once



#include "../../common/common.h"
#include "../../common/interfaces/Singletone.h"
#include "../Helpers/Lights.h"
#include "../interfaces/Shader.h"
#include "../interfaces/ICamera.h"



// Used to clear the pipeline (after using Displacement Mapping for example)
class EmptyShader :
	public IShader, public Singletone<EmptyShader>
{
public:
	EmptyShader() {};
	~EmptyShader() {};


public:
	void Create() override
	{}
	const Pipeline GetPipelineType() const	override
	{
		return Pipeline::PipelineNone;
	}
};