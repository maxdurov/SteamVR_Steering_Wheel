#define GLM_ENABLE_EXPERIMENTAL

#include "Header.h"
#include <Windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wincodec.h>
#include <iostream>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <functional>

#include <fstream>

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>       

#include "lib/openvr.h"
#include "lib/public.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "openvr_api.lib")
#pragma comment(lib, "dxgi.lib")





VRWheel::VRWheel(HWND hMainWin, Interface* gui) {

    if (!this->ConnectToSteamVROverlay()) {
        this->err_init = true;
        return;
    }

    if (!this->OverlaysInit()) {
        this->err_init = true;
        return;
    }

    if (!this->InitvJoyIntegration()) {
        this->err_init = true;
        return;
    }

    this->steamInput = new SteamVRIntegrationInput();

    if (!this->steamInput->Init()) {
        this->err_init = true;
        return;
    }

    this->gui = gui;
    this->hMainWin = hMainWin;

    return;
}

VRWheel::~VRWheel() {
    
    if(this->ID_TEXT_LEFT_CONTROLLER) this->gui->DeleteObjectByID(this->ID_TEXT_LEFT_CONTROLLER);
    if(this->ID_TEXT_RIGHT_CONTROLLER) this->gui->DeleteObjectByID(this->ID_TEXT_RIGHT_CONTROLLER);
    if(this->ID_TEXT_OVERLAY_POSITION) this->gui->DeleteObjectByID(this->ID_TEXT_OVERLAY_POSITION);
    if(this->ID_TEXT_POSITIONS) this->gui->DeleteObjectByID(this->ID_TEXT_POSITIONS);
    if(this->ID_TEXT_STEERING_WHEEL_ROTATION) this->gui->DeleteObjectByID(this->ID_TEXT_STEERING_WHEEL_ROTATION);
    if (this->ID_CHECKBOX_EDITMODE) this->gui->DeleteObjectByID(this->ID_CHECKBOX_EDITMODE);
    if (this->ID_CHECKBOX_HIDE_HANDS) this->gui->DeleteObjectByID(this->ID_CHECKBOX_HIDE_HANDS);
    if (this->ID_CHECKBOX_HIDE_WHEEL) this->gui->DeleteObjectByID(this->ID_CHECKBOX_HIDE_WHEEL);
    if (this->ID_CHECKBOX_ENABLE_JOYSTICKS) this->gui->DeleteObjectByID(this->ID_CHECKBOX_ENABLE_JOYSTICKS);
    if (this->ID_CHECKBOX_RETURN_WHEEL) this->gui->DeleteObjectByID(this->ID_CHECKBOX_RETURN_WHEEL);

    if (this->vjoyintegration) delete this->vjoyintegration;

    if (this->steamInput) delete this->steamInput;


    vr::VR_Shutdown();
}


bool VRWheel::ConnectToSteamVROverlay() {
    vr::EVRInitError initError = vr::VRInitError_None;
    vr::VR_Init(&initError, vr::VRApplication_Overlay);
    if (initError != vr::VRInitError_None) {
        const char* err = vr::VR_GetVRInitErrorAsEnglishDescription(initError);

        MessageBox(NULL, Helper::ToConvert(err).c_str(), L"ERROR", MB_OK);

        return false;
    }
    return true;
}

bool VRWheel::InitvJoyIntegration() {

    this->vjoyintegration = new vJoyIntegration(1, ANGLE_ROTATION);

    if (!this->vjoyintegration->Start()) {
    
        MessageBox(NULL, L"Failed to init vJoyIntegration", L"ERROR", MB_OK);
        return false;
    };
    return true;
}


void VRWheel::GUIInit() {
    
        ID_TEXT_POSITIONS = gui->AddText(L"POSITIONS:");
        ID_TEXT_LEFT_CONTROLLER = gui->AddText(L"LEFT CONTROLLER: NULL");
        ID_TEXT_RIGHT_CONTROLLER = gui->AddText(L"RIGHT CONTROLLER: NULL");
        ID_TEXT_STEERING_WHEEL_ROTATION = gui->AddText(L"STEERING WHEEL: NULL degrees");
        ID_TEXT_OVERLAY_POSITION = gui->AddText(L"OVERLAY: NULL");

        ID_CHECKBOX_EDITMODE = gui->AddCheckBox(L"Settings mode", std::function<void(HWND)>([&](HWND hwnd) {
            
            if (Button_GetCheck(hwnd)) {
                this->isEditModeON = true;
            }
            else {
                this->SavePositionWheel(Helper::ToConvert(this->overlayPos_native));
                this->originalWheelTransform = Helper::ToConvert(this->overlayPos_native);
                this->isEditModeON = false;
            }

            }));

        ID_CHECKBOX_HIDE_HANDS = gui->AddCheckBox(L"Hide hands", std::function<void(HWND)>([&](HWND hwnd) {

            if (Button_GetCheck(hwnd)) {
                this->isHideHands = true;
            }
            else {
                this->isHideHands = false;
            }

            }));

        ID_CHECKBOX_HIDE_WHEEL = gui->AddCheckBox(L"Hide steering wheel", std::function<void(HWND)>([&](HWND hwnd) {

            if (Button_GetCheck(hwnd)) {
                this->isHideWheel = true;
            }
            else {
                this->isHideWheel = false;

            }
            }));

        ID_CHECKBOX_RETURN_WHEEL = gui->AddCheckBox(L"Returning steering wheel", std::function<void(HWND)>([&](HWND hwnd) {

            if (Button_GetCheck(hwnd)) {
                this->isWheelReturn = true;
            }
            else {
                this->isWheelReturn = false;

            }
            }));

        ID_CHECKBOX_ENABLE_JOYSTICKS = gui->AddCheckBox(L"Enable joysticks (emulate GAMEPAD)", std::function<void(HWND)>([&](HWND obj) {

            if (Button_GetCheck(obj)) {
                if (this->isJoySticksFirstTurnOn == false) {
                    isJoySticksFirstTurnOn = true;
                    if (this->vjoyintegration->JoyStickInputInit()) {
                        return;
                    }
                    else {
                        MessageBox(this->hMainWin, L"Before activating this option, you need to enable Device 2 in \"Configure VJoy\"", L"Activate joysticks", MB_OK);
                        Button_SetCheck(obj, false);
                        return;
                    }
                }

                if (!this->vjoyintegration->JoyStickInputInit()) {
                    MessageBox(this->hMainWin, L"Error activating joysticks. Device 2 is unavailable. Please enable Device 2 in \"Configure VJoy\"", L"Activate joysticks", MB_OK);
                    Button_SetCheck(obj, false);
                    return;
                }




            }
            else {
                this->vjoyintegration->JoyStickInputRelease();
            }

            }));

}

void VRWheel::FirstSetup() {

    if (std::filesystem::exists("wheelposition")) {
        return;
    }

    MessageBox(this->hMainWin, L"Please set up the steering wheel position using your VR controllers and side triggers in your game.\nWhen you're done, exit Settings mode by unchecking the \"Settings mode\".", L"INFO", MB_OK);
    HWND editmode = gui->GetObjectByID(ID_CHECKBOX_EDITMODE);
    Button_SetCheck(editmode, true);
    Helper::UpdateState(ID_CHECKBOX_EDITMODE);
}


void VRWheel::Start(const std::function<void()> _callback) {

    this->GUIInit();
    this->FirstSetup();



    this->mThread = std::thread([this, _callback]() {

        glm::mat4 rightControllerPos;
        glm::mat4 leftControllerPos;
        glm::mat4 overlayPos;

        this->originalWheelTransform = this->LoadPositionWheel();
        this->overlayPos_native = Helper::ToConvert(this->originalWheelTransform);
        
       

        glm::mat4 initialControllerTransform = 0;
        glm::mat4 initialWheelTransform = 0;
        bool isHold = false;
        bool isHoldLeft = false;
        float initialAngle = 0.0f;    
        float currentRotation = 0.0f;

        float steeringWheel = 0.0f;
        this->vjoyintegration->SetPosition(HID_USAGE_X, glm::degrees(steeringWheel));

       

        if (vr::VR_IsHmdPresent()) {
            vr::VROverlay()->ShowOverlay(this->g_overlayHandle);
        }

        

        while (this->isWork && vr::VR_IsHmdPresent()) {
            
            this->steamInput->UpdateButtonsState(this->vjoyintegration);
            
                
                leftControllerPos = Helper::ToConvert(this->steamInput->leftController.pos.pose.mDeviceToAbsoluteTracking);
                rightControllerPos = Helper::ToConvert(this->steamInput->rightController.pos.pose.mDeviceToAbsoluteTracking);
                overlayPos = this->GetPositionOverlay();

                if (this->isEditModeON) {
                
                    if (steeringWheel != 0.0f) {
                        steeringWheel = 0.0f;
                        this->SteeringWheelUpdate(-steeringWheel, overlayPos_native);
                        this->vjoyintegration->SetPosition(HID_USAGE_X, glm::degrees(steeringWheel));
                    }

                }

                if (!this->steamInput->leftController.GripBtn.bState && !this->steamInput->rightController.GripBtn.bState && !isEditModeON && this->isWheelReturn) {

                    if (steeringWheel != 0.0f) {

                        float speedReturn = 0.03f;

                        if (steeringWheel < 0.0f) {
                            steeringWheel += speedReturn;
                            this->SteeringWheelUpdate(steeringWheel, overlayPos_native);
                        }
                        else {
                            steeringWheel -= speedReturn;
                            this->SteeringWheelUpdate(steeringWheel, overlayPos_native);
                        }

                        if (steeringWheel < 0.1f && steeringWheel > -0.1f) {
                            
                            steeringWheel = 0.0f;
                            this->SteeringWheelUpdate(steeringWheel, overlayPos_native);
                        }

                        this->vjoyintegration->SetPosition(HID_USAGE_X, glm::degrees(steeringWheel));

                    }

                }

                if (this->isHideWheel && !this->isEditModeON) {
                    vr::VROverlay()->HideOverlay(this->g_overlayHandle);
                }
                else {
                    vr::VROverlay()->ShowOverlay(this->g_overlayHandle);
                }
                
                
                
                
                this->OverlaysUpdate();
                this->ControllersHandler(leftControllerPos, rightControllerPos, this->overlayPos_native, initialControllerTransform, initialWheelTransform, isHold, initialAngle, currentRotation, steeringWheel, isHoldLeft);
                
                vr::VROverlay()->SetOverlayTransformAbsolute(this->g_overlayHandle, vr::TrackingUniverseStanding, &overlayPos_native);
                
                this->UpdateGUIInformation(leftControllerPos, rightControllerPos, overlayPos, glm::degrees(steeringWheel));
            
                std:Sleep(5);
        }

        if (_callback) Helper::MainThreadInvoke(new std::function<void()>([_callback]() { _callback(); }));

        });

    



    return;

};


bool VRWheel::SavePositionWheel(glm::mat4 pos) {

    std::ofstream file("wheelposition", std::ios::out | std::ios::trunc);
    if (file.is_open()) {

        for (int i = 0; i < pos.length(); i++) {
            for (int b = 0; b < pos[i].length(); b++) {
                file << pos[i][b];
                file << "\n";
            }
        }
        file.close();
    }
    else {
        MessageBox(NULL, L"Error while saving positiom", L"Error", MB_OK);
        return false;
    }
}

glm::mat4 VRWheel::LoadPositionWheel() {

    std::ifstream file("wheelposition");

    glm::mat4 pos = glm::mat4(1.0f);

    if (file.good()) {
        if (file.is_open()) {
            std::string line;

            int i = 0;
            int b = 0;

            while (std::getline(file, line)) {
                pos[i][b] = std::stof(line);
                b++;
                if (b > 3) {
                    b = 0;
                    i++;
                    if (i > 3) {
                        break;
                    }
                }
            }
            file.close();
    }
    }
    else {
        //MessageBox(NULL, L"File don't exist. Please setup the steering wheel position in Settings mode", L"Error", MB_OK);
        pos = glm::mat4(1.0f);
    }

    return pos;
}

bool VRWheel::OverlaysInit() {

    std::string path = Helper::ToConvert(Helper::GetFolderPath());
    path.append("\\media");

    if (vr::VROverlay()->CreateOverlay("SteeringWheel.main", "PNG Overlay", &this->g_overlayHandle) != vr::VROverlayError_None) {
        MessageBox(NULL, L"Failed to create overlay 1", L"ERROR", MB_OK);
        return false;
    }

    if (vr::VROverlay()->CreateOverlay("SteeringWheel.hand.right.open", "PNG Overlay", &this->g_overlayRightHandOpen) != vr::VROverlayError_None) {
        MessageBox(NULL, L"Failed to create overlay 2", L"ERROR", MB_OK);
        return false;
    }
    if (vr::VROverlay()->CreateOverlay("SteeringWheel.hand.right.close", "PNG Overlay", &this->g_overlayRightHandClose) != vr::VROverlayError_None) {
        MessageBox(NULL, L"Failed to create overlay 3", L"ERROR", MB_OK);
        return false;
    }
    if (vr::VROverlay()->CreateOverlay("SteeringWheel.hand.left.open", "PNG Overlay", &this->g_overlayLeftHandOpen) != vr::VROverlayError_None) {
        MessageBox(NULL, L"Failed to create overlay 4", L"ERROR", MB_OK);
        return false;
    }
    if (vr::VROverlay()->CreateOverlay("SteeringWheel.hand.left.close", "PNG Overlay", &this->g_overlayLeftHandClose) != vr::VROverlayError_None) {
        MessageBox(NULL, L"Failed to create overlay 5", L"ERROR", MB_OK);
        return false;
    }

    vr::VROverlay()->SetOverlayFromFile(this->g_overlayHandle, (path + "\\image.png").c_str());
    vr::VROverlay()->SetOverlayFromFile(this->g_overlayRightHandOpen, (path + "\\h_r_o.png").c_str());
    vr::VROverlay()->SetOverlayFromFile(this->g_overlayRightHandClose, (path + "\\h_r_c.png").c_str());
    vr::VROverlay()->SetOverlayFromFile(this->g_overlayLeftHandOpen, (path + "\\h_l_o.png").c_str());
    vr::VROverlay()->SetOverlayFromFile(this->g_overlayLeftHandClose, (path + "\\h_l_c.png").c_str());

    glm::mat4 pos = glm::mat4(1.0f);

    pos = glm::rotate(pos, -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    pos = glm::scale(pos, glm::vec3(0.15f, 0.15f, 0.15f));

    vr::HmdMatrix34_t pos_ = Helper::ToConvert(pos);
    
    vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(this->g_overlayRightHandOpen, vr::TrackedControllerRole_RightHand, &pos_);
    vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(this->g_overlayRightHandClose, vr::TrackedControllerRole_RightHand, &pos_);
    vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(this->g_overlayLeftHandOpen, vr::TrackedControllerRole_LeftHand, &pos_);
    vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(this->g_overlayLeftHandClose, vr::TrackedControllerRole_LeftHand, &pos_);

    return true;

}

void VRWheel::OverlaysUpdate() {

    if (this->isHideHands) {

        vr::VROverlay()->HideOverlay(this->g_overlayRightHandOpen);
        vr::VROverlay()->HideOverlay(this->g_overlayRightHandClose);

        vr::VROverlay()->HideOverlay(this->g_overlayLeftHandOpen);
        vr::VROverlay()->HideOverlay(this->g_overlayLeftHandClose);

        return;
    }

    if (this->steamInput->rightController.GripBtn.bState) {
        vr::VROverlay()->HideOverlay(this->g_overlayRightHandOpen);
        vr::VROverlay()->ShowOverlay(this->g_overlayRightHandClose);
    }
    else {
        vr::VROverlay()->HideOverlay(this->g_overlayRightHandClose);
        vr::VROverlay()->ShowOverlay(this->g_overlayRightHandOpen);
    }

    if (this->steamInput->leftController.GripBtn.bState) {
        vr::VROverlay()->HideOverlay(this->g_overlayLeftHandOpen);
        vr::VROverlay()->ShowOverlay(this->g_overlayLeftHandClose);
    }
    else {
        vr::VROverlay()->HideOverlay(this->g_overlayLeftHandClose);
        vr::VROverlay()->ShowOverlay(this->g_overlayLeftHandOpen);
    }
}

void VRWheel::SteeringWheelUpdate(float angle, vr::HmdMatrix34_t& overlayPos) {

    glm::mat4 pos = this->originalWheelTransform;

    pos = glm::rotate(pos, angle, glm::vec3(0, 0, 1));

    overlayPos = Helper::ToConvert(pos);
}

void VRWheel::ControllersHandler(const glm::mat4 leftController, const glm::mat4 rightController, vr::HmdMatrix34_t& overlayPos, glm::mat4 &initialControllerTransform, glm::mat4 &initialWheelTransform, bool& isHold, float& initialAngle, float& currentRotation, float& steeringWheel, bool& isHoldLeft) {

    bool left_press = false;
    bool right_press = false;

    glm::mat4 pos = Helper::ToConvert(overlayPos);

 
    


    if (this->steamInput->rightController.pos.pose.bPoseIsValid) {
        
        right_press = this->steamInput->rightController.GripBtn.bState;

        if (right_press && this->isEditModeON) {
            pos[3] = rightController[3];
            overlayPos = Helper::ToConvert(pos);
        }

        if (right_press && !left_press && !this->isEditModeON) {
            if (!isHold) {
                isHold = true;
                initialWheelTransform = pos;

                
                glm::mat4 invWheel = glm::inverse(pos);
                glm::vec3 controllerLocal = glm::vec3(invWheel * rightController[3]);
                initialAngle = atan2(controllerLocal.y, controllerLocal.x);
                initialAngle_ = initialAngle;
                
                return;
            }
            
            
            
            glm::mat4 invWheel_ = glm::inverse(initialWheelTransform);
            glm::vec3 controllerLocal_ = glm::vec3(invWheel_ * rightController[3]);

            float angleNow_ = atan2(controllerLocal_.y, controllerLocal_.x);
            float delta_ = angleNow_ - initialAngle_;

            if (delta_ > glm::pi<float>()) delta_ -= glm::two_pi<float>();
            if (delta_ < -glm::pi<float>()) delta_ += glm::two_pi<float>();

            delta_ = glm::clamp(delta_, -0.15f, 0.15f);



            steeringWheel = steeringWheel + delta_;
            steeringWheel = glm::clamp(steeringWheel, -glm::radians(ANGLE_ROTATION), glm::radians(ANGLE_ROTATION));

            bool limitReached = glm::degrees(steeringWheel) >= ANGLE_ROTATION || glm::degrees(steeringWheel) <= -ANGLE_ROTATION;

            
            this->SteeringWheelUpdate(steeringWheel, overlayPos);
            


            initialAngle_ = angleNow_;
            

            this->vjoyintegration->SetPosition(HID_USAGE_X, glm::degrees(steeringWheel));
            this->steeringRotation = glm::degrees(steeringWheel);

        }
        else {
            isHold = false;
        }
        
    }

    if (this->steamInput->leftController.pos.pose.bPoseIsValid) {
        left_press = this->steamInput->leftController.GripBtn.bState;
        
        if (left_press && !this->isEditModeON) {
                if (!isHoldLeft) {
                    isHoldLeft = true;
                    initialWheelTransform = pos;


                    glm::mat4 invWheel = glm::inverse(pos);
                    glm::vec3 controllerLocal = glm::vec3(invWheel * leftController[3]);
                    initialAngle = atan2(controllerLocal.y, controllerLocal.x);
                    initialAngle_ = initialAngle;

                    return;
                }



                glm::mat4 invWheel_ = glm::inverse(initialWheelTransform);
                glm::vec3 controllerLocal_ = glm::vec3(invWheel_ * leftController[3]);

                float angleNow_ = atan2(controllerLocal_.y, controllerLocal_.x);
                float delta_ = angleNow_ - initialAngle_;

                if (delta_ > glm::pi<float>()) delta_ -= glm::two_pi<float>();
                if (delta_ < -glm::pi<float>()) delta_ += glm::two_pi<float>();

                delta_ = glm::clamp(delta_, -0.15f, 0.15f);

                steeringWheel = steeringWheel + delta_;
                steeringWheel = glm::clamp(steeringWheel, -glm::radians(ANGLE_ROTATION), glm::radians(ANGLE_ROTATION));

                bool limitReached = glm::degrees(steeringWheel) >= ANGLE_ROTATION || glm::degrees(steeringWheel) <= -ANGLE_ROTATION;


                this->SteeringWheelUpdate(steeringWheel, overlayPos);


                initialAngle_ = angleNow_;


                this->vjoyintegration->SetPosition(HID_USAGE_X, glm::degrees(steeringWheel));
                this->steeringRotation = glm::degrees(steeringWheel);

            }
            else {
                isHoldLeft = false;
            }
        
    }




    if (left_press && right_press && isEditModeON) {
        float currentDistance = glm::distance(leftController[3], rightController[3]);

        glm::vec3 center = (leftController[3] + rightController[3]) * 0.5f;

        glm::mat4 transform = glm::mat4(1.0f);

        transform = glm::translate(transform, center);

        transform = glm::scale(transform, glm::vec3(currentDistance));

        glm::mat3 rot = glm::mat3(rightController);

        glm::vec3 euler = glm::eulerAngles(glm::quat_cast(rot));


        transform = glm::rotate(transform, euler.x, glm::vec3(1, 0, 0));

        pos = transform;

        overlayPos = Helper::ToConvert(pos);

        
    }

    
}

glm::mat4 VRWheel::GetPositionOverlay() {
    vr::HmdMatrix34_t overlayPos = vr::HmdMatrix34_t();
    vr::ETrackingUniverseOrigin tracking = vr::TrackingUniverseStanding;
    vr::EVROverlayError err = vr::VROverlay()->GetOverlayTransformAbsolute(this->g_overlayHandle, &tracking, &overlayPos);

    if (err) {
        return glm::mat4(1.0f);
    }
    else {
        return Helper::ToConvert(overlayPos);
    }

}

void VRWheel::UpdateGUIInformation(const glm::mat4 leftController, const glm::mat4 rightController, const glm::mat4 overlay, const float wheelRotation) {
    
    HWND controller_l = this->gui->GetObjectByID(ID_TEXT_LEFT_CONTROLLER);
    std::wstring pos_l = L"LEFT CONTROLLER: ";
    pos_l.append(L"X: ");
    pos_l.append(std::to_wstring(leftController[3][0]));
    pos_l.append(L" Y: ");
    pos_l.append(std::to_wstring(leftController[3][1]));
    pos_l.append(L" Z: ");
    pos_l.append(std::to_wstring(leftController[3][2]));

    Helper::MainThreadInvoke(new std::function<void()>([=]() {Static_SetText(controller_l, pos_l.c_str());}));

    HWND controller_r = this->gui->GetObjectByID(ID_TEXT_RIGHT_CONTROLLER);
    std::wstring pos_r = L"RIGHT CONTROLLER: ";
    pos_r.append(L"X: ");
    pos_r.append(std::to_wstring(rightController[3][0]));
    pos_r.append(L" Y: ");
    pos_r.append(std::to_wstring(rightController[3][1]));
    pos_r.append(L" Z: ");
    pos_r.append(std::to_wstring(rightController[3][2]));

    Helper::MainThreadInvoke(new std::function<void()>([=]() {Static_SetText(controller_r, pos_r.c_str());}));

    HWND _overlay = this->gui->GetObjectByID(ID_TEXT_OVERLAY_POSITION);
    std::wstring pos_o = L"OVERLAY: ";
    pos_o.append(L"X: ");
    pos_o.append(std::to_wstring(overlay[3][0]));
    pos_o.append(L" Y: ");
    pos_o.append(std::to_wstring(overlay[3][1]));
    pos_o.append(L" Z: ");
    pos_o.append(std::to_wstring(overlay[3][2]));

    Helper::MainThreadInvoke(new std::function<void()>([=]() {Static_SetText(_overlay, pos_o.c_str());}));

    HWND _wheelRotation = this->gui->GetObjectByID(ID_TEXT_STEERING_WHEEL_ROTATION);
    std::wstring pos_w = L"STEERING WHEEL: ";
    pos_w.append(std::to_wstring(wheelRotation));
    pos_w.append(L" degrees");

    Helper::MainThreadInvoke(new std::function<void()>([=]() { Static_SetText(_wheelRotation, pos_w.c_str()); }));
}

void VRWheel::Stop() {
    this->isWork = false;
    if (mThread.joinable()) this->mThread.join();
    return;
};

bool VRWheel::Status() {
    return this->mThread.joinable();
}