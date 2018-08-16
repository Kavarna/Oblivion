#include "commonmath.h"
#include "../scripting/LuaManager.h"

void Math::LuaRegister()
{
	US_NS_LUA;
	getGlobalNamespace(g_luaState.get())
		.beginNamespace("Oblivion")
			.beginClass<DirectX::XMFLOAT2>("float2")
				.addConstructor<void(*)(float, float)>()
				.addData("x", &DirectX::XMFLOAT2::x,true)
				.addData("y", &DirectX::XMFLOAT2::y,true)
			.endClass()
			.beginClass<DirectX::XMFLOAT3>("float3")
				.addConstructor<void(*)(float, float, float)>()
				.addData("x", &DirectX::XMFLOAT3::x, true)
				.addData("y", &DirectX::XMFLOAT3::y, true)
				.addData("z", &DirectX::XMFLOAT3::z, true)
			.endClass()
			.beginClass<DirectX::XMFLOAT4>("float4")
				.addConstructor<void(*)(float, float, float, float)>()
				.addData("x", &DirectX::XMFLOAT4::x,true)
				.addData("y", &DirectX::XMFLOAT4::y, true)
				.addData("z", &DirectX::XMFLOAT4::z,true)
				.addData("w", &DirectX::XMFLOAT4::w,true)
			.endClass()
		.endNamespace();
}
