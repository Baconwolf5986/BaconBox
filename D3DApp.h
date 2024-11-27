#pragma once
#include "stdafx.h"
#include "GameTimer.h"

// Base D3D Application class
class D3DApp
{
public:
	D3DApp();
	D3DApp(HINSTANCE hInstance);

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool InitWindow();
	virtual int Run();

	static D3DApp* GetApp();
	void CalculateFrameStats();

	bool Initialize();

protected:
	static D3DApp* mApp;

	HWND hwnd;
	RECT g_WindowRect;
	LPCWSTR windowName = L"MainWindow";
	LPCWSTR windowTitle = L"MainWindow";
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;

	int m_Width;
	int m_Height;

	bool isFullscreen;
	float m_AspectRatio;

	HINSTANCE m_hInstance;
	GameTimer m_Timer;

	UINT frameBufferCount = 3;
	UINT m_FrameBufferIndex = 0;

	bool m4xMsaaState = false;
	UINT m4xMsaaQuality = 0;
};

