#include "CommandQueue.h"

void CommandQueue::Create(ComPtr<ID3D12Device6> device, D3D12_COMMAND_LIST_TYPE type)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = type;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	m_Device = device;
	ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));

}

void CommandQueue::InitSyncObjects(UINT frameBufferIndex, std::vector<FrameResource> FrameResources)
{
	ThrowIfFailed(m_Device->CreateFence(FrameResources[frameBufferIndex].FenceValue, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_Fence)));
	FrameResources[frameBufferIndex].FenceValue++;

	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_FenceEvent == nullptr)
	{
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	}

	WaitForGPU(frameBufferIndex, FrameResources);
}

void CommandQueue::WaitForGPU(UINT& framBufferIndex, std::vector<FrameResource> FrameResources)
{
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), FrameResources[framBufferIndex].FenceValue));

	ThrowIfFailed(m_Fence->SetEventOnCompletion(FrameResources[framBufferIndex].FenceValue, m_FenceEvent));
	WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

	FrameResources[framBufferIndex].FenceValue++;
}

void CommandQueue::MoveToNextFrame(FrameResource fr, UINT& frameIndex, ComPtr<IDXGISwapChain3> swapChain)
{
	const UINT64 currentFenceValue = fr.FenceValue;
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), currentFenceValue));

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	if (m_Fence->GetCompletedValue() < currentFenceValue)
	{
		ThrowIfFailed(m_Fence->SetEventOnCompletion(currentFenceValue, m_FenceEvent));
		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
	}

	fr.FenceValue = currentFenceValue + 1;
}