#pragma once

#include "../../common/commonmath.h"
#include "../BatchRenderer.h"

namespace RenderHelper
{
	/// <summary>Renders a frustum; Any states should be set outside this function</summary>
	/// <param name="beginEnd">If true it will begin / end a cycle in render batch</param>
	inline void RenderFrustum(ICamera * cam, DirectX::BoundingFrustum const& frustum,
		bool beginEnd = false, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f))
	{
		DirectX::XMFLOAT3 corners[8];
		frustum.GetCorners(corners);
		auto batch = BatchRenderer::Get();

		if (beginEnd)
			batch->Begin();

		auto vertex = [&](DirectX::XMFLOAT3 & point)
		{
			batch->Vertex(point, DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
		};

		vertex(corners[0]);
		vertex(corners[1]);
		vertex(corners[0]);
		vertex(corners[3]);
		vertex(corners[0]);
		vertex(corners[4]);
		vertex(corners[1]);
		vertex(corners[2]);
		vertex(corners[1]);
		vertex(corners[5]);
		vertex(corners[2]);
		vertex(corners[3]);
		vertex(corners[2]);
		vertex(corners[6]);
		vertex(corners[3]);
		vertex(corners[7]);
		vertex(corners[4]);
		vertex(corners[5]);
		vertex(corners[5]);
		vertex(corners[6]);
		vertex(corners[6]);
		vertex(corners[7]);
		vertex(corners[4]);
		vertex(corners[7]);

		if (beginEnd)
			batch->End(cam, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}

	/// <summary>Renders a bounding box; Any states should be set outside this function</summary>
	/// <param name="beginEnd">If true it will begin / end a cycle in render batch</param>
	inline void RenderBoundingBox(ICamera * cam, DirectX::BoundingBox const& box,
		bool beginEnd = false, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f))
	{
		auto batch = BatchRenderer::Get();

		if (beginEnd)
			batch->Begin();

		auto vertex = [&](DirectX::XMFLOAT3 & point)
		{
			batch->Vertex(point, color);
		};

		DirectX::XMFLOAT3 corners[8];
		box.GetCorners(corners);

		vertex(corners[0]);
		vertex(corners[1]);
		vertex(corners[0]);
		vertex(corners[3]);
		vertex(corners[0]);
		vertex(corners[4]);
		vertex(corners[1]);
		vertex(corners[2]);
		vertex(corners[1]);
		vertex(corners[5]);
		vertex(corners[2]);
		vertex(corners[3]);
		vertex(corners[2]);
		vertex(corners[6]);
		vertex(corners[3]);
		vertex(corners[7]);
		vertex(corners[4]);
		vertex(corners[5]);
		vertex(corners[5]);
		vertex(corners[6]);
		vertex(corners[6]);
		vertex(corners[7]);
		vertex(corners[4]);
		vertex(corners[7]);

		if (beginEnd)
			batch->End(cam, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}
}