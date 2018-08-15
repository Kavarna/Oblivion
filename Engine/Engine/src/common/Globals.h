#pragma once

#include "common.h"

class Camera;
class Projection;

extern uint32_t							g_drawCalls;
extern bool								g_isDeveloper;
extern std::unique_ptr<Camera>			g_camera;
extern std::unique_ptr<Projection>		g_screen;