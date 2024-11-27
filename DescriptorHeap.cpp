#include "DescriptorHeap.h"
DescriptorHeap::DescriptorHeap()
{

}

void DescriptorHeap::CreateAsRTVHeap(ComPtr<ID3D12Device6> device, UINT numDescriptors)
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = numDescriptors;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;

	ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap)));
}

void DescriptorHeap::CreateAsDSVHeap(ComPtr<ID3D12Device6> device, UINT numDescriptors)
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = numDescriptors;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;

	ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap)));
}

void DescriptorHeap::CreateAsCBVSRVUAVHeap(ComPtr<ID3D12Device6> device, UINT numDescriptors)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavHeapDesc;
	cbvSrvUavHeapDesc.NumDescriptors = numDescriptors;
	cbvSrvUavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvSrvUavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvSrvUavHeapDesc.NodeMask = 0;

	ThrowIfFailed(device->CreateDescriptorHeap(&cbvSrvUavHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap)));
}