#include "BaconBox.h"

// Base run command
int BaconBox::Run()
{
	MSG msg = { 0 };

	m_Timer.Reset();
	initD3D();

	while (msg.message != WM_QUIT)
	{
		Render();
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);			
		}
		else
		{
			// Update the game timer.
			m_Timer.Tick();
			Sleep(100);
			CalculateFrameStats();
		}
	}

	return (int)msg.wParam;
}

// ====================================================================================================
// D3D12 Main Initialization Functions
// ====================================================================================================
void BaconBox::initD3D()
{
	// Get hardware properties
	UINT dxgiFactoryFlags = 0;
	// Enable debug layer
#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();

			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	// Initialize D3D12 Device
	initDevice(dxgiFactoryFlags);

	// Create render target heap
	// Create depth stencil heap
	m_RTVDescriptorHeap.CreateAsRTVHeap(m_Device, frameBufferCount);
	m_DSVDescriptorHeap.CreateAsDSVHeap(m_Device, 1);
	createDepthStencilBuffer();

	// Create the command queue
	m_CommandQueue.Create(m_Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	createSwapChain();

	// Initialize the swap chain
	m_FrameBufferIndex = m_SwapChain.Get()->GetCurrentBackBufferIndex();

	// Create the frame resources
	createFrameResources();

	// Create the Fence objects
	m_CommandQueue.InitSyncObjects(m_FrameBufferIndex, m_FrameResources);

	
	// ASSET LOADING
	// -----------------------------------------------------------------------
	// Create the root signature and pipelines
	createRootSignature();
	buildPipelines();

	// Create the command list
	createCommandList();

	// Create Render Items
	createRenderItems();
	// Create Materials
	createMaterials();
	// Load the textures
	loadTextures();
	executeInitCommandList();


	// Initialize the screen render space
	m_ScreenViewport = CD3DX12_VIEWPORT{ 0.0f, 0.0f, (float)m_Width, (float)m_Height, 0.0f, 1.0f };
	m_ScissorRect = CD3DX12_RECT{ 0, 0, static_cast<LONG>(m_Width), static_cast<LONG>(m_Height) };

}

// ====================================================================================================
// D3D12 Device
// ====================================================================================================
void BaconBox::initDevice(UINT dxgiFactoryFlags)
{
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_Factory)));

	ComPtr<IDXGIAdapter> adapter;
	ThrowIfFailed(m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));
	ThrowIfFailed(D3D12CreateDevice(
		adapter.Get(),
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_Device)));
}

// ====================================================================================================
// Frame Resources (Render Targets, Depth Stencil Buffers and Constant Buffers)
// ====================================================================================================
void BaconBox::createFrameResources()
{

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap.GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
	UINT rtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for (int i = 0; i < frameBufferCount; i++)
	{
		FrameResource fr;

		fr.CreateCommandAllocator(m_Device);

		// Assign the render target view to the frame resource
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&fr.renderTarget)));
		m_Device->CreateRenderTargetView(fr.renderTarget.Get(), nullptr, rtvHandle);
		fr.RTVHandle = rtvHandle;
		rtvHandle.Offset(rtvDescriptorSize);

		// TODO: Initialize constant buffers
		fr.CreatePassConstantBuffer(m_Device);
		fr.CreateObjectConstantBuffer(m_Device);
		fr.CreateMaterialConstantBuffer(m_Device);
		fr.CreateLightConstantBuffer(m_Device);

		m_FrameResources.push_back(fr);
	}
}

void BaconBox::createDepthStencilBuffer()
{
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	ThrowIfFailed(m_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_Width, m_Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&m_DepthStencilBuffer)));

	m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &dsvDesc, m_DSVDescriptorHeap.GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
}

// ====================================================================================================
// D3D12 Swap Chain
// ====================================================================================================
void BaconBox::createSwapChain()
{
	ComPtr<IDXGISwapChain> tempSwapChain;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = m_Width;
	swapChainDesc.BufferDesc.Height = m_Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = frameBufferCount;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(m_Factory->CreateSwapChain(
		m_CommandQueue.GetCommandQueue(),
		&swapChainDesc, 
		tempSwapChain.GetAddressOf()));

	ThrowIfFailed(tempSwapChain.As(&m_SwapChain));
}


// ====================================================================================================
// D3D12 Root Signature Functions
// ====================================================================================================
std::array<const CD3DX12_STATIC_SAMPLER_DESC, (size_t)6> GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}

// Initialize root signature
void BaconBox::createRootSignature()
{
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	if (FAILED(m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	CD3DX12_ROOT_PARAMETER rootParameters[5];

	CD3DX12_DESCRIPTOR_RANGE textureTable;
	textureTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	rootParameters[0].InitAsDescriptorTable(1, &textureTable, D3D12_SHADER_VISIBILITY_ALL);

	// Root Parameter 1 as a root descriptor to a constant buffer
	// Pass Constants (Ex: View, Projection matrices) to the shaders
	// Register 0
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].Descriptor.RegisterSpace = 0;
	rootParameters[1].Descriptor.ShaderRegister = 0; // Shader register b0;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// Root Parameter 2 as a root descriptor to a constant buffer
	// Object Specific Constants (Ex: World matrix)
	// Register 1
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[2].Descriptor.RegisterSpace = 0;
	rootParameters[2].Descriptor.ShaderRegister = 1;// Shader register b1;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// Root Parameter 3 as a root descriptor to a constant buffer
	// Material Specific Constants (Ex: Diffuse, Specular, Normal maps)
	// Register 2
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].Descriptor.RegisterSpace = 0;
	rootParameters[3].Descriptor.ShaderRegister = 2;// Shader register b2;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// Root Parameter 4 as a root descriptor to a constant buffer
	// Light Constants, contains an array of light structures
	// Register 3
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[4].Descriptor.RegisterSpace = 0;
	rootParameters[4].Descriptor.ShaderRegister = 3;// Shader register b3;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	auto staticSamplers = GetStaticSamplers();

	// Root Signature Description
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDescription(_countof(rootParameters), rootParameters, 
		(UINT)staticSamplers.size(), staticSamplers.data(), 
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// Serialize the root signature
	ComPtr<ID3DBlob> SerializedSignature;
	ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3D12SerializeRootSignature(
		&rootSignatureDescription, 
		D3D_ROOT_SIGNATURE_VERSION_1, 
		&SerializedSignature, 
		&error));

	ThrowIfFailed(m_Device->CreateRootSignature(0, SerializedSignature->GetBufferPointer(), SerializedSignature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));
}

// ====================================================================================================
// D3D12 COMMAND LIST
// ====================================================================================================
void BaconBox::createCommandList()
{
	// Create the command list
	ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_FrameResources[m_FrameBufferIndex].CommandAllocator.Get(), 
		m_OpaquePipelineStateObject.GetPipeline(), IID_PPV_ARGS(&m_CommandList)));
	ThrowIfFailed(m_CommandList->Close());
}

void BaconBox::executeInitCommandList()
{
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* commandLists[] = { m_CommandList.Get() };
	m_CommandQueue.GetCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);
	m_CommandQueue.WaitForGPU(m_FrameBufferIndex, m_FrameResources);

	OutputDebugString(L"Finished Executing Command List\n");
}

// ====================================================================================================
// D3D12 PIPELINES
// ====================================================================================================
void BaconBox::buildPipelines()
{
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	// Create the opqaque pipeline state object
	m_OpaquePipelineStateObject.initPSO(m_RootSignature, inputElementDescs, _countof(inputElementDescs), L"Shaders/VertexShader.hlsl", L"Shaders/PixelShader.hlsl", m_Device);
	m_OpaquePipelineStateObject.CreateAsOpaquePSO();
	// TODO: Create other pipelines
}

// ====================================================================================================
// Render Items (Initialize vertex buffer, index buffer and CB values to pass to the GPU)
// ====================================================================================================
void BaconBox::createRenderItems()
{
	// Reset the command allocator and command list
	ThrowIfFailed(m_FrameResources[m_FrameBufferIndex].CommandAllocator->Reset());
	ThrowIfFailed(m_CommandList->Reset(m_FrameResources[m_FrameBufferIndex].CommandAllocator.Get(), m_OpaquePipelineStateObject.GetPipeline()));

	// Create the scene geometry
	GeometryGenerator geoGen;

	std::vector<Vertex> combinedVertices;
	std::vector<UINT16> combinedIndices;

	// Create the box
	RenderItem box;
	box.mesh = geoGen.GenerateBox(2.0f, 2.0f, 2.0f);
	box.material = "wood";
	box.vertexBufferStartIndex = 0;
	box.indexBufferStartIndex = 0;
	box.objectCBIndex = 0;
	box.srvHeapIndex = 0;
	combinedVertices = box.mesh.vertices;
	combinedIndices = box.mesh.indices;

	// Create the grid
	RenderItem grid;
	grid.mesh = geoGen.GenerateGrid(10.0f, 10.0f, 5, 5);
	grid.material = "grass";
	grid.vertexBufferStartIndex = combinedVertices.size();
	grid.indexBufferStartIndex = combinedIndices.size();
	grid.objectCBIndex = 1;
	grid.srvHeapIndex = 0;
	combinedVertices.insert(combinedVertices.end(), grid.mesh.vertices.begin(), grid.mesh.vertices.end());
	for (auto index : grid.mesh.indices)
	{
		combinedIndices.push_back(index + (UINT16)grid.vertexBufferStartIndex);
	}

	// Create the vertex and index buffers for the static items
	RenderStage solidObjects;
	solidObjects.name = "static_items";
	// Vertex Buffer
	solidObjects.vertexBuffer.UploadToDefaultBuffer(m_Device, m_CommandList,
		combinedVertices.data(), combinedVertices.size());
	solidObjects.VBV.BufferLocation = solidObjects.vertexBuffer.GetResource()->GetGPUVirtualAddress();
	solidObjects.VBV.StrideInBytes = sizeof(Vertex);
	solidObjects.VBV.SizeInBytes = static_cast<UINT>(sizeof(Vertex) * combinedVertices.size());
	// Index Buffer
	solidObjects.indexBuffer.UploadToDefaultBuffer(m_Device, m_CommandList,
		combinedIndices.data(), combinedIndices.size());
	solidObjects.indexBufferSize = combinedIndices.size();
	solidObjects.IBV.BufferLocation = solidObjects.indexBuffer.GetResource()->GetGPUVirtualAddress();
	solidObjects.IBV.Format = DXGI_FORMAT_R16_UINT;
	solidObjects.IBV.SizeInBytes = static_cast<UINT>(sizeof(UINT16) * combinedIndices.size());
	solidObjects.pipeline = &m_OpaquePipelineStateObject;
	// Add the render items to the rend stage
	solidObjects.renderItems["woodBox"] = box;
	solidObjects.renderItems["grassGrid"] = grid;

	// Store the render buffer
	m_RenderStages.push_back(solidObjects);
}

void BaconBox::createMaterials()
{
	// Create wood material
	Material wood;
	wood.name = "wood";
	wood.DiffuseAlbedo = XMFLOAT4(0.7f, 0.5f, 0.1f, 1.0f);
	wood.FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	wood.Roughness = 0.1f;
	wood.MaterialCBVHeapIndex = 0;
	m_Materials[wood.name] = wood;

	// Create grass material
	Material grass;
	grass.name = "grass";
	grass.DiffuseAlbedo = XMFLOAT4(0.2f, 0.7f, 0.2f, 1.0f);
	grass.FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	grass.Roughness = 0.1f;
	grass.MaterialCBVHeapIndex = 1;
	m_Materials[grass.name] = grass;
}

// D3D12 SRV Textures
// ----------------------------------------------------------------------------------------------------
void BaconBox::loadTextures()
{
	// Use the DirectXTK class ResourceUploadBatch to load textures
	ResourceUploadBatch resourceUpload(m_Device.Get());
	resourceUpload.Begin();

	Texture crateTex;
	crateTex.name = "wood";
	crateTex.filename = L"Textures/WoodCrate02.dds";
	crateTex.srvHeapIndex = 0;

	Texture grassTex;
	grassTex.name = "grass";
	grassTex.filename = L"Textures/grass.dds";
	grassTex.srvHeapIndex = 1;

	m_Textures[crateTex.name] = crateTex;
	m_Textures[grassTex.name] = grassTex;

	m_SRVDescriptorHeap.CreateAsCBVSRVUAVHeap(m_Device, m_Textures.size());
	// Use DDSTextureLoad and ResourceUploadBatch to load the textures
	for (auto& tex : m_Textures)
	{
		ThrowIfFailed(DirectX::CreateDDSTextureFromFile(m_Device.Get(), resourceUpload, tex.second.filename.c_str(),
			tex.second.TextureResource.GetAddressOf()));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = tex.second.TextureResource->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = tex.second.TextureResource->GetDesc().MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = m_SRVDescriptorHeap.GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
		srvHandle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * tex.second.srvHeapIndex;
	
		m_Device->CreateShaderResourceView(tex.second.TextureResource.Get(), &srvDesc, srvHandle);
	}

	// Use DDSTextureLoad to upload the texture to the GPU
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile(m_Device.Get(), resourceUpload, crateTex.filename.c_str(),
		crateTex.TextureResource.GetAddressOf()));

	// Upload and wait to complete
	std::future<void> upload = resourceUpload.End(m_CommandQueue.GetCommandQueue());
	upload.wait();

	// Create SRV for the texture
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = crateTex.TextureResource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = crateTex.TextureResource->GetDesc().MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	// Init the descriptor heap
	m_SRVDescriptorHeap.CreateAsCBVSRVUAVHeap(m_Device, 1);

	m_Device->CreateShaderResourceView(crateTex.TextureResource.Get(), &srvDesc, m_SRVDescriptorHeap.GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
}

void BaconBox::SetLights()
{
	// Simple directional light
	m_LightManager.Lights[0].Strength = XMFLOAT3(0.9f, 0.9f, 0.9f);
	m_LightManager.Lights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
	m_LightManager.Lights[0].FalloffStart = 0.5f;
	m_LightManager.Lights[0].FalloffEnd = 100.0f;
	m_LightManager.Lights[0].SpotPower = 0;
}

// ====================================================================================================
// RENDERING
// ====================================================================================================

// UPDATE CONSTANT BUFFERS
// -----------------------------------------------------------------------

// Update the pass constant buffer (View, Projection, Inverse Matrices)
void BaconBox::UpdatePassCB(FrameResource& fr)
{
	PassConstants passCB;

	// Update the view matrix
	XMMATRIX view = m_Camera.GetViewMatrix();
	XMStoreFloat4x4(&passCB.ViewMatrix, XMMatrixTranspose(view));

	// Update the projection matrix
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_AspectRatio, 0.01f, 100.0f);
	XMStoreFloat4x4(&passCB.ProjectionMatrix, XMMatrixTranspose(proj));

	// Update the view projection matrix
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMStoreFloat4x4(&passCB.ViewProjectionMatrix, XMMatrixTranspose(viewProj));

	// Update the inverse matrices
	// Inverse view matrix
	XMStoreFloat4x4(&passCB.InvView, XMMatrixInverse(nullptr, view));
	// Inverse projection matrix
	XMStoreFloat4x4(&passCB.InvProjection, XMMatrixInverse(nullptr, proj));
	// Inverse view projection matrix
	XMStoreFloat4x4(&passCB.InvViewProjection, XMMatrixInverse(nullptr, viewProj));

	// Update the Camera Position
	XMStoreFloat3(&passCB.EyePosition, m_Camera.GetPosition());


	// Set the ambient light
	passCB.AmbientLight = { 0.9f, 0.9f, 0.9f, 1.0f };

	fr.PassCB.CopyToMappedResource(&passCB, 1);
}

// Update the constant buffers for the objects
void BaconBox::UpdateObjectCB(FrameResource& fr)
{
	std::vector<ObjectConstants> objectCBs;
	
	for (int i = 0; i < 2; i++)
	{
		ObjectConstants obj;
		objectCBs.push_back(obj);
	}

	// TODO: Iterative method for each object CB
	// TODO: Change positions for each object (Maybe use map for access)
	
	// First object
	objectCBs[0].WorldMatrix = MathHelper::Identity4x4();
	objectCBs[0].TexTransform = MathHelper::Identity4x4();
	// Second object
	objectCBs[1].WorldMatrix = MathHelper::Identity4x4();
	objectCBs[1].TexTransform = MathHelper::Identity4x4();

	fr.ObjectCB.CopyToMappedResource(objectCBs.data(), objectCBs.size());
}

// Update the constant buffers for materials (Not frequent)
void BaconBox::UpdateMaterialCB(FrameResource& fr)
{
	// TODO: Update material constant buffers based on state
	for (int i = 0; i < frameBufferCount; ++i)
	{
		std::vector<MaterialConstants> matConstants;
		for (auto& material : m_Materials)
		{
			MaterialConstants matC;
			matC.DiffuseAlbedo = material.second.DiffuseAlbedo;
			matC.FresnelR0 = material.second.FresnelR0;
			matC.Roughness = material.second.Roughness;

			matConstants.push_back(matC);
		}

		m_FrameResources[i].MaterialCB.CopyToMappedResource(matConstants.data(), matConstants.size());
	}
}

void BaconBox::UpdateLightCB(FrameResource& fr)
{
	std::vector<LightConstants> lightCBs;
	for (int i = 0; i < frameBufferCount; ++i)
	{
		LightConstants lightCB;
		lightCB.lights[0] = m_LightManager.Lights[0];

		lightCBs.push_back(lightCB);
		m_FrameResources[i].LightCB.CopyToMappedResource(lightCBs.data(), lightCBs.size());
	}
}

// DRAW COMMANDS
// -----------------------------------------------------------------------
void BaconBox::Render()
{
	OutputDebugString(L"Rendering...\n");
	FrameResource& currentFR = m_FrameResources[m_FrameBufferIndex];

	// Update constant buffers
	UpdatePassCB(currentFR);
	UpdateObjectCB(currentFR);
	UpdateMaterialCB(currentFR);
	UpdateLightCB(currentFR);

	// Reset the command allocator and command list for this frame
		// ** TODO: Change pipeline based on render stage (Opaque, Transparent, etc.)
	ThrowIfFailed(currentFR.CommandAllocator->Reset());
	ThrowIfFailed(m_CommandList->Reset(currentFR.CommandAllocator.Get(), m_OpaquePipelineStateObject.GetPipeline()));
	OutputDebugString(L"Reset\n");

	// Indicate the current back buffer to be used as render target
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(currentFR.renderTarget.Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap.GetCPUDescriptorHandleStart(), m_FrameBufferIndex, m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_DSVDescriptorHeap.GetCPUDescriptorHandleStart());
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);
	
	// Clear the render target and depth stencil before drawing
	m_CommandList->ClearRenderTargetView(rtvHandle, Colors::White, 0, nullptr);
	m_CommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// Iterate through each render stage to draw
	DrawRenderStages(currentFR);

	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(currentFR.renderTarget.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_CommandList->Close());

	// Execute the command list
	ID3D12CommandList* commandLists[] = { m_CommandList.Get() };
	m_CommandQueue.GetCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);
	// Move to the next frame
	ThrowIfFailed(m_SwapChain->Present(1, 0));
	m_CommandQueue.MoveToNextFrame(currentFR, m_FrameBufferIndex, m_SwapChain);

	OutputDebugString(L"FinishedRendering...\n");
}

void BaconBox::DrawRenderStages(FrameResource& fr)
{
	for (auto& stage : m_RenderStages)
	{
		// Set the vertex and index lists
		m_CommandList->IASetVertexBuffers(0, 1, &stage.VBV);
		m_CommandList->IASetIndexBuffer(&stage.IBV);

		// Set the input assembler
		m_CommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Set the root signature and screen render space
		m_CommandList->RSSetViewports(1, &m_ScreenViewport);
		m_CommandList->RSSetScissorRects(1, &m_ScissorRect);
		
		// Set the root signature 
		m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
		// Set the pipeline state object
		m_CommandList->SetPipelineState(stage.pipeline->GetPipeline());

		DrawRenderItems(stage, fr);
	}
}

void BaconBox::DrawRenderItems(RenderStage& stage, FrameResource& fr)
{
	// Init constant buffer addresses
	D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = fr.PassCB.GetResource()->GetGPUVirtualAddress();
	D3D12_GPU_VIRTUAL_ADDRESS objectCBAddress = fr.ObjectCB.GetResource()->GetGPUVirtualAddress();
	D3D12_GPU_VIRTUAL_ADDRESS materialCBAddress = fr.MaterialCB.GetResource()->GetGPUVirtualAddress();
	D3D12_GPU_VIRTUAL_ADDRESS lightCBAddress = fr.LightCB.GetResource()->GetGPUVirtualAddress();

	// Init descriptor heap for textures
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_SRVDescriptorHeap.GetDescriptorHeap().Get() };
	m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE srvStart = m_SRVDescriptorHeap.GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	UINT descriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Draw each item
	for (auto& renderItem : stage.renderItems)
	{
		std::wstring name = std::wstring(renderItem.first.begin(), renderItem.first.end());
		OutputDebugString(L"Drawing: ");
		OutputDebugString(name.c_str());
		OutputDebugString(L"\n");

		OutputDebugString(L"Index Buffer Size: ");
		OutputDebugString(std::to_wstring(renderItem.second.mesh.indices.size()).c_str());
		OutputDebugString(L"\n");

		OutputDebugString(L"Index Buffer Start: ");
		OutputDebugString(std::to_wstring(renderItem.second.indexBufferStartIndex).c_str());
		OutputDebugString(L"\n");

		// Get the SRV Handle for the texture from the heap
		UINT srvIndex = renderItem.second.srvHeapIndex;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = srvStart;
		srvHandle.ptr += srvIndex * descriptorSize;
		// Set the constant buffer views
		m_CommandList->SetGraphicsRootDescriptorTable(0, srvHandle);
		m_CommandList->SetGraphicsRootConstantBufferView(1, passCBAddress);
		m_CommandList->SetGraphicsRootConstantBufferView(2, objectCBAddress + UINT(renderItem.second.objectCBIndex * sizeof(ObjectConstants)));
		m_CommandList->SetGraphicsRootConstantBufferView(3, materialCBAddress + UINT((m_Materials[renderItem.second.material].MaterialCBVHeapIndex) * sizeof(MaterialConstants)));
		m_CommandList->SetGraphicsRootConstantBufferView(4, lightCBAddress);
		m_CommandList->DrawIndexedInstanced(renderItem.second.mesh.indices.size() , 1, renderItem.second.indexBufferStartIndex, 0, 0);
	}

}