#pragma once

#include "AlignedObject.h"

class ICamera : public AlignedObject
{
public:
	ICamera() = default;
	virtual ~ICamera() = default;

public:
	virtual void Construct() = 0;

	virtual DirectX::XMMATRIX& GetView() = 0;
	virtual DirectX::XMMATRIX& GetProjection() = 0;

};