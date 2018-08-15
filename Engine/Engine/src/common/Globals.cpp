#include "Globals.h"

#include "../graphics/Camera.h"
#include "../graphics/Projection.h"

uint32_t	g_drawCalls;
bool		g_isDeveloper = false;

std::unique_ptr<Camera>			g_camera;
std::unique_ptr<Projection>		g_screen;