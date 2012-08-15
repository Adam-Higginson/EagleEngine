#include "Graphics.h"

namespace ee
{
	Graphics::Graphics(HWND hWnd, Config *config, Logger *logger)
		: m_theta(1.5f * MathsUtil::PI), m_phi(0.1f * MathsUtil::PI), m_radius(3.0f), m_eyePosW(0.0f, 0.0f, 0.0f), m_isFlashlight(true)
	{
		m_hWnd = hWnd;
		m_fullScreen = config->GetFullScreen();
		m_screenWidth = config->GetScreenWidth();
		m_screenHeight = config->GetScreenHeight();
		m_is4xMsaa = config->Get4xMsaa();
		m_logger = logger;
		m_isWireframe = FALSE;
		m_lastMousePos.x = m_lastMousePos.y = 0;

		ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
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
		//HR(dxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_WINDOW_CHANGES));

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

			delete [] descs;
			descs = NULL;
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
		/*m_viewport.TopLeftX = 100.0f;
		m_viewport.TopLeftY  = 100.0f;
		m_viewport.Width = 500.0f;
		m_viewport.Height = 400.0f;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;*/

		m_deviceContext->RSSetViewports(1, &m_viewport);

		//Build wireframe raster state
		D3D11_RASTERIZER_DESC wireframeDesc;
		ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
		wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
		wireframeDesc.CullMode = D3D11_CULL_BACK;
		wireframeDesc.FrontCounterClockwise = false;
		wireframeDesc.DepthClipEnable = true;

		HR(m_device->CreateRasterizerState(&wireframeDesc, &m_wireframeRS));

		LoadTextures();
		InitLight();
		Effects::Init(m_device);
		InputLayouts::Init(m_device);
		CreateBuffers();
		BuildMatrices();
		


		return TRUE;

	}

	void Graphics::UpdateScene(float elapsedTime)
	{
		float x = m_radius * sinf(m_phi) * cosf(m_theta);
		float y = m_radius * cosf(m_phi);
		float z = m_radius * sinf(m_phi) * sinf(m_theta);

		m_eyePosW = XMFLOAT3(x, y, z);

		//Build view matrix
		XMVECTOR position = XMVectorSet(x, y, z, 1.0f);
		XMVECTOR target = XMVectorZero();
		XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMMATRIX view = XMMatrixLookAtLH(position, target, up);
		XMStoreFloat4x4(&m_view, view);

		m_spotLight.position = m_eyePosW;
		XMStoreFloat3(&m_spotLight.direction, XMVector3Normalize(target - position));
	}

	void Graphics::DrawScene(float r, float g, float b)
	{
		m_deviceContext->ClearRenderTargetView(m_backBuffer, D3DXCOLOR(r, g, b, 1.0f));
		m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		m_deviceContext->IASetInputLayout(InputLayouts::posNormalTexLayout);
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (m_isWireframe)
			m_deviceContext->RSSetState(m_wireframeRS);
		else
			m_deviceContext->RSSetState(NULL);

		UINT stride = sizeof(Vertex::PosNormalTex);
		UINT offset = 0;

		XMMATRIX view = XMLoadFloat4x4(&m_view);
		XMMATRIX proj = XMLoadFloat4x4(&m_proj);
		XMMATRIX viewProj = view * proj;

		//per frame constants
		Effects::basicFX->SetDirLights(m_dirLights);
		Effects::basicFX->SetSpotLight(m_spotLight);
		Effects::basicFX->SetStaticSpotLight(m_staticSpotLight);
		Effects::basicFX->SetPointLight(m_pointLight);
		Effects::basicFX->SetEyePosW(m_eyePosW);

		ID3DX11EffectTechnique *activeTech;
		
		if (m_isFlashlight)
			activeTech = Effects::basicFX->light1SpotTexTech;
		else
			activeTech = Effects::basicFX->light1TexTech;

		D3DX11_TECHNIQUE_DESC techDesc;
		activeTech->GetDesc(&techDesc);

		for (UINT pass = 0; pass < techDesc.Passes; ++pass)
		{
			m_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
			m_deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			//Draw box
			XMMATRIX world = XMLoadFloat4x4(&m_boxWorld);
			XMMATRIX worldInvTranspose = MathsUtil::InverseTranspose(world);
			XMMATRIX worldViewProj = world * view * proj;

			Effects::basicFX->SetWorld(world);
			Effects::basicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::basicFX->SetWorldViewProj(worldViewProj);
			Effects::basicFX->SetTexTransform(XMLoadFloat4x4(&m_texTransform));
			Effects::basicFX->SetMaterial(m_boxMat);
			Effects::basicFX->SetDiffuseMap(m_diffuseMap);
			activeTech->GetPassByIndex(pass)->Apply(0, m_deviceContext);
			m_deviceContext->DrawIndexed(m_boxIndexCount, m_boxIndexOffset, m_boxVertexOffset);

			//Draw left wall
			world = XMLoadFloat4x4(&m_wallLeftWorld);
			worldViewProj = world * view * proj;
			Effects::basicFX->SetWorld(world);
			Effects::basicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::basicFX->SetWorldViewProj(worldViewProj);
			Effects::basicFX->SetTexTransform(XMLoadFloat4x4(&m_texTransform));
			Effects::basicFX->SetMaterial(m_boxMat);
			Effects::basicFX->SetDiffuseMap(m_diffuseMap);
			activeTech->GetPassByIndex(pass)->Apply(0, m_deviceContext);
			m_deviceContext->DrawIndexed(m_wallLeftIndexCount, m_wallLeftIndexOffset, m_wallLeftVertexOffset);

			//Draw right wall
			world = XMLoadFloat4x4(&m_wallRightWorld);
			worldViewProj = world * view * proj;
			Effects::basicFX->SetWorld(world);
			Effects::basicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::basicFX->SetWorldViewProj(worldViewProj);
			Effects::basicFX->SetTexTransform(XMLoadFloat4x4(&m_texTransform));
			Effects::basicFX->SetMaterial(m_boxMat);
			Effects::basicFX->SetDiffuseMap(m_diffuseMap);
			activeTech->GetPassByIndex(pass)->Apply(0, m_deviceContext);
			m_deviceContext->DrawIndexed(m_wallRightIndexCount, m_wallRightIndexOffset, m_wallRightVertexOffset);

			//Draw back wall
			world = XMLoadFloat4x4(&m_wallBackWorld);
			worldViewProj = world * view * proj;
			Effects::basicFX->SetWorld(world);
			Effects::basicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::basicFX->SetWorldViewProj(worldViewProj);
			Effects::basicFX->SetTexTransform(XMLoadFloat4x4(&m_texTransform));
			Effects::basicFX->SetMaterial(m_boxMat);
			Effects::basicFX->SetDiffuseMap(m_diffuseMap);
			activeTech->GetPassByIndex(pass)->Apply(0, m_deviceContext);
			m_deviceContext->DrawIndexed(m_wallBackIndexCount, m_wallBackIndexOffset, m_wallBackVertexOffset);

			//Draw floor wall
			world = XMLoadFloat4x4(&m_wallFloorWorld);
			worldViewProj = world * view * proj;
			Effects::basicFX->SetWorld(world);
			Effects::basicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::basicFX->SetWorldViewProj(worldViewProj);
			Effects::basicFX->SetTexTransform(XMLoadFloat4x4(&m_texTransform));
			Effects::basicFX->SetMaterial(m_boxMat);
			Effects::basicFX->SetDiffuseMap(m_diffuseMap);
			activeTech->GetPassByIndex(pass)->Apply(0, m_deviceContext);
			m_deviceContext->DrawIndexed(m_wallFloorIndexCount, m_wallFloorIndexOffset, m_wallFloorVertexOffset);

			//Draw Roof wall
			world = XMLoadFloat4x4(&m_wallRoofWorld);
			worldViewProj = world * view * proj;
			Effects::basicFX->SetWorld(world);
			Effects::basicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::basicFX->SetWorldViewProj(worldViewProj);
			Effects::basicFX->SetTexTransform(XMLoadFloat4x4(&m_texTransform));
			Effects::basicFX->SetMaterial(m_boxMat);
			Effects::basicFX->SetDiffuseMap(m_diffuseMap);
			activeTech->GetPassByIndex(pass)->Apply(0, m_deviceContext);
			m_deviceContext->DrawIndexed(m_wallRoofIndexCount, m_wallRoofIndexOffset, m_wallRoofVertexOffset);

			//Draw Crate
			world = XMLoadFloat4x4(&m_crateWorld);
			worldViewProj = world * view * proj;
			Effects::basicFX->SetWorld(world);
			Effects::basicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::basicFX->SetWorldViewProj(worldViewProj);
			Effects::basicFX->SetTexTransform(XMLoadFloat4x4(&m_crateTexTransform));
			Effects::basicFX->SetMaterial(m_boxMat);
			Effects::basicFX->SetDiffuseMap(m_crateDiffuseMap);
			activeTech->GetPassByIndex(pass)->Apply(0, m_deviceContext);
			m_deviceContext->DrawIndexed(m_crateIndexCount, m_crateIndexOffset, m_crateVertexOffset);

		}

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

		//Update aspect ratio and recompute projection matrix
		//45 degree FOV, near at 1.0f, far at 1000.0f
		XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathsUtil::PI, GetAspectRatio(), 1.0f, 1000.0f);
		XMStoreFloat4x4(&m_proj, P);
		
	}

	float Graphics::GetAspectRatio() const
	{
		return static_cast<float>(m_screenWidth) / m_screenHeight;
	}

	void Graphics::ToggleFlashlight()
	{
		m_isFlashlight = !m_isFlashlight;
	}

	void Graphics::Release()
	{

		if (m_vertexBuffer)
		{
			m_vertexBuffer->Release();
			m_vertexBuffer = NULL;
		}

		if (m_indexBuffer)
		{
			m_indexBuffer->Release();
			m_indexBuffer = NULL;
		}


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

		if (m_wireframeRS)
		{
			m_wireframeRS->Release();
			m_wireframeRS = NULL;
		}

		if (m_diffuseMap)
		{
			m_diffuseMap->Release();
			m_diffuseMap = NULL;
		}

		if (m_crateDiffuseMap)
		{
			m_crateDiffuseMap->Release();
			m_crateDiffuseMap = NULL;
		}

		Effects::Release();
		InputLayouts::Release();
	}

	bool Graphics::IsDevice() const
	{
		return m_device != NULL;
	}

	void Graphics::ToggleWireframe()
	{
		m_isWireframe = !m_isWireframe;
	}

	void Graphics::MouseDown(MouseButton button, int x, int y)
	{
		m_lastMousePos.x = x;
		m_lastMousePos.y = y;

		SetCapture(m_hWnd);
		ShowCursor(FALSE);
	}

	void Graphics::MouseUp(MouseButton button, int x, int y)
	{
		ReleaseCapture();
		ShowCursor(TRUE);
	}

	void Graphics::MouseMoved(WPARAM button, int x, int y)
	{
		if ((button & MK_LBUTTON) != 0)
		{
			float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_lastMousePos.x));
			float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_lastMousePos.y));

			m_theta += dx;
			m_phi += dy;
	
			m_phi = MathsUtil::Clamp(m_phi, 0.1f, MathsUtil::PI - 0.1f);
		}

		else if((button & MK_RBUTTON) != 0 )
		{
			// Make each pixel correspond to 0.01 unit in the scene.
			float dx = 0.01f*static_cast<float>(x - m_lastMousePos.x);
			float dy = 0.01f*static_cast<float>(y - m_lastMousePos.y);

			// Update the camera radius based on input.
			m_radius += dx - dy;

			// Restrict the radius.
			m_radius = MathsUtil::Clamp(m_radius, 1.5f, 20.0f);
		}

		m_lastMousePos.x = x;
		m_lastMousePos.y = y;
	}

	////////////////////////
	//PRIVATE FUNCTIONS
	///////////////////////
	void Graphics::CreateBuffers()
	{
		//Create the cube in main memory
		GeometryBuilder builder;
		GeometryBuilder::MeshData cubeData, wallLeft, wallRight, wallBack, wallFloor, wallRoof, crate;
		builder.CreateBox(20.0f, 10.0f, 1.0f, cubeData);
		builder.CreateBox(1.0f, 10.0f, 15.0f, wallLeft);
		builder.CreateBox(1.0f, 10.0f, 15.0f, wallRight);
		builder.CreateBox(20.0f, 10.0f, 1.0f, wallBack);
		builder.CreateBox(20.0f, 1.0f, 15.0f, wallFloor);
		builder.CreateBox(20.0f, 1.0f, 15.0f, wallRoof);
		builder.CreateBox(1.0f, 1.0f, 1.0f, crate);

		//Cache vertex offset for each object
		m_boxVertexOffset = 0;
		m_wallLeftVertexOffset = cubeData.vertices.size();
		m_wallRightVertexOffset = m_wallLeftVertexOffset + wallLeft.vertices.size();
		m_wallBackVertexOffset = m_wallRightVertexOffset + wallRight.vertices.size();
		m_wallFloorVertexOffset = m_wallBackVertexOffset + wallBack.vertices.size();
		m_wallRoofVertexOffset = m_wallFloorVertexOffset + wallFloor.vertices.size();
		m_crateVertexOffset = m_wallRoofVertexOffset + wallRoof.vertices.size();

		//Cache index count of each object
		m_boxIndexCount = cubeData.indices.size();
		m_wallLeftIndexCount = wallLeft.indices.size();
		m_wallRightIndexCount = wallRight.indices.size();
		m_wallBackIndexCount = wallBack.indices.size();
		m_wallFloorIndexCount = wallFloor.indices.size();
		m_wallRoofIndexCount = wallRoof.indices.size();
		m_crateIndexCount = crate.indices.size();

		//Cache the starting index for each object in index buffer
		m_boxIndexOffset = 0;
		m_wallLeftIndexOffset = m_boxIndexCount;
		m_wallRightIndexOffset = m_wallLeftIndexOffset + m_wallLeftIndexCount;
		m_wallBackIndexOffset = m_wallRightIndexOffset + m_wallRightIndexCount;
		m_wallFloorIndexOffset = m_wallBackIndexOffset + m_wallBackIndexCount;
		m_wallRoofIndexOffset = m_wallFloorIndexOffset + m_wallFloorIndexCount;
		m_crateIndexOffset = m_wallRoofIndexOffset + m_wallRoofIndexCount;


		UINT totalVertices = cubeData.vertices.size() 
							+ wallLeft.vertices.size() 
							+ wallRight.vertices.size() 
							+ wallBack.vertices.size() 
							+ wallFloor.vertices.size()
							+ wallRoof.vertices.size()
							+ crate.vertices.size();

		UINT totalIndices = m_boxIndexCount 
						  + m_wallLeftIndexCount
						  + m_wallRightIndexCount
						  + m_wallBackIndexCount
						  + m_wallFloorIndexCount
						  + m_wallRoofIndexCount
						  + m_crateIndexCount;

		//A vector of all the vertices into one vertex buffer
		std::vector<Vertex::PosNormalTex> vertices(totalVertices);

		UINT k = 0;
		for (size_t i = 0; i < cubeData.vertices.size(); i++, k++)
		{
			vertices[k].pos		= cubeData.vertices[i].position;
			vertices[k].normal	= cubeData.vertices[i].normal;
			vertices[k].tex		= cubeData.vertices[i].texC;
		}

		for (size_t i = 0; i < wallLeft.vertices.size(); i++, k++)
		{
			vertices[k].pos		= wallLeft.vertices[i].position;
			vertices[k].normal	= wallLeft.vertices[i].normal;
			vertices[k].tex		= wallLeft.vertices[i].texC;
		}

		for (size_t i = 0; i < wallRight.vertices.size(); i++, k++)
		{
			vertices[k].pos		= wallRight.vertices[i].position;
			vertices[k].normal	= wallRight.vertices[i].normal;
			vertices[k].tex		= wallRight.vertices[i].texC;
		}

		for (size_t i = 0; i < wallBack.vertices.size(); i++, k++)
		{
			vertices[k].pos		= wallBack.vertices[i].position;
			vertices[k].normal	= wallBack.vertices[i].normal;
			vertices[k].tex		= wallBack.vertices[i].texC;
		}

		for (size_t i = 0; i < wallFloor.vertices.size(); i++, k++)
		{
			vertices[k].pos		= wallFloor.vertices[i].position;
			vertices[k].normal	= wallFloor.vertices[i].normal;
			vertices[k].tex		= wallFloor.vertices[i].texC;
		}

		for (size_t i = 0; i < wallRoof.vertices.size(); i++, k++)
		{
			vertices[k].pos		= wallRoof.vertices[i].position;
			vertices[k].normal	= wallRoof.vertices[i].normal;
			vertices[k].tex		= wallRoof.vertices[i].texC;
		}

		for (size_t i = 0; i < crate.vertices.size(); i++, k++)
		{
			vertices[k].pos		= crate.vertices[i].position;
			vertices[k].normal	= crate.vertices[i].normal;
			vertices[k].tex		= crate.vertices[i].texC;
		}

		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.ByteWidth = sizeof(Vertex::PosNormalTex) * totalVertices;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vSubData;
		vSubData.pSysMem = &vertices[0];
		HR(m_device->CreateBuffer(&vertexBufferDesc, &vSubData, &m_vertexBuffer));

		//Pack the indices of all meshes into one index buffer
		std::vector<UINT> indices;
		indices.insert(indices.end(), cubeData.indices.begin(), cubeData.indices.end());
		indices.insert(indices.end(), wallLeft.indices.begin(), wallLeft.indices.end());
		indices.insert(indices.end(), wallRight.indices.begin(), wallRight.indices.end());
		indices.insert(indices.end(), wallBack.indices.begin(), wallBack.indices.end());
		indices.insert(indices.end(), wallFloor.indices.begin(), wallFloor.indices.end());
		indices.insert(indices.end(), wallRoof.indices.begin(), wallRoof.indices.end());
		indices.insert(indices.end(), crate.indices.begin(), crate.indices.end());

		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.ByteWidth = sizeof(UINT) * totalIndices;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA iSubData;
		iSubData.pSysMem = &indices[0];
		HR(m_device->CreateBuffer(&indexBufferDesc, &iSubData, &m_indexBuffer));
	}

	void Graphics::BuildMatrices()
	{
		//Load world, view and projection matrices
		XMMATRIX identityMatrix = XMMatrixIdentity();
		XMStoreFloat4x4(&m_view, identityMatrix);
		XMStoreFloat4x4(&m_proj, identityMatrix);

		XMMATRIX texScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
		XMStoreFloat4x4(&m_texTransform, texScale);

		XMStoreFloat4x4(&m_crateTexTransform, identityMatrix);

		//XMMATRIX boxScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
		XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 4.0f, 8.f);
		XMStoreFloat4x4(&m_boxWorld, boxOffset);

		XMMATRIX wallLeftOffset = XMMatrixTranslation(10.0f, 4.0f, 0.0f);
		XMStoreFloat4x4(&m_wallLeftWorld, wallLeftOffset);

		XMMATRIX wallRightOffset = XMMatrixTranslation(-10.0f, 4.0f, 0.0f);
		XMStoreFloat4x4(&m_wallRightWorld, wallRightOffset);

		XMMATRIX wallBackOffset = XMMatrixTranslation(0.0f, 4.0f, -8.f);
		XMStoreFloat4x4(&m_wallBackWorld, wallBackOffset);

		XMMATRIX wallFloorOffset = XMMatrixTranslation(0.0f, -1.0f, 0.0f);
		XMStoreFloat4x4(&m_wallFloorWorld, wallFloorOffset);

		XMMATRIX wallRoofOffset = XMMatrixTranslation(0.0f, +9.0f, 0.0f);
		XMStoreFloat4x4(&m_wallRoofWorld, wallRoofOffset);

		XMStoreFloat4x4(&m_crateWorld, identityMatrix);

	}

	void Graphics::InitLight()
	{
		m_dirLights[0].ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.5f);
		m_dirLights[0].diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 0.5f);
		m_dirLights[0].specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
		m_dirLights[0].direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

		m_spotLight.ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		m_spotLight.diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_spotLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_spotLight.att      = XMFLOAT3(1.0f, 0.0f, 0.0f);
		m_spotLight.spot     = 96.0f;
		m_spotLight.range    = 1000.0f;

		m_staticSpotLight.ambient = XMFLOAT4(0.0f, 0.3f, 0.4f, 1.0f);
		m_staticSpotLight.diffuse  = XMFLOAT4(0.3f, 0.3f, 0.4f, 1.0f);
		m_staticSpotLight.specular = XMFLOAT4(0.3f, 0.3f, 0.5f, 1.0f);
		m_staticSpotLight.att      = XMFLOAT3(1.0f, 0.0f, 0.0f);
		m_staticSpotLight.spot     = 96.0f;
		m_staticSpotLight.range    = 1000.0f;
		m_staticSpotLight.position = XMFLOAT3(0.0f, 9.0f, 0.0f);
		m_staticSpotLight.direction = XMFLOAT3(0.0f, -1.0f, 0.0f);

		m_pointLight.ambient  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		m_pointLight.diffuse  = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
		m_pointLight.specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
		m_pointLight.att      = XMFLOAT3(0.0f, 0.1f, 0.0f);
		m_pointLight.range    = 25.0f;
		m_pointLight.position = XMFLOAT3(5.0f, 5.0f, 5.0f);

		m_boxMat.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		m_boxMat.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_boxMat.specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
		m_boxMat.reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	}

	void Graphics::LoadTextures()
	{
		//Just load single brick texture
		HR(D3DX11CreateShaderResourceViewFromFile(m_device, L"Textures/darkbrickdxt1.dds", NULL, NULL, &m_diffuseMap, NULL));
		//And crate texture
		HR(D3DX11CreateShaderResourceViewFromFile(m_device, L"Textures/WoodCrate01.dds", NULL, NULL, &m_crateDiffuseMap, NULL));
	}
}