#pragma once

#define UNICODE

#if !(defined UNICODE)
#define ENGINE_NAME	"Oblivion"
#else // defined UNICODE
#define ENGINE_NAME L"Oblivion"
#endif // !defined UNICODE

#if _DEBUG || DEBUG
#define CATCH catch(std::exception const& e) { \
char buffer[500]; sprintf_s(buffer, "Error: %s\n", e.what()); \
OutputDebugStringA(buffer); exit(1); }\
catch( ... ) { \
OutputDebugStringA( "Unexpected error occured\n" ); exit(1);}
#else
#define CATCH catch(std::exception const& e) { \
char buffer[500]; sprintf_s(buffer, "Error: %s", e.what());\
MessageBoxA(NULL,buffer,"Error",MB_ICONERROR| MB_OK); exit(1);}\
catch (...) {\
MessageBoxA(NULL,"Unexpected error occured", "Error", MB_ICONERROR| MB_OK); exit(1);\
}
#endif

#if DEBUG || _DEBUG
#define ThrowIfFailed(hr) if ( FAILED( hr) )\
{\
	_com_error err( hr );\
	const wchar_t* errorMessage = err.ErrorMessage( );\
	wchar_t buffer[ 500 ];\
	int line = __LINE__;\
	const char* file = __FILE__;\
	swprintf_s( buffer, L"DirectX Error occured at line %d in file %hs;\nMessage: %ws\n", line, file, errorMessage );\
	OutputDebugString( buffer );\
	throw std::exception( "DirectX Error" );\
}
#else
#define ThrowIfFailed(hr) if ( FAILED( hr) )\
{\
	_com_error err( hr );\
	const wchar_t* errorMessage = err.ErrorMessage( );\
	wchar_t buffer[ 500 ];\
	swprintf_s( buffer, L"DirectX Error occured; Message: %ws", errorMessage );\
	MessageBox( NULL, buffer, L"Error", MB_ICONERROR | MB_OK );\
	throw std::exception( "DirectX Error" );\
}
#endif

#pragma warning (push)
#pragma warning (disable : 5043)
// TODO: Add other warnings to be disabled

#include <windows.h>
#include <d3d11.h>
#include <d3d10.h>
#include <dxgi.h>
#include <D3Dcompiler.h>
#include <wrl.h>
#include <comdecl.h>
#include <comdef.h>
#include <ShObjIdl.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include <random>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <bitset>
#include <type_traits>
#include <typeinfo>
#include <typeindex>
#include <optional>

#include <OblivionObjects.h>

#include "commonmath.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3d10.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "D3Dcompiler.lib")

#pragma warning (pop)

template <class type>
using MicrosoftPointer = Microsoft::WRL::ComPtr<type>;
template <class type>
using unique_delete_ptr = std::unique_ptr<type, std::function<void(type*)>>;

namespace DX
{
	inline void OutputVDebugString(const wchar_t * format, ...)
	{
#if DEBUG || _DEBUG
		static wchar_t Sequence[1024];
		va_list args;
		va_start(args, format);
		_vsnwprintf_s(Sequence, sizeof(Sequence), format, args);
		va_end(args);
		OutputDebugStringW(Sequence);
#endif
	}
	/// <summary>Chechks if the objects exits before releasing it</summary>
	inline void SafeRelease(IUnknown *& object)
	{
		OutputVDebugString(L"Please don't use SafeRelease(); Use smart pointers instead");
		if (object)
		{
			object->Release();
			object = nullptr;
		}
	}
	/// <summary>Clamps a value between lower and upper</summary>
	template <class type>
	inline type clamp(type& x, type lower, type upper)
	{
		return max(lower, min(x, upper));
	}
	/// <summary>Returns a random number between a and b</summary>
	inline float randomNumber(float a, float b)
	{
		return a + ( float(rand()) / RAND_MAX ) * ( b - a );
	}
	/// <summary>Returns the minimum value</summary>
	template <class type>
	inline type Min(type first, type second)
	{
		return first < second ? first : second;
	}
	/// <summary>Returns the polar angle of the point (x,y) in [0, 2*PI).</summary>
	inline float AngleFromXY(float x, float y)
	{
		using namespace DirectX;
		float theta = 0.0f;

		// Quadrant I or IV
		if (x >= 0.0f)
		{
			// If x = 0, then atanf(y/x) = +pi/2 if y > 0
			//                atanf(y/x) = -pi/2 if y < 0
			theta = atanf(y / x); // in [-pi/2, +pi/2]

			if (theta < 0.0f)
				theta += 2.0f * XM_PI; // in [0, 2*pi).
		}

		// Quadrant II or III
		else
			theta = atanf(y / x) + XM_PI; // in [0, 2*pi).

		return theta;
	}
	/// <summary>Gets the component count from known simple formats (RGBA,BGRA...); -1 if it's an invalid format</summary>
	int GetComponentCountFromFormat(DXGI_FORMAT format);
	/// <summary>Initializes Direct3D states</summary>
	void InitStates(ID3D11Device*);
}

namespace COM
{
	extern MicrosoftPointer<IFileOpenDialog> g_openFileDialog;
	/// <summary>Initializes COM objects</summary>
	void InitializeComObjects();
	/// <summary>Uniintializes safely COM objects</summary>
	void UninitializeComObjects();
}
