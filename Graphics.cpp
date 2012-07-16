#include "Graphics.h"

namespace ee
{
	Graphics::Graphics(HWND hWnd, Config *config, Logger *logger)
	{
		m_hWnd = hWnd;
		m_fullScreen = config->GetFullScreen();
		m_screenWidth = config->GetScreenWidth();
		m_screenHeight = config->GetScreenHeight();
		m_is4xMsaa = config->Get4xMsaa();
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
		UINT createDeviceFlags = 0;

		#if defined(DEBUG) || defined(_DEBUG)
			createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif

		if (m_fullScreen)
			m_logger->Log("Full screen enabled", ee::LEVEL_INFO);

		D3D_FEATURE_LEVEL featureLevel;

		//Create device and device context
		HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 
									   createDeviceFlags, NULL, 0, D3D11_SDK_VERSION, 
									   &m_device, &featureLevel, &m_deviceContext);

		if (FAILED(hr))
		{
			MessageBox(NULL, L"D3D11CreateDevice Failed.", NULL, 0);
			m_logger->Log("D3D11CreateDevice failed!", ee::LEVEL_SEVERE);
			return FALSE;
		}

		if (featureLevel != D3D_FEATURE_LEVEL_11_0)
		{
			MessageBox(NULL, L"Direct3D Feature Level 11 unsupported!", NULL, 0);
			m_logger->Log("Direct3D feature level 11 unsupported!", ee::LEVEL_SEVERE);
			return FALSE;
		}

		//Direct 11 should always allow at least 4xMSAA. We assert this here
		HR(m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_msaaQuality));
		assert(m_msaaQuality > 0);

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
		swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapDesc.Windowed = TRUE;
		swapDesc.Flags = NULL;

		if (m_is4xMsaa)
		{
			swapDesc.SampleDesc.Count = 4;
			swapDesc.SampleDesc.Quality = m_msaaQuality - 1;

			m_logger->Log("4xMSAA Enabled", ee::LEVEL_INFO);

		}
		else
		{
			swapDesc.SampleDesc.Count = 1;
			swapDesc.SampleDesc.Quality = 0;

			m_logger->Log("4xMSAA Disabled", ee::LEVEL_INFO);
		}

		//We need to create the swap chain now
		IDXGIDevice *dxgiDevice = NULL;
		HR(m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

		IDXGIAdapter *dxgiAdapter = NULL;
		HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

		//The factory interface
		IDXGIFactory *dxgiFactory = NULL;
		HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

		//Now create swap chain
		HR(dxgiFactory->CreateSwapChain(m_device, &swapDesc, &m_swapChain));

		dxgiDevice->Release();
		dxgiDevice = NULL;

		dxgiAdapter->Release();
		dxgiAdapter = NULL;

		dxgiFactory->Release();
		dxgiFactory = NULL;

		//Create the Render Target View
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

		//Create the Depth/Stencil Buffer and view
		D3D11_TEXTURE2D_DESC depthDesc;

		depthDesc.Width = m_screenWidth;
		depthDesc.Height = m_screenHeight;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //24 bits for Depth, 8 for stencil. In range [0, 1]
		//Msaa must be same as render target
		if (m_is4xMsaa)
		{
			depthDesc.SampleDesc.Count = 4;
			depthDesc.SampleDesc.Quality = m_msaaQuality - 1;
		}
		else
		{
			depthDesc.SampleDesc.Count = 1;
			depthDesc.SampleDesc.Quality = 0;
		}

		//GPU will be reading/writing to resource
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		//Where the resource will be bound to the pipeline
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthDesc.CPUAccessFlags = 0;
		depthDesc.MiscFlags = 0;

		//Return pointer to depth/stencil buffer
		HR(m_device->CreateTexture2D(&depthDesc, NULL, &m_depthStencilBuffer)); 
		//Resource we want to create a view to
		HR(m_device->CreateDepthStencilView(m_depthStencilBuffer, NULL, &m_depthStencilView)); 

		//Set the render targets
		m_deviceContext->OMSetRenderTargets(1, &m_backBuffer, m_depthStencilView);

		backBufferImage = NULL;

		//Set the viewport
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.Width = static_cast<float>(m_screenWidth);
		m_viewport.Height = static_cast<float>(m_screenHeight);
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		m_deviceContext->RSSetViewports(1, &m_viewport);

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

	void Graphics::Resize(int newWidth, int newHeight)
	{
		m_screenWidth = newWidth;
		m_screenHeight = newHeight;

		m_logger->Log(ee::IntegerToString(m_screenWidth), ee::LEVEL_INFO);
		m_logger->Log(ee::IntegerToString(m_screenHeight), ee::LEVEL_INFO);

		assert(m_device);
		assert(m_deviceContext);
		assert(m_swapChain);

		HR(m_swapChain->ResizeBuffers(1, m_screenWidth, m_screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
		ID3D11Texture2D *backBuffer;
		HR(m_swapChain->GetBuffer(1, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
		HR(m_device->CreateRenderTargetView(backBuffer, NULL, &m_backBuffer));
		
		backBuffer->Release();

		//Must rebuild depth and stencil buffer
		D3D11_TEXTURE2D_DESC depthDesc;

		depthDesc.Width = m_screenWidth;
		depthDesc.Height = m_screenHeight;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //24 bits for Depth, 8 for stencil. In range [0, 1]
		//Msaa must be same as render target
		if (m_is4xMsaa)
		{
			depthDesc.SampleDesc.Count = 4;
			depthDesc.SampleDesc.Quality = m_msaaQuality - 1;
		}
		else
		{
			depthDesc.SampleDesc.Count = 1;
			depthDesc.SampleDesc.Quality = 0;
		}

		//GPU will be reading/writing to resource
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		//Where the resource will be bound to the pipeline
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthDesc.CPUAccessFlags = 0;
		depthDesc.MiscFlags = 0;

		HR(m_device->CreateTexture2D(&depthDesc, NULL, &m_depthStencilBuffer));
		HR(m_device->CreateDepthStencilView(m_depthStencilBuffer, NULL, &m_depthStencilView));

		m_deviceContext->OMSetRenderTargets(1, &m_backBuffer, m_depthStencilView);

		//Transform the viewport
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.Width = static_cast<float>(m_screenWidth);
		m_viewport.Height = static_cast<float>(m_screenHeight);
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		m_deviceContext->RSSetViewports(1, &m_viewport);

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

		if (m_depthStencilBuffer)
		{
			m_depthStencilBuffer->Release();
			m_depthStencilBuffer = NULL;
		}

		if (m_depthStencilView)
		{
			m_depthStencilView->Release();
			m_depthStencilView = NULL;
		}
	}
}