#include "Shader.h"

std::type_index IShader::m_currentlyBoundShader = std::type_index(typeid(nullptr));