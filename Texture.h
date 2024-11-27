#pragma once
#include "stdafx.h"
#include "D3DUtil.h"

// Class representing a texture covering a mesh
struct Texture
{
	std::string name;
	std::wstring filename;
	UINT srvHeapIndex = -1;

	ComPtr<ID3D12Resource> TextureResource;
	ComPtr<ID3D12Resource> UploadResource;

	Texture LoadTexture(D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc);
};

