#include "PipelineStateObject.h"
PipelineStateObject::PipelineStateObject()
{
	m_psoDesc = {};
    m_psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // Change to wireframeDesc || CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT)
    m_psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    m_psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    m_psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    m_psoDesc.SampleMask = UINT_MAX;
    m_psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    m_psoDesc.NumRenderTargets = 1;
    m_psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_psoDesc.SampleDesc.Count = 1;
}

PipelineStateObject::~PipelineStateObject()
{
}

void PipelineStateObject::initPSO(ComPtr<ID3D12RootSignature> rootSignature,
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[], UINT elementCount,
    LPCWSTR vertexShader, LPCWSTR pixelShader, ComPtr<ID3D12Device6> device)
{
#if defined(_DEBUG)
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    m_Device = device;

    ThrowIfFailed(D3DCompileFromFile(vertexShader, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_1", compileFlags, 0, &m_VSByteCode, nullptr));
    ThrowIfFailed(D3DCompileFromFile(pixelShader, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_1", compileFlags, 0, &m_PSByteCode, nullptr));

    m_psoDesc.InputLayout = { inputElementDescs, elementCount };
    m_psoDesc.pRootSignature = rootSignature.Get();
    m_psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_VSByteCode.Get());
    m_psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_PSByteCode.Get());
}

void PipelineStateObject::CreateAsOpaquePSO()
{
    ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&m_psoDesc, IID_PPV_ARGS(&m_PipelineStateObject)));
}

void PipelineStateObject::CreateAsTransparentPSO()
{
    
}

void PipelineStateObject::CreateAsAlphaTestedPSO()
{

}

void PipelineStateObject::CreateAsShadowMapPSO()
{

}

void PipelineStateObject::CreateAsWireframePSO()
{
	m_psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	m_psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&m_psoDesc, IID_PPV_ARGS(&m_PipelineStateObject)));
}