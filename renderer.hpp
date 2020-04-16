#pragma once
#include "includes.hpp"

//window stuff
void render();

WPARAM loop() {
	MSG message;
	RECT rect;
	ZeroMemory(&rect, sizeof(RECT));
	GetWindowRect(g_game_hwnd, &rect);

	while (true) {
		ZeroMemory(&message, sizeof(MSG));

		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		if (message.message == WM_QUIT) { exit(0); }

		MoveWindow(g_hwnd, rect.left, rect.top, g_screen_width, g_screen_height, true);

		HWND hWindow2 = GetForegroundWindow();
		HWND hWindow3 = GetWindow(hWindow2, GW_HWNDPREV);

		SetWindowPos(g_hwnd, hWindow3, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		UpdateWindow(g_hwnd);

		render();
		
		Sleep(5);
	}
	return message.wParam;
}

void init_d3d(HWND hwnd) {
	g_d3 = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS direct3DPresetParams;
	ZeroMemory(&direct3DPresetParams, sizeof(direct3DPresetParams));

	direct3DPresetParams.Windowed = true;
	direct3DPresetParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	direct3DPresetParams.hDeviceWindow = hwnd;
	direct3DPresetParams.BackBufferFormat = D3DFMT_A8R8G8B8;
	direct3DPresetParams.BackBufferWidth = g_screen_width;
	direct3DPresetParams.BackBufferHeight = g_screen_height;
	direct3DPresetParams.EnableAutoDepthStencil = TRUE;
	direct3DPresetParams.AutoDepthStencilFormat = D3DFMT_D16;

	g_d3->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &direct3DPresetParams, &g_d3dev);
	D3DXCreateFontA(g_d3dev, 12, NULL, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &g_d3Font);
	D3DXCreateLine(g_d3dev, &g_d3Line);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static MARGINS margins = { 0, 0, g_screen_width, g_screen_height };

	switch (message) {
		case WM_CREATE:
			DwmExtendFrameIntoClientArea(hWnd, &margins);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProcA(hWnd, message, wParam, lParam);
}

void setup_window() {
	RECT rect;
	GetWindowRect(g_game_hwnd, &rect);

	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	windowClass.lpszClassName = "epic overlay";
	RegisterClassExA(&windowClass);

	g_hwnd = CreateWindowExA(WS_EX_LAYERED | WS_EX_TRANSPARENT, windowClass.lpszClassName, " ", WS_POPUP, rect.left, rect.top, g_screen_width, g_screen_height, NULL, NULL, windowClass.hInstance, NULL);

	SetLayeredWindowAttributes(g_hwnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
	SetLayeredWindowAttributes(g_hwnd, 0, 255, LWA_ALPHA);

	ShowWindow(g_hwnd, SW_SHOW);
	init_d3d(g_hwnd);
}

//drawing functions
constexpr double M_PI = 3.141592653589793116;
constexpr D3DCOLOR epic_blue = D3DCOLOR_RGBA(0x41, 0x87, 0xF5, 0xFF);

constexpr D3DCOLOR red       = D3DCOLOR_RGBA(0xFF, 0x00, 0x00, 0xFF);
constexpr D3DCOLOR green     = D3DCOLOR_RGBA(0x00, 0xFF, 0x00, 0xFF);
constexpr D3DCOLOR blue      = D3DCOLOR_RGBA(0x00, 0x00, 0xFF, 0xFF);
constexpr D3DCOLOR black     = D3DCOLOR_RGBA(0x00, 0x00, 0x00, 0xFF);
constexpr D3DCOLOR white     = D3DCOLOR_RGBA(0xFF, 0xFF, 0xFF, 0xFF);

void draw_line(float x0, float y0, float x1, float y1, D3DCOLOR color) {
	D3DXVECTOR2 lines[2] = {
		D3DXVECTOR2(x0, y0),
		D3DXVECTOR2(x1, y1)
	};
	g_d3Line->Begin();
	g_d3Line->Draw(lines, 2, color);
	g_d3Line->End();
}

void draw_rect(float x, float y, float w, float h, D3DCOLOR color) {
	draw_line(x, y, x + w, y, color);
	draw_line(x, y, x, y + h, color);
	draw_line(x + w, y, x + w, y + h, color);
	draw_line(x, y + h, x + w + 1, y + h, color);
}

void draw_filled_rect(float x, float y, float w, float h, D3DCOLOR color) {
	D3DTLVERTEX qV[4] = {
		{ x, y + h, 0.f, 1.f, color },
		{ x, y, 0.f, 1.f, color },
		{ x + w, y + h, 0.f, 1.f, color },
		{ x + w, y , 0.f, 1.f, color }
	};
	g_d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_d3dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_d3dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	g_d3dev->SetTexture(0, nullptr);
	g_d3dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, qV, sizeof(D3DTLVERTEX));
}

void draw_healthbars(float x, float y, float w, float h, float val, float max, D3DCOLOR color) {
	draw_rect(x, y - 1, w + 1, h + 2, black);
	draw_filled_rect(x, y, w, val / max * h, color);
	
}

void draw_outlined_rect(float x, float y, float w, float h, D3DCOLOR color) {
	draw_rect(x - 1, y - 1, w + 2, h + 2, black);
	draw_rect(x + 1, y + 1, w - 2, h - 2, black);
	draw_rect(x, y, w, h, color);
}

void draw_circle(float x, float y, float r, int num_sides, D3DCOLOR color) {
	D3DXVECTOR2 Line[128];
	double Step = M_PI * 2.0 / num_sides;
	int Count = 0;
	for (double a = 0; a < M_PI * 2.0; a += Step)
	{
		float X1 = r * cos(a) + x;
		float Y1 = r * sin(a) + y;
		float X2 = r * cos(a + Step) + x;
		float Y2 = r * sin(a + Step) + y;
		Line[Count].x = X1;
		Line[Count].y = Y1;
		Line[Count + 1].x = X2;
		Line[Count + 1].y = Y2;
		Count += 2;
	}
	g_d3Line->Draw(Line, Count, color);
}