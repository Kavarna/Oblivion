#include "DebugDraw.h"

std::once_flag	DebugDraw::m_singletoneFlag;
std::unique_ptr<DebugDraw>	DebugDraw::m_singletoneInstance;

DebugDraw * DebugDraw::Get()
{
	std::call_once(m_singletoneFlag, [&] { m_singletoneInstance = std::make_unique<DebugDraw>(); });
	return m_singletoneInstance.get();
}

void DebugDraw::Begin()
{
	m_batchRenderer->Begin();
	m_insideBeginEnd = true;
}

void DebugDraw::RenderBoundingBox(DirectX::BoundingBox const & box)
{
	if (!m_insideBeginEnd)
		return;
	if (m_debugFlags & DBG_DRAW_BOUNDING_BOX)
	{
		DirectX::XMFLOAT3 corners[8];
		box.GetCorners(corners);

		auto vertex = [&](DirectX::XMFLOAT3 & point)
		{
			m_batchRenderer->Vertex(point, DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
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
	}
}

void DebugDraw::RenderBoundingFrustum(DirectX::BoundingFrustum const & frustum)
{
	if (m_insideBeginEnd)
		return;
	if (m_debugFlags & DBG_DRAW_BOUNDING_FRUSTUM)
	{
		DirectX::XMFLOAT3 corners[8];
		frustum.GetCorners(corners);

		auto vertex = [&](DirectX::XMFLOAT3 & point)
		{
			m_batchRenderer->Vertex(point, DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
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
	}
}

void DebugDraw::End(ICamera * cam)
{
	m_batchRenderer->End(cam);
	m_insideBeginEnd = false;
}

void DebugDraw::BuildBatchRenderers()
{
	m_batchRenderer = std::make_unique<BatchRenderer>();
	m_batchRenderer->Create();
}
