#pragma once


#include "common/common.h"

#include "graphics/Direct3D11.h"
#include "input/Mouse.h"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "graphics/Helpers/GraphicsDebugDraw.h"
#include "graphics/Shaders/BasicShader.h"
#include "graphics/Shaders/TextureLightShader.h"
#include "graphics/Shaders/EmptyShader.h"
#include "graphics/Camera.h"
#include "graphics/Projection.h"
#include "graphics/Model.h"
#include "graphics/RenderTexture.h"
#include "graphics/Square.h"


#include "scripting/Script.h"
#include "scripting/Entity.h"
#include "scripting/GameScript.h"

#include "physics/World.h"
#include "physics/CollisionObject.h"
#include "physics/PhysicsDebugDraw.h"