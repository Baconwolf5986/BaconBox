#pragma once
#include "stdafx.h"
#include "D3DUtil.h"

template <typename T>
class Resource
{
public:
	Resource() {};
	Resource(T value) : Data(value) {};
	T Data;

	ComPtr<ID3D12Resource> m_Resource = nullptr;
	ComPtr<ID3D12Resource> m_UploadBuffer = nullptr;
	BYTE* m_MappedData = nullptr;

	void CreateBuffer(ComPtr<ID3D12Device6> device, UINT bufferSize, const D3D12_HEAP_PROPERTIES& heapProps)
	{
		const D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

		ThrowIfFailed(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&m_Resource)));
	}

	void UploadToDefaultBuffer(ComPtr<ID3D12Device6> device,
		ComPtr<ID3D12GraphicsCommandList> cmdList, T* data, UINT elementCount)
	{
		UINT bufferSize = sizeof(T) * elementCount;
		const D3D12_HEAP_PROPERTIES uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const D3D12_HEAP_PROPERTIES defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		const D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

		// Create upload buffer
		ThrowIfFailed(device->CreateCommittedResource(
			&uploadHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_UploadBuffer)));

		// Create default buffer
		ThrowIfFailed(device->CreateCommittedResource(
			&defaultHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&m_Resource)));

		// Map the data to the upload buffer
		UINT8* pDataBegin = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(m_UploadBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin)));
		memcpy(pDataBegin, data, bufferSize);
		m_UploadBuffer->Unmap(0, nullptr);

		// Use the command list to copy the vertices to the default heap
		cmdList->CopyBufferRegion(m_Resource.Get(), 0, m_UploadBuffer.Get(), 0, bufferSize);
		// Transition buffer to be used for rendering
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_Resource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		cmdList->ResourceBarrier(1, &barrier);
	}

	void MapResource(T* data, UINT objectCount)
	{
		D3D12_RANGE readRange = { 0, 0 };
		ThrowIfFailed(m_Resource->Map(0, &readRange, reinterpret_cast<void**>(&m_MappedData)));
	}

	void CopyToMappedResource(T* data, UINT objectCount)
	{
		memcpy(m_MappedData, data, sizeof(T) * objectCount);
	}

	ComPtr<ID3D12Resource> GetResource() { return m_Resource; }
};

struct FrameResource
{
public:
	ComPtr<ID3D12CommandAllocator>		CommandAllocator;
	ComPtr<ID3D12Fence>					Fence;
	UINT64								FenceValue;

	Resource<ObjectConstants>			ObjectCB;
	Resource<PassConstants>				PassCB;
	Resource<MaterialConstants>			MaterialCB;
	Resource<LightConstants>			LightCB;

	// Back Buffer
	ComPtr<ID3D12Resource>				renderTarget;
	D3D12_CPU_DESCRIPTOR_HANDLE			RTVHandle;

	UINT								ObjectCount = 2;

	// Helper functions
	void CreateCommandAllocator(ComPtr<ID3D12Device6> device);

	// CBV Buffers
	void CreatePassConstantBuffer(ComPtr<ID3D12Device6> device);
	void CreateObjectConstantBuffer(ComPtr<ID3D12Device6> device);
	void CreateMaterialConstantBuffer(ComPtr<ID3D12Device6> device);
	void CreateLightConstantBuffer(ComPtr<ID3D12Device6> device);
};

