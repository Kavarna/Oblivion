#pragma once

#include "../../common/common.h"
#include "../../common/commonmath.h"
#include "BatchRenderer.h"



enum DebugFlags
{
	DBG_DRAW_BOUNDING_BOX		= 0b00001,
	DBG_DRAW_BOUNDING_FRUSTUM	= 0b00010
};

struct SColorScheme
{
	DirectX::XMFLOAT4 boundingBoxColor;
	DirectX::XMFLOAT4 boundingFrustumColor;

	SColorScheme() :
		boundingBoxColor(DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)),
		boundingFrustumColor(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f))
	{};
};

class DebugDraw
{
public:
	DebugDraw() = default;
	DebugDraw(unsigned int debugFlags, SColorScheme colorScheme = SColorScheme()) :
		m_debugFlags(debugFlags), m_colorScheme(colorScheme)
	{
		BuildBatchRenderers();
	};

private:
	static std::once_flag						m_singletoneFlag;
	static std::unique_ptr<DebugDraw>			m_singletoneInstance;
public:
	static DebugDraw*							Get();


public:
	void							Begin();
	void							RenderBoundingBox(DirectX::BoundingBox const&);
	void							RenderBoundingFrustum(DirectX::BoundingFrustum const&);
	void							End(ICamera * cam);

public:
	bool							RenderBoundingBox() const { return m_debugFlags & DBG_DRAW_BOUNDING_BOX; };
	bool							RenderBoundingFrustum() const { return m_debugFlags & DBG_DRAW_BOUNDING_FRUSTUM; };

	void							ToggleFlag(int flag)
	{
		m_debugFlags ^= flag;
	};
	void							SetFlag(int flag)
	{
		m_debugFlags |= flag;
	}

public:
	void							SetDebugFlags(unsigned int debugFlags) { m_debugFlags = debugFlags; BuildBatchRenderers(); };
	void							SetColorScheme(SColorScheme const& colorScheme) { m_colorScheme = colorScheme; BuildBatchRenderers(); };
	int								GetDebugFlags() { return m_debugFlags; };
	SColorScheme&					GetColorScheme() { return m_colorScheme; };

public:
	void							BuildBatchRenderers();

private:
	unsigned int					m_debugFlags;
	SColorScheme					m_colorScheme;
	bool							m_insideBeginEnd;

	std::unique_ptr<BatchRenderer>	m_batchRenderer;
};