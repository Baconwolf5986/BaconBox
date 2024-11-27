#include "Resource.h"
// =================================================
// Frame Resources
// =================================================

void FrameResource::CreateCommandAllocator(ComPtr<ID3D12Device6> device)
{
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator)));
}

// Creating the CBV Buffers
void FrameResource::CreatePassConstantBuffer(ComPtr<ID3D12Device6> device)
{
	UINT bufferSize = sizeof(PassConstants);
	PassCB.CreateBuffer(device, sizeof(PassConstants), CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD));
	PassCB.MapResource(&PassCB.Data, 1);
}

void FrameResource::CreateObjectConstantBuffer(ComPtr<ID3D12Device6> device)
{
	UINT bufferSize = ObjectCount*sizeof(ObjectConstants);
	ObjectCB.CreateBuffer(device, bufferSize, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD));
	ObjectCB.MapResource(&ObjectCB.Data, 2);
}

void FrameResource::CreateMaterialConstantBuffer(ComPtr<ID3D12Device6> device)
{
	UINT bufferSize = ObjectCount*sizeof(MaterialConstants);
	MaterialCB.CreateBuffer(device, bufferSize, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD));
	MaterialCB.MapResource(&MaterialCB.Data, 2);
}

void FrameResource::CreateLightConstantBuffer(ComPtr<ID3D12Device6> device)
{
	UINT bufferSize = 1 * sizeof(LightConstants);
	LightCB.CreateBuffer(device, bufferSize, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD));
	LightCB.MapResource(&LightCB.Data, 1);
}