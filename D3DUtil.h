#pragma once
#include "stdafx.h"
#include "Light.h"
#include <iostream>

using namespace Microsoft::WRL;
using namespace DirectX;

// Error helper for DirectX API
inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}

// Math Helpers
namespace MathHelper
{
	static XMFLOAT4X4 Identity4x4()
	{
		static XMFLOAT4X4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		return I;
	}
}

// Helper structures
// ------------------------------------------------

// Vertex Structure
struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT3 Tangent;
	DirectX::XMFLOAT2 TexCoord;

	// Vertex Constructor
	Vertex()
	{
		Position = { 0.0f, 0.0f, 0.0f };
		Normal = { 0.0f, 0.0f, 0.0f };
		Tangent = { 0.0f, 0.0f, 0.0f };
		TexCoord = { 1.0f, 0.0f };
	}
	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tanx, float tany, float tanz,
		float texx, float texy
	)
	{
		Position = { px, py, pz };
		Normal = { nx, ny, nz };
		Tangent = { tanx, tany, tanz };
		TexCoord = { texx, texy };
	}

	Vertex(DirectX::XMFLOAT3 p_Position,
		DirectX::XMFLOAT3 p_Normal,
		DirectX::XMFLOAT3 p_Tangent,
		DirectX::XMFLOAT2 p_TexCoord
	)
	{
		Position = p_Position;
		Normal = p_Normal;
		Tangent = p_Tangent;
		TexCoord = p_TexCoord;
	}
};

// Material Struct
struct Material
{
	std::string name;

	// CBV Data
	XMFLOAT4 DiffuseAlbedo;
	XMFLOAT3 FresnelR0;
	float Roughness;

	// Heap Data
	int MaterialCBVHeapIndex = -1;
};



// Constant Buffer Structures
// ------------------------------------------------

// Struct for general values (Camera, Lights, etc.) passed to the GPU
struct PassConstants
{
	XMFLOAT4X4		ViewMatrix = MathHelper::Identity4x4();
	XMFLOAT4X4		ProjectionMatrix = MathHelper::Identity4x4();
	XMFLOAT4X4		ViewProjectionMatrix = MathHelper::Identity4x4();

	XMFLOAT4X4		InvView = MathHelper::Identity4x4();
	XMFLOAT4X4		InvProjection = MathHelper::Identity4x4();
	XMFLOAT4X4		InvViewProjection = MathHelper::Identity4x4();

	XMFLOAT4		AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	XMFLOAT3		EyePosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT2		RenderTargetSize = { 0.0f, 0.0f };
	XMFLOAT2		InvRenderTargetSize = { 0.0f, 0.0f };

	float			NearZ = 0.0f;
	float			FarZ = 0.0f;
	float			TotalTime = 0.0f;
	float			DeltaTime = 0.0f;

	float			padding[17];
};
static_assert((sizeof(PassConstants) % 256) == 0, "PassConstants needs to be 256-byte aligned");

// Struct for object values passed to the GPU
struct ObjectConstants
{
	XMFLOAT4X4 WorldMatrix = MathHelper::Identity4x4();
	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

	float padding[32];
};
static_assert((sizeof(ObjectConstants) % 256) == 0, "ObjectConstants needs to be 256-byte aligned");

// Struct for material values passed to the GPU
struct MaterialConstants
{
	XMFLOAT4 DiffuseAlbedo;
	XMFLOAT3 FresnelR0;
	float Roughness;
	XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();

	float padding[40];
};
static_assert((sizeof(MaterialConstants) % 256) == 0, "MaterialConstants needs to be 256-byte aligned");

// Struct for light values passed to the GPU
struct LightConstants
{
	Light lights[16];
};
static_assert((sizeof(LightConstants) % 256) == 0, "LightConstants needs to be 256-byte aligned");