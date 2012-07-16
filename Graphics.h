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
#include <assert.h>

#include "Logger.h"
#include "Config.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
		#define HR(x)
		{
			HRESULT hr = (x);
			if (FAILED(hr))
			{
				DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true);
			}
		}
	#endif
#else
	#ifndef HR
		#define HR(x) (x)
	#endif
#endif


namespace ee
{
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
		void Release();

	private:
		Logger *m_logger;
		HWND m_hWnd;
		int m_screenWidth, m_screenHeight;
		//Whether to run in full screen or not
		BOOL m_fullScreen;
		//Whether 4x msaa is enabled
		BOOL m_is4xMsaa;
		//The swap chain
		IDXGISwapChain *m_swapChain;
		ID3D11Device *m_device;
		ID3D11DeviceContext *m_deviceContext;
		ID3D11RenderTargetView *m_backBuffer;
		D3DXMATRIX *m_viewMatrix;
		D3DXMATRIX *m_projectionMatrix;
		
	};
}

#endif