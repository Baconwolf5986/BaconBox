#pragma once
#include "D3DApp.h"
#include "D3DUtil.h"
#include "stdafx.h"
//
#include "PipelineStateObject.h"
#include "CommandQueue.h"
#include "Resource.h"
#include "DescriptorHeap.h"
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
// DirectXTK
#include <DDSTextureLoader.h>
#include <ResourceUploadBatch.h>

class BaconBox : public D3DApp
{
public:
	BaconBox() {};

	int Run() override;

	// D3D Initialization
	// Main
	void initD3D();
	// Initialization Helpers
	void initDevice(UINT dxgiFactoryFlags);
	
	void createFrameResources();
	void createDepthStencilBuffer();
	void createSwapChain();

	void createRootSignature();
	void buildPipelines();

	void createCommandList();
	void executeInitCommandList();

	// Init Rendering Objects
	void createMaterials();
	void createRenderItems();
	void loadTextures();
	void SetLights();

	// Render Helper Structures

	// Struct representing a single object in the world
	// Contains: Material, Texture and Vertex/Index Buffers
	struct RenderItem
	{
		Mesh mesh;
		std::string material;
		std::string texture;

		UINT vertexBufferStartIndex;
		UINT indexBufferStartIndex;
		UINT objectCBIndex;
		UINT srvHeapIndex = -1;
	};

	// Struct representing a 
	struct RenderStage
	{
		std::string					name;
		PipelineStateObject*		pipeline;

		// Rendering Data
		std::map<std::string, RenderItem> renderItems;
		Resource<Vertex>			vertexBuffer;
		Resource<UINT16>			indexBuffer;
		UINT						indexBufferSize;
		D3D12_VERTEX_BUFFER_VIEW	VBV;
		D3D12_INDEX_BUFFER_VIEW		IBV;
	};

	// RENDERING
	void Render();
	// RENDERING HELPERS
	void UpdatePassCB(FrameResource& fr);
	void UpdateObjectCB(FrameResource& fr);
	void UpdateMaterialCB(FrameResource& fr);
	void UpdateLightCB(FrameResource& fr);
	void DrawRenderStages(FrameResource& fr);
	void DrawRenderItems(RenderStage& stage, FrameResource& fr);

private:
	// D3D12 device objects
	ComPtr<ID3D12Device6>				m_Device;
	ComPtr<IDXGIFactory4>				m_Factory;

	// D3D12 objects
	ComPtr<ID3D12RootSignature>			m_RootSignature;
	ComPtr<IDXGISwapChain3>				m_SwapChain;
	CommandQueue						m_CommandQueue;
	ComPtr<ID3D12GraphicsCommandList>	m_CommandList;

	// Descriptor Heaps
	DescriptorHeap						m_RTVDescriptorHeap;
	DescriptorHeap						m_DSVDescriptorHeap;
	DescriptorHeap						m_SRVDescriptorHeap;

	// Pipelines
	PipelineStateObject					m_OpaquePipelineStateObject;

	// Depth Stencil Buffer
	ComPtr<ID3D12Resource>				m_DepthStencilBuffer;

	// Frame Resources
	std::vector<FrameResource> 			m_FrameResources;

	// Render Items and their materials
	std::vector<RenderStage>						m_RenderStages;
	std::unordered_map<std::string, Material>		m_Materials;
	std::unordered_map<std::string, Texture>		m_Textures;

	// Matrices
	XMMATRIX							m_ViewMatrix;
	XMMATRIX							m_ProjectionMatrix;

	Camera								m_Camera;
	LightManager						m_LightManager;

	// Screen Render Space
	CD3DX12_VIEWPORT					m_ScreenViewport;
	CD3DX12_RECT						m_ScissorRect;
	
};

