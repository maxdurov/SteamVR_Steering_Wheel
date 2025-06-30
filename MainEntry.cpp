
#include "Header.h"
#include <Windows.h>
#include <windowsx.h>

#include <thread>
#include <chrono>



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

WNDCLASS winClass;
HWND hwnd;

LPCWSTR CLASS_NAME = L"Steering Wheel SteamVR";

GUI* gui = nullptr;

HWND InitWindow(HINSTANCE hInst) {
	winClass = {};
	winClass.lpfnWndProc = WindowProc;
	winClass.hInstance = hInst;
	winClass.lpszClassName = CLASS_NAME;

	RegisterClassW(&winClass);

	HWND hwnd = CreateWindowExW(

		0, // optional style
		CLASS_NAME, // win class
		CLASS_NAME, // win text
		WS_OVERLAPPEDWINDOW, // win style

		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // size and pos

		NULL, // parent win
		NULL, // menu
		hInst,
		NULL // additional application data

	);

	return hwnd;

}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {

	hwnd = InitWindow(hInstance);
	Helper::hMainWin = hwnd;

	ShowWindow(hwnd, nShowCmd);

	MSG msg = {  };
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	/*while (true) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return 0;
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		Sleep(10);
	}*/



	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	
		gui = new GUI(hwnd);
		
		return 0;

	case WM_COMMAND:
		gui->WM_COMMAND_HANDLER(wParam);
		return 0;

	case WM_MAIN_THREAD_FUNCTION:
	{
		std::function<void()>* fun = (std::function<void()>*) wParam;
		if (fun) (*fun)();
		delete fun;
		return 0;
	}

	case WM_SIZE:
	{
		gui->WM_RESIZE_HANDLER();
		return 0;
	}


	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// All painting occurs here, between BeginPaint and EndPaint.

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		gui->WM_PAINT_HANDLER(hdc);


		EndPaint(hwnd, &ps);
		return 0;
	}

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}