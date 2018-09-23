#pragma once

#include "../graphics/Helpers/BatchRenderer.h"
#include "../common/interfaces/Singletone.h"
#include "World.h"

class btDebugDraw : public btIDebugDraw, public Singletone<btDebugDraw>
{
public:
	btDebugDraw();
	~btDebugDraw();

	void Render();

public:
	void	drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
	void    drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override;
	void	drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB,
		btScalar distance, int lifeTime, const btVector3& color) override;
	void	reportErrorWarning(const char* warningString) override;
	void	draw3dText(const btVector3& location, const char* textString) override;
	void	setDebugMode(int debugMode) override;
	int		getDebugMode() const override;
	void	clearLines() override;
	void	flushLines() override;

public:
	void	EnableDebug();
	void	DisableDebug();
	void	ToggleDebug();
	bool	isEnabled() const { return m_debugActivated; };

	/*
	enum	DebugDrawModes
	{
		DBG_NoDebug=0,
		DBG_DrawWireframe = 1, <- Wireframe
		DBG_DrawAabb=2, <- AABB
		DBG_DrawFeaturesText=4,
		DBG_DrawContactPoints=8,
		DBG_NoDeactivation=16,
		DBG_NoHelpText = 32,
		DBG_DrawText=64,
		DBG_ProfileTimings = 128,
		DBG_EnableSatComparison = 256,
		DBG_DisableBulletLCP = 512,
		DBG_EnableCCD = 1024,
		DBG_DrawConstraints = (1 << 11), <- Draw constraints
		DBG_DrawConstraintLimits = (1 << 12), <- Draw constraint limits
		DBG_FastWireframe = (1<<13),
		DBG_DrawNormals = (1<<14), <- Draw normals
		DBG_DrawFrames = (1<<15),
		DBG_MAX_DEBUG_DRAW_MODE <- MAX_DEBUG_MODE
	};
	*/
	bool							GetFlagState(int flag)
	{
		return bool((m_debugMode & flag) == flag);
	}
	void							ToggleFlag(int flag)
	{
		m_debugMode ^= flag;
		if (flag == DBG_MAX_DEBUG_DRAW_MODE)
			if (m_debugMode == 0) m_debugMode = 0;
			else m_debugMode = DBG_MAX_DEBUG_DRAW_MODE;
	};
	void							SetFlag(int flag)
	{
		m_debugMode |= flag;
	}

private:
	std::unique_ptr<BatchRenderer>	m_renderer;
	int								m_debugMode;
	bool							m_debugActivated = false;
};

typedef btDebugDraw PhysicsDebugDraw;