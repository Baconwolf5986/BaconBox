#pragma once
#include "stdafx.h"
#include "D3DUtil.h"
using namespace DirectX;

class Camera
{
public:
	Camera();

	// Getters
	XMMATRIX GetViewMatrix();
	XMVECTORF32 GetPosition() { return c_position; }

private:
	XMVECTORF32 c_position;	// Where the camera is located
	XMVECTORF32 c_lookAt;		// Where the camera is looking
	XMVECTORF32 c_up;		// Which direction is up (Relative to the camera)
};

