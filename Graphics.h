//Filename: Graphics.h
//A class which interfaces with Direct3D to allow the
//drawing of hraphics onto a window

#pragma once

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <windows.h>
#include <WindowsX.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3DX10.h>
#include <DxErr.h>
#include <assert.h>
#include <vector>
#include <xnamath.h>

#include "Logger.h"
#include "Config.h"
#include "EagleEngineUtil.h"
#include "Colour.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#if defined(DEBUG) | defined(_DEBUG)
#pragma comment(lib, "d3dx11d.lib")
#pragma comment(lib, "d3dx10d.lib")

#else
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#endif

#pragma comment(lib, "dxerr.lib")

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR 
	#define HR(x)														\
	{																	\
			HRESULT hr = (x);											\
			if (FAILED(hr))												\
			{															\
				DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true);		\
			}															\
	}
	#endif
#else
	#ifndef HR
		#define HR(x) (x)
	#endif
#endif


namespace ee
{
	//Basic Vertex struct
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT4 colour;
	};

	class Graphics
	{
	public:
		Graphics(HWND, Config*, Logger*);
		Graphics(const Graphics&);
		~Graphics();

		BOOL Init();
		//Update the scene
		void UpdateScene(float elapsedTime);
		//Draw the scene
		void DrawScene(float r, float g, float b);
		//Resize the scene
		void Resize(int newWidth, int newHeight);

		float GetAspectRatio() const;

		void Release();

	private:
		//Creates the vertex and index buffers
		void CreateBuffers();
		//Builds and compilles shaders
		bool BuildShaders();

		//Logger object used for logging info to text file
		Logger *m_logger;
		//Handle to main output window
		HWND m_hWnd;
		int m_screenWidth, m_screenHeight;
		//Whether to run in full screen or not
		BOOL m_fullScreen;
		//Whether 4x msaa is enabled
		BOOL m_is4xMsaa;
		//The swap chain
		IDXGISwapChain *m_swapChain;
		//Device
		ID3D11Device *m_device;
		//Device context
		ID3D11DeviceContext *m_deviceContext;
		//Render target view of the back buffer
		ID3D11RenderTargetView *m_backBuffer;
		//The depth stencil buffer
		ID3D11Texture2D *m_depthStencilBuffer;
		//The depth stencil view
		ID3D11DepthStencilView *m_depthStencilView;
		//The msaa quality
		UINT m_msaaQuality;
		//The viewport
		D3D11_VIEWPORT m_viewport;

		//World, view and projection matrices
		XMFLOAT4X4 m_world;
		XMFLOAT4X4 m_view;
		XMFLOAT4X4 m_proj;

		//Box buffer to hold data about cube
		ID3D11Buffer *m_cubeVB;
		ID3D11Buffer *m_cubeIB;
	};
}

#endif