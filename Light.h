#pragma once
#include "stdafx.h"

struct Light
{
	DirectX::XMFLOAT3 Strength;
	float FalloffStart;
	DirectX::XMFLOAT3 Direction;
	float FalloffEnd;
	DirectX::XMFLOAT3 Position;
	float SpotPower;
};

class LightManager
{
public:
	LightManager() {};

	Light Lights[16];
	int numDirLights = 0;
	int numPointLights = 0;
	int numSpotLights = 0;
};

