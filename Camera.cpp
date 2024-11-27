#include "Camera.h"

Camera::Camera()
{
	c_position = { 0.0f, 3.0f, -6.0f, 0.0f };	// Where the camera is located
	c_lookAt = { 0.0f, 0.0f, 0.0f, 0.0f };		// Where the camera is looking
	c_up = { 0.0f, 0.5f, 0.0f, 0.0f };			// Which direction is up (Relative to the camera)
}

XMMATRIX Camera::GetViewMatrix()
{
	XMMATRIX viewMatrix = XMMatrixLookAtLH(c_position, c_lookAt, c_up);

	return viewMatrix;
}