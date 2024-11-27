#pragma once
#include "stdafx.h"
#include "D3DUtil.h"

class PipelineStateObject
{
public:
	PipelineStateObject(ComPtr<ID3D12Device6> device);
	PipelineStateObject();
	~PipelineStateObject();

	// Different pipeline state objects for different object types
	void initPSO(ComPtr<ID3D12RootSignature> rootSignature, 
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[], UINT elementCount,
		LPCWSTR vertexShader, LPCWSTR pixelShader, ComPtr<ID3D12Device6> device);

	// Solid objects
	void CreateAsOpaquePSO();
	// Clear Objects
	void CreateAsTransparentPSO();
	// Perforated Objects (Semi-Transparent)
	void CreateAsAlphaTestedPSO();

	// Stencil objects
	void CreateAsShadowMapPSO();
	
	// Debugging
	void CreateAsWireframePSO();

	ID3D12PipelineState* GetPipeline() { return m_PipelineStateObject.Get(); }

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC	m_psoDesc;
	ComPtr<ID3D12PipelineState>			m_PipelineStateObject;
	ComPtr<ID3D12Device6>				m_Device;

	ComPtr<ID3DBlob> 					m_VSByteCode;
	ComPtr<ID3DBlob> 					m_PSByteCode;
};

