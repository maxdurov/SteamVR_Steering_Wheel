#define STB_IMAGE_IMPLEMENTATION
#include "Header.h"
#include <Windows.h>
#include <cwchar>
#include <vector>
#include "lib/stb_image.h"

#include <glm/glm.hpp>

HWND Helper::hMainWin = nullptr;

void Helper::MainThreadInvoke(std::function<void()>* fun) {
    PostMessage(Helper::hMainWin, WM_MAIN_THREAD_FUNCTION, (WPARAM)fun, NULL);
}

void Helper::MainThreadInvokeSyn(std::function<void()>* fun) {
    SendMessage(Helper::hMainWin, WM_MAIN_THREAD_FUNCTION, (WPARAM)fun, NULL);
}

std::wstring Helper::ToConvert(const char* text) {

	int size = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
	std::wstring wstr(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, text, -1, &wstr[0], size);

	return wstr;

}

void Helper::UpdateState(int ID_ELEM) {
    SendMessage(Helper::hMainWin, WM_COMMAND, ID_ELEM, NULL);
}

std::wstring Helper::GetFolderPath() {
    wchar_t fullPath[MAX_PATH];
    GetModuleFileNameW(nullptr, fullPath, MAX_PATH);

    std::wstring path(fullPath);
    size_t pos = path.find_last_of(L"\\/");  // Find last slash
    std::wstring directory = path.substr(0, pos);
    
    return directory;
}

std::string Helper::ToConvert(std::wstring ws) {
    return std::string(ws.begin(), ws.end());
}

std::wstring Helper::ToConvert(std::string s) {
    return std::wstring(s.begin(), s.end());
}

vr::HmdMatrix34_t Helper::ToConvert(const glm::mat4& mat) {
    vr::HmdMatrix34_t result;

    // Extract the 3x4 from 4x4
    for (int row = 0; row < 3; ++row)
        for (int col = 0; col < 4; ++col)
            result.m[row][col] = mat[col][row];  // transpose for column-major layout

    return result;
}

void Helper::Debug(std::vector<std::string> str) {
    std::string text = "[DEBUG] ";
    
    for (auto& string : str) {
        text.append(string + "\n");
    }
    
    OutputDebugStringA(text.c_str());
}

void Helper::Debug(std::vector<std::wstring> str) {
    std::wstring text = L"[DEBUG] ";

    for (auto& string : str) {
        text.append(string + L"\n");
    }

    OutputDebugStringW(text.c_str());
}

void Helper::Debug(std::vector<int> str) {
    std::string txt = "[DEBUG] ";

    for (auto& _int : str) {
        txt.append(std::to_string(_int) + '\n');
    }

    OutputDebugStringA(txt.c_str());
}

const char* Helper::SteamVRInput_GetInputError(vr::EVRInputError error) {
    switch (error) {
    case vr::VRInputError_None:                 return "No error";
    case vr::VRInputError_NameNotFound:         return "Name not found";
    case vr::VRInputError_WrongType:            return "Wrong type";
    case vr::VRInputError_InvalidHandle:        return "Invalid handle";
    case vr::VRInputError_InvalidParam:         return "Invalid parameter";
    case vr::VRInputError_MaxCapacityReached:   return "Max capacity reached";
    case vr::VRInputError_IPCError:             return "IPC communication error";
    case vr::VRInputError_NoActiveActionSet:    return "No active action set";
    case vr::VRInputError_InvalidDevice:        return "Invalid device";
    default:
        return "Unknown error: " + error;
    }
}

glm::mat4 Helper::ToConvert(const vr::HmdMatrix34_t& mat)
{
    glm::mat4 result = glm::mat4(1.0f);

    result[0][0] = mat.m[0][0];
    result[1][0] = mat.m[0][1];
    result[2][0] = mat.m[0][2];
    result[3][0] = mat.m[0][3];

    result[0][1] = mat.m[1][0];
    result[1][1] = mat.m[1][1];
    result[2][1] = mat.m[1][2];
    result[3][1] = mat.m[1][3];

    result[0][2] = mat.m[2][0];
    result[1][2] = mat.m[2][1];
    result[2][2] = mat.m[2][2];
    result[3][2] = mat.m[2][3];

    result[0][3] = 0.0f;
    result[1][3] = 0.0f;
    result[2][3] = 0.0f;
    result[3][3] = 1.0f;

    return result;
}

HBITMAP Helper::CreateHBITMAPFromSTBI(unsigned char* data, int srcW, int srcH, int destW, int destH) {
    BITMAPV5HEADER bi = { 0 };
    bi.bV5Size = sizeof(BITMAPV5HEADER);
    bi.bV5Width = destW;
    bi.bV5Height = -destH;  
    bi.bV5Planes = 1;
    bi.bV5BitCount = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask = 0x00FF0000;
    bi.bV5GreenMask = 0x0000FF00;
    bi.bV5BlueMask = 0x000000FF;
    bi.bV5AlphaMask = 0xFF000000;

    void* pixels = nullptr;
    HDC hdc = GetDC(NULL);
    HBITMAP hBitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &pixels, NULL, 0);
    ReleaseDC(NULL, hdc);

    if (!hBitmap) return NULL;

    for (int y = 0; y < destH; ++y) {
        for (int x = 0; x < destW; ++x) {
            int srcX = x * srcW / destW;
            int srcY = y * srcH / destH;
            int srcIndex = (srcY * srcW + srcX) * 4;
            int destIndex = (y * destW + x) * 4;

            ((unsigned char*)pixels)[destIndex + 0] = data[srcIndex + 2];  // B
            ((unsigned char*)pixels)[destIndex + 1] = data[srcIndex + 1];  // G
            ((unsigned char*)pixels)[destIndex + 2] = data[srcIndex + 0];  // R
            ((unsigned char*)pixels)[destIndex + 3] = data[srcIndex + 3];  // A
        }
    }

    return hBitmap;
}

void Helper::DrawImageWithAlpha(HDC hDc, HWND hWnd, const char* path, int destW, int destH, int posX, int posY) {
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 4);

    if (!data) {
        MessageBox(hWnd, L"Failed to load image", L"Error", MB_ICONERROR);
        return;
    }

    HBITMAP hBmp = Helper::CreateHBITMAPFromSTBI(data, width, height, destW, destH);

    stbi_image_free(data);
;
    HDC memDC = CreateCompatibleDC(hDc);

    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, hBmp);

    BLENDFUNCTION bf = { 0 };
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;

    AlphaBlend(hDc, posX, posY, destW, destH, memDC, 0, 0, destW, destH, bf);

    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);
    DeleteObject(hBmp);
}