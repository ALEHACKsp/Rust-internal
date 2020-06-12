#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3dcompiler.h>

#pragma warning(disable:4005) //dwrite.h conflicts with winerror.h
#pragma warning(disable:4458)

#include "FW1FontWrapper.h"

#pragma comment(lib, "FW1FontWrapper.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "gui.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

typedef HRESULT(__fastcall* DX11PresentFn) (IDXGISwapChain* pSwapChain, UINT syncintreval, UINT flags);
typedef HRESULT(__stdcall* DX11ResizeFn)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

class renderer_t
{
private:
public:
	std::unique_ptr<r_helper> helper;
public:
	void init();
};

inline renderer_t i_renderer;
