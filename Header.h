#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include "Header.h"

#include <Windows.h>
#include <vector>
#include <functional>
#include <d3d11.h>
#include "lib/openvr.h"
#include <glm/glm.hpp>
#include <thread>


#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>     



#define WM_MAIN_THREAD_FUNCTION (WM_USER + 1)

#define ANGLE_ROTATION 900.f

class Interface {
public:
	std::vector<std::pair<HWND, std::function<void(HWND)>>>  hObjects;
	HWND hParent;
	int ID_NUM = 1000;
	int HEIGHT_POINT = 20;
	CONST int POS_SHIFT = 20;
	

	Interface(HWND hParent);
	int AddCheckBox(LPCWSTR title, std::function<void(HWND)> func = nullptr);
	void WM_HANDLER(WPARAM wParam);
	int AddText(LPCWSTR title, std::function<void(HWND)> func = nullptr);
	HWND GetObjectByID(int id);
	void DeleteObjectByID(int id);
	void DeleteAllObjects();
	void WM_RESIZE();
	void WM_PAINT_(HDC hDc);
	virtual ~Interface();

private:
	void UpdatePositions();
};



class vJoyIntegration {
public:
	vJoyIntegration(int ID, float MAX_ANGLE_ROTATION);
	virtual ~vJoyIntegration();
	bool Start();
	void PrintvJoyDevices();
	void Stop();
	void SetPosition(int ID_CONTROL, float value);
	void SetPosition(int ID_CONTROL, bool value);
	void CheckButtons(std::vector<int> btns);
	void JoyStickInput(int ID_CONTROL, float value);
	void JoyStickInput(int ID_CONTROL, bool value);
	bool JoyStickInputInit();
	void JoyStickInputRelease();

private:
	int ID_DEVICE;
	float MAX_ANGLE_ROTATION;
	bool isJoySticksInputInit;

};

class SteamVRIntegrationInput {
public:
	bool Init();
	void UpdateButtonsState(vJoyIntegration* vjoy);

	struct VRController {
		vr::InputPoseActionData_t pos;
		vr::InputDigitalActionData_t TriggerBtn;
		vr::InputAnalogActionData_t TriggerValue;
		vr::InputDigitalActionData_t GripBtn;
		vr::InputAnalogActionData_t GripValue;
		vr::InputDigitalActionData_t A;
		vr::InputDigitalActionData_t B;
		vr::InputDigitalActionData_t TrackPadBtn;
		vr::InputDigitalActionData_t TrackPadTouch;
		vr::InputAnalogActionData_t TrackPadPos;
		vr::InputDigitalActionData_t JoyStickBtn;
		vr::InputAnalogActionData_t JoyStickPos;
		vr::InputDigitalActionData_t MenuButton;
		vr::InputDigitalActionData_t SystemButton;
		vr::VRActionHandle_t HapticOut;
	};

	VRController leftController;
	VRController rightController;

private:
	struct VRControllerType {
		vr::VRInputValueHandle_t left;
		vr::VRInputValueHandle_t right;
	};

	struct VRControllerHandlers
	{
		VRControllerType SelectController;
		vr::VRActionHandle_t Pose;
		vr::VRActionHandle_t TriggerBtn;
		vr::VRActionHandle_t TriggerValue;
		vr::VRActionHandle_t GripBtn;
		vr::VRActionHandle_t GripValue;
		vr::VRActionHandle_t A;
		vr::VRActionHandle_t B;
		vr::VRActionHandle_t TrackPadBtn;
		vr::VRActionHandle_t TrackPadTouch;
		vr::VRActionHandle_t TrackPadPos;
		vr::VRActionHandle_t JoyStickBtn;
		vr::VRActionHandle_t JoyStickPos;
		vr::VRActionHandle_t MenuButton;
		vr::VRActionHandle_t SystemButton;
		vr::VRActionHandle_t HapticOut;

	};

	/*VRController rightController;
	VRController leftController;*/
	VRControllerHandlers vrControllerHandlers;
	vr::VRActiveActionSet_t actionSet;
	bool InitHandlers();
	vr::VRActiveActionSet_t CreateActionSet(vr::VRActionHandle_t actionHandle);

};

///////////////////////////////////////////////////////////////////////////////

class VRWheel {
public:
	bool err_init = false;
	

	VRWheel(HWND hMainWin, Interface* gui);
	virtual ~VRWheel();
	vJoyIntegration* vjoyintegration = nullptr;
	SteamVRIntegrationInput* steamInput = nullptr;
	
	void Start(const std::function<void()> _callback = nullptr);
	void Stop();
	bool Status();

private:
	
	vr::VROverlayHandle_t   g_overlayHandle = vr::k_ulOverlayHandleInvalid;
	vr::VROverlayHandle_t g_overlayLeftHandOpen = vr::k_ulOverlayHandleInvalid;
	vr::VROverlayHandle_t g_overlayLeftHandClose = vr::k_ulOverlayHandleInvalid;
	vr::VROverlayHandle_t g_overlayRightHandOpen = vr::k_ulOverlayHandleInvalid;
	vr::VROverlayHandle_t g_overlayRightHandClose = vr::k_ulOverlayHandleInvalid;

	

	std::thread mThread;
	bool isWork = true;

	bool isEditModeON = false;
	bool isHideHands = false;
	bool isHideWheel = false;
	bool isJoySticksEnable = false;
	bool isJoySticksFirstTurnOn = false;
	bool isWheelReturn = false;
	
	Interface* gui = nullptr;

	int ID_TEXT_POSITIONS;
	int ID_TEXT_LEFT_CONTROLLER;
	int ID_TEXT_RIGHT_CONTROLLER;

	int ID_TEXT_OVERLAY_POSITION;
	int ID_TEXT_STEERING_WHEEL_ROTATION;

	int ID_CHECKBOX_EDITMODE;
	int ID_CHECKBOX_HIDE_HANDS;
	int ID_CHECKBOX_HIDE_WHEEL;
	int ID_CHECKBOX_ENABLE_JOYSTICKS;
	int ID_CHECKBOX_RETURN_WHEEL;


	HWND hMainWin;

	glm::mat4 originalWheelTransform = glm::mat4(1.0f);
	glm::vec2 localControllerStart = glm::vec2(0.0f);
	
	float initialAngle_;
	float steeringRotation;

	vr::HmdMatrix34_t overlayPos_native;


	bool ConnectToSteamVROverlay();
	glm::mat4 GetPositionOverlay();
	void ControllersHandler(const glm::mat4 leftController, const glm::mat4 rightController, vr::HmdMatrix34_t& overlayPos, glm::mat4& initialControllerTransform, glm::mat4& initialWheelTransform, bool& isHold, float& initialAngle, float& currentRotation, float& steeringWheel, bool& isHoldLeft);
	void UpdateGUIInformation(const glm::mat4 leftController, const glm::mat4 rightController, const glm::mat4 overlay, const float wheelRotation);
	bool InitvJoyIntegration();
	void OverlaysUpdate();
	bool OverlaysInit();
	void SteeringWheelUpdate(float angle, vr::HmdMatrix34_t& overlayPos);
	void GUIInit();
	bool SavePositionWheel(glm::mat4 pos);
	glm::mat4 LoadPositionWheel();
	void FirstSetup();
};

class Helper {
public:
	static std::wstring ToConvert(const char* text);
	static std::wstring GetFolderPath();
	static vr::HmdMatrix34_t ToConvert(const glm::mat4& mat);
	static glm::mat4 ToConvert(const vr::HmdMatrix34_t& mat);
	static HWND hMainWin;
	static void MainThreadInvoke(std::function<void()>* fun);
	static void Debug(std::vector<std::string> str);
	static void Debug(std::vector<std::wstring> str);
	static void Debug(std::vector<int> str);
	static std::string ToConvert(std::wstring ws);
	static std::wstring ToConvert(std::string s);
	static const char* SteamVRInput_GetInputError(vr::EVRInputError error);
	static void UpdateState(int ID_ELEM);
	static void MainThreadInvokeSyn(std::function<void()>* fun);
	static HBITMAP CreateHBITMAPFromSTBI(unsigned char* data, int srcW, int srcH, int destW, int destH);
	static void DrawImageWithAlpha(HDC hDc, HWND hWnd, const char* path, int destW, int destH, int posX, int posY);

private:

};

class GUI {
public:
	GUI(HWND hMainWin);
	void WM_COMMAND_HANDLER(WPARAM wParam);
	void WM_RESIZE_HANDLER();
	void WM_PAINT_HANDLER(HDC hDc);
	
private:
	HWND hMainWin;
	Interface* gui = nullptr;
	VRWheel* vrwheel = nullptr;
	

	void Init();
};

