#include "Header.h"
#include <Windows.h>
#include <windowsx.h>
#include <thread>

GUI::GUI(HWND hMainWin) {
	this->hMainWin = hMainWin;
	this->Init();
}

void GUI::Init() {
	gui = new Interface(hMainWin);

	gui->AddCheckBox(L"Activate steering wheel", std::function<void(HWND)>([this](HWND obj) {
		
		if (Button_GetCheck(obj)) {

			if (this->vrwheel != nullptr) {
				Button_SetCheck(obj, true);
				return;
			}

			vrwheel = new VRWheel(this->hMainWin, gui);
			if (vrwheel->err_init)
			{
				MessageBox(obj, L"Error init VRWheel", L"Error", MB_OK);
				delete vrwheel;
				vrwheel = nullptr;
				Button_SetCheck(obj, false);
				return;
			}

			vrwheel->Start(std::function<void()>([]() {
				//MessageBox(Helper::hMainWin, L"Callback is works", L"Message", MB_OK);
				}));

			Button_SetCheck(obj, true);

		}
		else {

			if (vrwheel == nullptr) {
				Button_SetCheck(obj, false);
				return;
			}

			vrwheel->Stop();

			delete vrwheel;
			vrwheel = nullptr;
			Button_SetCheck(obj, false);
		}

		}));
}

void GUI::WM_COMMAND_HANDLER(WPARAM wParam) {
	this->gui->WM_HANDLER(wParam);
}

void GUI::WM_RESIZE_HANDLER() {
	this->gui->WM_RESIZE();
}

void GUI::WM_PAINT_HANDLER(HDC hDc) {
	this->gui->WM_PAINT_(hDc);
}