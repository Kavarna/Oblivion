#include "GraphicsDebugDraw.h"
#include "../Pipelines/ColorPipeline.h"

std::once_flag	GraphicsDebugDraw::m_singletoneFlag;
std::unique_ptr<GraphicsDebugDraw>	GraphicsDebugDraw::m_singletoneInstance;

GraphicsDebugDraw * GraphicsDebugDraw::Get()
{
	std::call_once(m_singletoneFlag, [&] { m_singletoneInstance = std::make_unique<GraphicsDebugDraw>(); });
	return m_singletoneInstance.get();
}

void GraphicsDebugDraw::Begin()
{
	m_batchRenderer->Begin();
	m_insideBeginEnd = true;
}

void GraphicsDebugDraw::RenderBoundingBox(DirectX::BoundingBox const & box)
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

void GraphicsDebugDraw::RenderBoundingFrustum(DirectX::BoundingFrustum const & frustum)
{
	if (!m_insideBeginEnd)
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

void GraphicsDebugDraw::End(ICamera * cam)
{
	m_batchRenderer->End<ColorPipeline>(cam);
	m_insideBeginEnd = false;
}

void GraphicsDebugDraw::BuildBatchRenderers()
{
	m_batchRenderer = std::make_unique<BatchRenderer>();
	m_batchRenderer->Create();
}
