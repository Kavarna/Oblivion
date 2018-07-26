#include "common.h"

namespace DX
{
	int GetComponentCountFromFormat( DXGI_FORMAT format )
	{ // Please keep this folded
		switch ( format )
		{
		case DXGI_FORMAT_UNKNOWN:
			return -1;
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
			return 4;
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return 4;
		case DXGI_FORMAT_R32G32B32A32_UINT:
			return 4;
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return 4;
		case DXGI_FORMAT_R32G32B32_TYPELESS:
			return 3;
		case DXGI_FORMAT_R32G32B32_FLOAT:
			return 3;
		case DXGI_FORMAT_R32G32B32_UINT:
			return 3;
		case DXGI_FORMAT_R32G32B32_SINT:
			return 3;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
			return 4;
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			return 4;
		case DXGI_FORMAT_R16G16B16A16_UNORM:
			return 4;
		case DXGI_FORMAT_R16G16B16A16_UINT:
			return 4;
		case DXGI_FORMAT_R16G16B16A16_SNORM:
			return 4;
		case DXGI_FORMAT_R16G16B16A16_SINT:
			return 4;
		case DXGI_FORMAT_R32G32_TYPELESS:
			return 2;
		case DXGI_FORMAT_R32G32_FLOAT:
			return 2;
		case DXGI_FORMAT_R32G32_UINT:
			return 2;
		case DXGI_FORMAT_R32G32_SINT:
			return 2;
		case DXGI_FORMAT_R32G8X24_TYPELESS:
			break;
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			break;
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			break;
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			break;
		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
			return 4;
		case DXGI_FORMAT_R10G10B10A2_UNORM:
			return 4;
		case DXGI_FORMAT_R10G10B10A2_UINT:
			return 4;
		case DXGI_FORMAT_R11G11B10_FLOAT:
			return 3;
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			return 4;
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return 4;
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return 4;
		case DXGI_FORMAT_R8G8B8A8_UINT:
			return 4;
		case DXGI_FORMAT_R8G8B8A8_SNORM:
			return 4;
		case DXGI_FORMAT_R8G8B8A8_SINT:
			return 4;
		case DXGI_FORMAT_R16G16_TYPELESS:
			return 2;
		case DXGI_FORMAT_R16G16_FLOAT:
			return 2;
		case DXGI_FORMAT_R16G16_UNORM:
			return 2;
		case DXGI_FORMAT_R16G16_UINT:
			return 2;
		case DXGI_FORMAT_R16G16_SNORM:
			return 2;
		case DXGI_FORMAT_R16G16_SINT:
			return 2;
		case DXGI_FORMAT_R32_TYPELESS:
			return 1;
		case DXGI_FORMAT_D32_FLOAT:
			break;
		case DXGI_FORMAT_R32_FLOAT:
			return 1;
		case DXGI_FORMAT_R32_UINT:
			return 1;
		case DXGI_FORMAT_R32_SINT:
			return 1;
		case DXGI_FORMAT_R24G8_TYPELESS:
			return 2;
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			break;
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
			break;
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			break;
		case DXGI_FORMAT_R8G8_TYPELESS:
			return 2;
		case DXGI_FORMAT_R8G8_UNORM:
			return 2;
		case DXGI_FORMAT_R8G8_UINT:
			return 2;
		case DXGI_FORMAT_R8G8_SNORM:
			return 2;
		case DXGI_FORMAT_R8G8_SINT:
			return 2;
		case DXGI_FORMAT_R16_TYPELESS:
			return 1;
		case DXGI_FORMAT_R16_FLOAT:
			return 1;
		case DXGI_FORMAT_D16_UNORM:
			return 1;
		case DXGI_FORMAT_R16_UNORM:
			return 1;
		case DXGI_FORMAT_R16_UINT:
			return 1;
		case DXGI_FORMAT_R16_SNORM:
			return 1;
		case DXGI_FORMAT_R16_SINT:
			return 1;
		case DXGI_FORMAT_R8_TYPELESS:
			return 1;
		case DXGI_FORMAT_R8_UNORM:
			return 1;
		case DXGI_FORMAT_R8_UINT:
			return 1;
		case DXGI_FORMAT_R8_SNORM:
			return 1;
		case DXGI_FORMAT_R8_SINT:
			return 1;
		case DXGI_FORMAT_A8_UNORM:
			return 1;
		case DXGI_FORMAT_R1_UNORM:
			return 1;
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
			break;
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
			break;
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
			break;
		case DXGI_FORMAT_BC1_TYPELESS:
			break;
		case DXGI_FORMAT_BC1_UNORM:
			break;
		case DXGI_FORMAT_BC1_UNORM_SRGB:
			break;
		case DXGI_FORMAT_BC2_TYPELESS:
			break;
		case DXGI_FORMAT_BC2_UNORM:
			break;
		case DXGI_FORMAT_BC2_UNORM_SRGB:
			break;
		case DXGI_FORMAT_BC3_TYPELESS:
			break;
		case DXGI_FORMAT_BC3_UNORM:
			break;
		case DXGI_FORMAT_BC3_UNORM_SRGB:
			break;
		case DXGI_FORMAT_BC4_TYPELESS:
			break;
		case DXGI_FORMAT_BC4_UNORM:
			break;
		case DXGI_FORMAT_BC4_SNORM:
			break;
		case DXGI_FORMAT_BC5_TYPELESS:
			break;
		case DXGI_FORMAT_BC5_UNORM:
			break;
		case DXGI_FORMAT_BC5_SNORM:
			break;
		case DXGI_FORMAT_B5G6R5_UNORM:
			return 3;
		case DXGI_FORMAT_B5G5R5A1_UNORM:
			return 4;
		case DXGI_FORMAT_B8G8R8A8_UNORM:
			return 4;
		case DXGI_FORMAT_B8G8R8X8_UNORM:
			return 3;
		case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
			break;
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
			return 4;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return 4;
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
			break;
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			break;
		case DXGI_FORMAT_BC6H_TYPELESS:
			break;
		case DXGI_FORMAT_BC6H_UF16:
			break;
		case DXGI_FORMAT_BC6H_SF16:
			break;
		case DXGI_FORMAT_BC7_TYPELESS:
			break;
		case DXGI_FORMAT_BC7_UNORM:
			break;
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			break;
		case DXGI_FORMAT_AYUV:
			break;
		case DXGI_FORMAT_Y410:
			break;
		case DXGI_FORMAT_Y416:
			break;
		case DXGI_FORMAT_NV12:
			break;
		case DXGI_FORMAT_P010:
			break;
		case DXGI_FORMAT_P016:
			break;
		case DXGI_FORMAT_420_OPAQUE:
			break;
		case DXGI_FORMAT_YUY2:
			break;
		case DXGI_FORMAT_Y210:
			break;
		case DXGI_FORMAT_Y216:
			break;
		case DXGI_FORMAT_NV11:
			break;
		case DXGI_FORMAT_AI44:
			break;
		case DXGI_FORMAT_IA44:
			break;
		case DXGI_FORMAT_P8:
			break;
		case DXGI_FORMAT_A8P8:
			break;
		case DXGI_FORMAT_B4G4R4A4_UNORM:
			return 4;
		case DXGI_FORMAT_FORCE_UINT:
			break;
		default:
			break;
		}
		return -1;
	}
	void InitStates( ID3D11Device * Device )
	{
	}
}

namespace COM
{
	MicrosoftPointer<IFileOpenDialog> g_openFileDialog;
	void InitializeComObjects()
	{
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<LPVOID*>(g_openFileDialog.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
			throw std::exception("Can't initialze COM");
	}
	void UninitializeComObjects()
	{
		g_openFileDialog.Reset();
	}
}