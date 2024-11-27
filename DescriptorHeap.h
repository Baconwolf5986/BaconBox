#pragma once
#include "D3DUtil.h"
#include "stdafx.h"

class DescriptorHeap
{
public:
	DescriptorHeap();
	~DescriptorHeap() {};

	// Create a descriptor heap
	void CreateAsRTVHeap(ComPtr<ID3D12Device6> device, UINT numDescriptors);
	void CreateAsDSVHeap(ComPtr<ID3D12Device6> device, UINT numDescriptors);
	void CreateAsCBVSRVUAVHeap(ComPtr<ID3D12Device6> device, UINT numDescriptors);

	// Getters
	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return m_DescriptorHeap; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleStart() {
		return m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	}

private:
	ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
};

