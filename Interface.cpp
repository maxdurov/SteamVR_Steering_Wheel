

#include "Header.h"
#include <Windows.h>
#include <vector>
#include <functional>
#include <windowsx.h>

#include <string>

#pragma comment(lib, "Msimg32.lib")

Interface::Interface(HWND hParent) {
	this->hParent = hParent;
}

int Interface::AddCheckBox(LPCWSTR title, std::function<void (HWND)> func) {
		HWND newCheckBox = CreateWindow(
			L"BUTTON",
			title,
			WS_TABSTOP | WS_VISIBLE |
			WS_CHILD | BS_AUTOCHECKBOX,
			20, 0, 150, 20,
			this->hParent,
			(HMENU)this->ID_NUM,        // ID
			(HINSTANCE)GetWindowLongPtr(this->hParent, GWLP_HINSTANCE),
			NULL
		);

		this->hObjects.push_back({ newCheckBox, func });

		this->ID_NUM = ID_NUM + 1;
		this->UpdatePositions();
		UpdateWindow(hParent);

	return ID_NUM - 1;
}



int Interface::AddText(LPCWSTR title, std::function<void(HWND)> func) {
		HWND newText = CreateWindow(
			L"STATIC",
			title,
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
			20, 0, 150, 20,
			this->hParent,
			(HMENU)this->ID_NUM,
			(HINSTANCE)GetWindowLongPtr(this->hParent, GWLP_HINSTANCE),
			NULL
		);

		this->hObjects.push_back({ newText, func });
		this->ID_NUM = ID_NUM + 1;
		this->UpdatePositions();
		UpdateWindow(hParent);

	return ID_NUM - 1;

};

HWND Interface::GetObjectByID(int id) {
	for (auto &[obj, fun] : this->hObjects) {
		if (id == GetDlgCtrlID(obj)) return obj;
	}
	return nullptr;
}

void Interface::DeleteObjectByID(int id) {
	int i = 0;
	for (auto& [obj, fun] : this->hObjects) {
		if (id == GetDlgCtrlID(obj)) {
			DestroyWindow(obj);
			Helper::Debug({ "Delete object" });
			Helper::Debug({ (int)this->hObjects.size() });
			Helper::Debug({ i });
			this->hObjects.erase(this->hObjects.begin() + i);
			this->UpdatePositions();
			UpdateWindow(this->hParent);
			return;
		}
		i++;
	}
}

void Interface::DeleteAllObjects() {
	
	int i = 0;
	for (auto& [obj, fun] : this->hObjects) {
			DestroyWindow(obj);
			Helper::Debug({ "Delete object" });
			Helper::Debug({ (int)this->hObjects.size() });
			i++;
	}
	this->hObjects.clear();
	this->UpdatePositions();
	UpdateWindow(this->hParent);
}

void Interface::UpdatePositions() {
	this->HEIGHT_POINT = 20;
	for (auto& [obj, fun] : this->hObjects) {
		SetWindowPos(obj, NULL, 20, this->HEIGHT_POINT, 500, 20, NULL);
		this->HEIGHT_POINT = this->HEIGHT_POINT + this->POS_SHIFT;
	}
}

void Interface::WM_HANDLER(WPARAM wParam) {
	
		for (const auto& [obj, fun] : this->hObjects) {
			if (wParam == GetDlgCtrlID(obj)) {
				if (fun) fun(obj);
				break;
			}
		}
}

Interface::~Interface() {

}

void Interface::WM_RESIZE() {
	InvalidateRect(this->hParent, NULL, TRUE);
}

void Interface::WM_PAINT_(HDC hDc) {

	std::string str = (Helper::ToConvert(Helper::GetFolderPath()));

	RECT rect;
	GetClientRect(this->hParent, &rect);

	int imgW = 128;
	int imgH = 128;

	int posX = (rect.right - imgW) / 2;
	int posY = rect.bottom - imgH;

	Helper::DrawImageWithAlpha(hDc, this->hParent, (str + "\\media\\statue-of-liberty.png").c_str(), 128, 128, posX, posY);

	Helper::DrawImageWithAlpha(hDc, this->hParent, (str + "\\media\\sign.png").c_str(), 400, 78, (rect.right - 400) / 2 + 4, rect.bottom - 400);


}

