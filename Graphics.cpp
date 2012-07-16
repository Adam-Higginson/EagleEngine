#include "Graphics.h"

namespace ee
{
	Graphics::Graphics(HWND hWnd, BOOL fullScreen, int screenWidth, int screenHeight, Logger *logger)
	{
		m_hWnd = hWnd;
		m_fullScreen = fullScreen;
		m_screenWidth = screenWidth;
		m_screenHeight = screenHeight;
		m_logger = logger;
	}

	Graphics::Graphics(const Graphics &other)
	{
	}

	Graphics::~Graphics()
	{
	}

	BOOL Graphics::Init()
	{
		if (m_fullScreen)
			m_logger->Log("Full screen enabled", ee::LEVEL_INFO);
		DXGI_SWAP_CHAIN_DESC swapDesc;
		ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		//1 back buffer
		swapDesc.BufferCount = 1;
		swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.OutputWindow = m_hWnd;
		swapDesc.SampleDesc.Count = 4;
		swapDesc.BufferDesc.Width = m_screenWidth;
		swapDesc.BufferDesc.Height = m_screenHeight;
		swapDesc.Windowed = TRUE;
		swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, 
												 NULL, NULL, D3D11_SDK_VERSION, &swapDesc, 
												 &m_swapChain, &m_device, NULL, &m_deviceContext)))
		{
			if (m_logger)
				m_logger->Log("Could not create device and swap chain!", ee::LEVEL_SEVERE);
			return FALSE;
		}

		ID3D11Texture2D *backBufferImage;

		if (FAILED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferImage)))
		{
			if (m_logger)
				m_logger->Log("Could not get buffer!", ee::LEVEL_SEVERE);
			return FALSE;
		}

		if (FAILED(m_device->CreateRenderTargetView(backBufferImage, NULL, &m_backBuffer)))
		{
			if (m_logger)
				m_logger->Log("Could not create render target view!", ee::LEVEL_SEVERE);
			return FALSE;
		}

		backBufferImage->Release();

		m_deviceContext->OMSetRenderTargets(1, &m_backBuffer, NULL);

		backBufferImage = NULL;

		//Set the viewport
		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = m_screenWidth;
		viewport.Height = m_screenHeight;

		m_deviceContext->RSSetViewports(1, &viewport);

		//Set up view matrix
		D3DXVECTOR3 eye(0.0f, 0.0f, -5.0f);
		D3DXVECTOR3 view(0.0f, 0.0f, 1.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

		//D3DXMatrixLookAtLH(m_viewMatrix, &eye, &view, &up);

		//Set up projection matrix
		//D3DXMatrixPerspectiveFovLH(m_projectionMatrix, (FLOAT)D3DX_PI * 0.5, (FLOAT)m_screenWidth / (FLOAT)m_screenHeight, 0.1f, 100.0f);

		//if (m_logger)
		//	m_logger->Log("Graphics context created", ee::LEVEL_INFO);

		return TRUE;

	}

	void Graphics::UpdateScene(float elapsedTime)
	{
		//TODO update scene
	}

	void Graphics::DrawScene(float r, float g, float b)
	{
		m_deviceContext->ClearRenderTargetView(m_backBuffer, D3DXCOLOR(r, g, b, 1.0f));

		m_swapChain->Present(0, 0);
	}

	void Graphics::Release()
	{
		if (m_swapChain)
		{
			m_swapChain->SetFullscreenState(FALSE, NULL);
			m_swapChain->Release();
			m_swapChain = NULL;
		}

		if (m_device)
		{
			m_device->Release();
			m_device = NULL;
		}

		if (m_deviceContext)
		{
			m_deviceContext->Release();
			m_deviceContext = NULL;
		}

		if (m_backBuffer)
		{
			m_backBuffer->Release();
			m_backBuffer = NULL;
		}
	}
}