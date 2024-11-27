#include "D3DApp.h"

LRESULT CALLBACK WndProc
(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return D3DApp::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

D3DApp* D3DApp::mApp = nullptr;
D3DApp* D3DApp::GetApp()
{
	return mApp;
}

D3DApp::D3DApp()
{
	mApp = this;
	isFullscreen = false;
	m_Width = 1280;
	m_Height = 720;
}

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (MessageBox(0, L"Are you sure you want to exit?",
				L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
				DestroyWindow(hwnd);
		}
		return 0;
	case WM_SIZE:
		// Get the new size of the window
		m_Width = LOWORD(lParam);
		m_Height = HIWORD(lParam);
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

D3DApp::D3DApp(HINSTANCE hInstance) : m_hInstance(hInstance)
{
	m_Width = 1280;
	m_Height = 720;
	m_AspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	isFullscreen = false;
}

bool D3DApp::InitWindow()
{
	if (isFullscreen)
	{
		HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		if (!GetMonitorInfo(hmon, &mi)) return false;

		m_Width = mi.rcMonitor.right - mi.rcMonitor.left;
		m_Height = mi.rcMonitor.bottom - mi.rcMonitor.top;
	}

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = windowName;

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	RECT R = { 0, 0, m_Width, m_Height };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	hwnd = CreateWindowEx(NULL,
		windowName,
		windowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		NULL,
		NULL,
		m_hInstance,
		NULL);

	if (!hwnd)
	{
		MessageBox(NULL, L"Error creating window",
			L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (isFullscreen)
	{
		SetWindowLong(hwnd, GWL_STYLE, 0);
	}

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	m_AspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);

	return true;
}

void D3DApp::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((m_Timer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);

		std::wstring windowText = std::wstring(windowTitle) +
			L"    fps: " + fpsStr +
			L"   mspf: " + mspfStr;

		SetWindowText(hwnd, windowText.c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

int D3DApp::Run()
{
	MSG msg = { 0 };

	m_Timer.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Update the game timer.
			m_Timer.Tick();
			Sleep(100);
			CalculateFrameStats();
		}
	}

	return (int)msg.wParam;
}


bool D3DApp::Initialize()
{
	if (!InitWindow())
	{
		MessageBox(0, L"Window Initialization - Failed",
			L"Error", MB_OK);
		return false;
	}

	return true;
}