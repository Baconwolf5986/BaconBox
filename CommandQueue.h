#pragma once
#include "stdafx.h"
#include "D3DUtil.h"
#include "Resource.h"

class CommandQueue
{
public: 
	CommandQueue() {};
	
	void Create(ComPtr<ID3D12Device6> device, D3D12_COMMAND_LIST_TYPE type);
	void InitSyncObjects(UINT frameBufferIndex, std::vector<FrameResource> frameResources);
	void WaitForGPU(UINT& frameBufferIndex, std::vector<FrameResource> frameResources);

	void MoveToNextFrame(FrameResource fr, UINT& frameBufferIndex, ComPtr<IDXGISwapChain3> swapChain);

	ID3D12CommandQueue* GetCommandQueue() { return m_CommandQueue.Get(); }


private:
	ComPtr<ID3D12Device>		m_Device;
	D3D12_COMMAND_LIST_TYPE		m_type;

	ComPtr<ID3D12CommandQueue>	m_CommandQueue;
	ComPtr<ID3D12Fence>			m_Fence;
	UINT64						m_FenceValue;
	HANDLE						m_FenceEvent;
};

