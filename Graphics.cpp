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

		//Load world, view and projection matrices
		XMMATRIX identityMatrix = XMMatrixIdentity();
		XMStoreFloat4x4(&m_world, identityMatrix);
		XMStoreFloat4x4(&m_view, identityMatrix);
		XMStoreFloat4x4(&m_proj, identityMatrix);
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

		//Stop ALT-ENTER functionality
		HR(dxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_WINDOW_CHANGES));

		//Log how many adapters are found
		Logger graphicsLog("graphics_info.log", ee::LEVEL_INFO);
		graphicsLog.Init();
		graphicsLog.ShouldWriteLevel(false);

		UINT i = 0;
		IDXGIAdapter *currentAdapter = NULL;
		std::vector<IDXGIAdapter *> adapters;
		while(dxgiFactory->EnumAdapters(i, &currentAdapter) != DXGI_ERROR_NOT_FOUND)
		{
			adapters.push_back(currentAdapter);
			i++;
		}

		graphicsLog.Log(("Adapters found: " + IntegerToString((int)i)), ee::LEVEL_INFO);

		i = 0;
		for (std::vector<IDXGIAdapter *>::iterator iter = adapters.begin(); iter != adapters.end(); iter++)
		{
			currentAdapter = *iter;
			if (currentAdapter->CheckInterfaceSupport(__uuidof(ID3D10Device), NULL) == S_OK)
				graphicsLog.Log("D3D10 Supported for device: " + IntegerToString((int) i), LEVEL_INFO);
			i++;
		}

		IDXGIAdapter *defaultAdapter = adapters.front();
		std::vector<IDXGIOutput *> outputs;
		i = 0;
		IDXGIOutput *currentOutput;
		while (defaultAdapter->EnumOutputs(i, &currentOutput) != DXGI_ERROR_NOT_FOUND)
		{
			outputs.push_back(currentOutput);
			i++;
		}

		graphicsLog.Log(("Outputs found for default adapter: " + IntegerToString(i)), LEVEL_INFO);



		//Write info about each display mode
		int count = 0;
		for (std::vector<IDXGIOutput *>::iterator it = outputs.begin(); it != outputs.end(); it++)
		{
			IDXGIOutput *currentOutput = *it;
			UINT num = 0;
			currentOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num, NULL);

			DXGI_MODE_DESC *descs = new DXGI_MODE_DESC[num];
			currentOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num, descs);
			graphicsLog.Log("For interface:" + IntegerToString(count), LEVEL_INFO);
			for (int j = 0; j < (int)num; j++)
			{
				graphicsLog.Log("	Width: " + IntegerToString((int)descs[j].Width) 
								+ " Height: " + IntegerToString((int)descs[j].Height) 
								+ " Refresh: " + IntegerToString((int)descs[j].RefreshRate.Numerator) 
								+ "/" 
								+ IntegerToString((int)descs[j].RefreshRate.Denominator), LEVEL_INFO);
			}

			count++;
		}

		//Release everything!
		for (std::vector<IDXGIAdapter *>::iterator iter = adapters.begin(); iter != adapters.end(); iter++)
		{
			(*iter)->Release();
			(*iter) = NULL;
		}

		for (std::vector<IDXGIOutput *>::iterator iter = outputs.begin(); iter != outputs.end(); iter++)
		{
			(*iter)->Release();
			(*iter) = NULL;
		}

		graphicsLog.CloseLogger();

		if (dxgiDevice)
		{
			dxgiDevice->Release();
			dxgiDevice = NULL;
		}

		if (dxgiAdapter)
		{
			dxgiAdapter->Release();
			dxgiAdapter = NULL;
		}

		if (dxgiFactory)
		{
			dxgiFactory->Release();
			dxgiFactory = NULL;
		}

		ID3D11Texture2D *backBuffer;
		HR(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
		HR(m_device->CreateRenderTargetView(backBuffer, NULL, &m_backBuffer));
		
		backBuffer->Release();

		//Build depth and stencil buffer
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

		//Quick test viewport
		m_viewport.TopLeftX = 100.0f;
		m_viewport.TopLeftY  = 100.0f;
		m_viewport.Width = 500.0f;
		m_viewport.Height = 400.0f;
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

		m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		HR(m_swapChain->Present(0, 0));
	}

	void Graphics::Resize(int newWidth, int newHeight)
	{
		m_screenWidth = newWidth;
		m_screenHeight = newHeight;

		assert(m_device);
		assert(m_deviceContext);
		assert(m_swapChain);

		m_backBuffer->Release();
		m_depthStencilView->Release();
		m_depthStencilBuffer->Release();

		HR(m_swapChain->ResizeBuffers(1, m_screenWidth, m_screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
		ID3D11Texture2D *backBuffer;
		HR(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
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

	float Graphics::GetAspectRatio() const
	{
		return static_cast<float>(m_screenWidth) / m_screenHeight;
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
			m_deviceContext->ClearState();
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

	////////////////////////
	//PRIVATE FUNCTIONS
	///////////////////////
	void Graphics::CreateBuffers()
	{
		//Create vertex buffer
		//Array represents a cube
		Vertex vertices[] =
		{
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), (const float*)&Colour::BLUE },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), (const float*)&Colour::BLUE },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), (const float*)&Colour::BLUE },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), (const float*)&Colour::BLUE },
			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), (const float*)&Colour::BLUE },
			{ XMFLOAT3(-1.0f, 1.0f, -.0f), (const float*)&Colour::BLUE },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), (const float*)&Colour::BLUE },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), (const float*)&Colour::BLUE },
		};

		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * 8;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vSubData;
		vSubData.pSysMem = vertices;
		HR(m_device->CreateBuffer(&vertexBufferDesc, &vSubData, &m_cubeVB));

		//Create indes buffer
		UINT indices[] = {
			//front face
			0, 1, 2,
			0, 2, 3,
			//back face
			4, 6, 5,
			4, 7, 6,
			//left face
			4, 5, 1,
			4, 1, 0,
			//right face
			3, 2, 6,
			3, 6, 7,
			//top face
			1, 5, 6,
			1, 6, 2,
			//bottom face
			4, 0, 3,
			4, 3, 7
		};

		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.ByteWidth = sizeof(UINT) * 36;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA iSubData;
		iSubData.pSysMem = indices;
		HR(m_device->CreateBuffer(&indexBufferDesc, &iSubData, &m_cubeIB));
	}

	bool Graphics::BuildShaders()
	{
		DWORD shaderFlags = 0;

	#if defined(DEBUG) || defined(_DEBUG)
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
	#endif

		ID3D10Blob *compiledShader = NULL;
		ID3D10Blob *errorMessages = NULL;

		HRESULT hr = D3DX11CompileFromFile(L"BasicShader.fx", NULL, NULL, NULL, "fx_5_0", shaderFlags, 0, 0, &compiledShader, &errorMessages, NULL);

		if (errorMessages != 0)
		{
			MessageBoxA(NULL, (char *)errorMessages->GetBufferPointer(), NULL, 0);
			errorMessages->Release();
			errorMessages = NULL;
		}

		if (FAILED(hr))
		{
			HR(hr);
			return false;
		}

		return true;

	}
}