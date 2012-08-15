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
#include <d3dx11effect.h>
#include <assert.h>
#include <vector>
#include <xnamath.h>
#include <fstream>
#include <sstream>

#include "Logger.h"
#include "Config.h"
#include "EagleEngineUtil.h"
#include "Colour.h"
#include "MathsUtil.h"
#include "GeometryBuilder.h"
#include "InputHandler.h"
#include "ModelLoader.h"
#include "Light.h"
#include "Material.h"
#include "EffectsUtil.h"
#include "Vertex.h"

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
		//Check whether device exists
		bool IsDevice() const;
		float GetAspectRatio() const;
		//Toggle wireframe
		void ToggleWireframe();
		//Mouse is down
		void MouseDown(MouseButton button, int x, int y);
		//Mouse is up
		void MouseUp(MouseButton button, int x, int y);
		//Mouse moved
		void MouseMoved(WPARAM button, int x, int y);
		//Toggle flashlight
		void ToggleFlashlight();
		//Call after no longer needed, e.g at window close time
		void Release();

	private:
		//Creates the vertex and index buffers
		void CreateBuffers();
		//Build world matrices for objects		
		void BuildMatrices();
		//Inits the light in the scene
		void InitLight();
		//Loads all textures
		void LoadTextures();

		//Logger object used for logging info to text file
		Logger *m_logger;
		//Handle to main output window
		HWND m_hWnd;
		int m_screenWidth, m_screenHeight;
		//Whether to run in full screen or not
		BOOL m_fullScreen;
		//Whether 4x msaa is enabled
		BOOL m_is4xMsaa;
		//Whether wireframe mode is enabled
		BOOL m_isWireframe;
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
		XMFLOAT4X4 m_view;
		XMFLOAT4X4 m_proj;
		//Eye pos
		XMFLOAT3 m_eyePosW;
		//World matrices for objects
		XMFLOAT4X4 m_boxWorld;
		XMFLOAT4X4 m_wallLeftWorld;
		XMFLOAT4X4 m_wallRightWorld;
		XMFLOAT4X4 m_wallBackWorld;
		XMFLOAT4X4 m_wallFloorWorld;
		XMFLOAT4X4 m_wallRoofWorld;
		XMFLOAT4X4 m_crateWorld;
		//Vertex and index buffers
		ID3D11Buffer *m_vertexBuffer;
		ID3D11Buffer *m_indexBuffer;
		//Lights
		DirectionalLight m_dirLights[1];
		SpotLight		 m_staticSpotLight;
		SpotLight		 m_spotLight;
		PointLight		 m_pointLight;
		Material m_boxMat;
		//Whether the spot light is on
		bool m_isFlashlight;
		//The input layout
		ID3D11InputLayout *m_inputLayout;
		//Offsets for vertex and index buffers
		int m_boxVertexOffset;
		int m_wallLeftVertexOffset;
		int m_wallRightVertexOffset;
		int m_wallBackVertexOffset;
		int m_wallFloorVertexOffset;
		int m_wallRoofVertexOffset;
		int m_crateVertexOffset;

		UINT m_boxIndexCount;
		UINT m_wallLeftIndexCount;
		UINT m_wallRightIndexCount;
		UINT m_wallBackIndexCount;
		UINT m_wallFloorIndexCount;
		UINT m_wallRoofIndexCount;
		UINT m_crateIndexCount;

		UINT m_boxIndexOffset;
		UINT m_wallLeftIndexOffset;
		UINT m_wallRightIndexOffset;
		UINT m_wallBackIndexOffset;
		UINT m_wallFloorIndexOffset;
		UINT m_wallRoofIndexOffset;
		UINT m_crateIndexOffset;
		//Wireframe raster state
		ID3D11RasterizerState *m_wireframeRS;
		//Rotation angles
		float m_theta, m_phi, m_radius;
		//last mouse pos
		POINT m_lastMousePos;
		//Diffuse map for textures
		ID3D11ShaderResourceView* m_diffuseMap;
		ID3D11ShaderResourceView *m_crateDiffuseMap;
		//Tex transform
		XMFLOAT4X4 m_texTransform;
		XMFLOAT4X4 m_crateTexTransform;
	};
}

#endif