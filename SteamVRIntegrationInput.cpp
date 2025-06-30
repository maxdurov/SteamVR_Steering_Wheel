#include "Header.h"
#include "lib/public.h"


bool SteamVRIntegrationInput::Init() {

    std::string pathway = Helper::ToConvert(Helper::GetFolderPath());
    pathway.append("\\manifest.json");

    vr::EVRInputError err = vr::VRInput()->SetActionManifestPath(pathway.c_str());
    if (err != vr::VRInputError_None) {
        const char* _err = Helper::SteamVRInput_GetInputError(err);

        MessageBox(NULL, Helper::ToConvert(_err).c_str(), L"ERROR", MB_OK);

        return false;
    }

    if (!this->InitHandlers()) {
        MessageBox(NULL, L"Failed to init handlers of vr controllers", L"ERROR", MB_OK);
        return false;
    }


    return true;
}

bool SteamVRIntegrationInput::InitHandlers() {

    if (vr::VRInput()->GetInputSourceHandle("/user/hand/right", &this->vrControllerHandlers.SelectController.right) != vr::VRInputError_None) return false;
    if (vr::VRInput()->GetInputSourceHandle("/user/hand/left", &this->vrControllerHandlers.SelectController.left) != vr::VRInputError_None) return false;
    if (vr::VRInput()->GetActionHandle("/actions/main/in/pose", &this->vrControllerHandlers.Pose) != vr::VRInputError_None) return false;
    if(vr::VRInput()->GetActionHandle("/actions/main/in/triggerclick", &this->vrControllerHandlers.TriggerBtn) != vr::VRInputError_None) return false;
    if(vr::VRInput()->GetActionHandle("/actions/main/in/triggervalue", &this->vrControllerHandlers.TriggerValue) != vr::VRInputError_None) return false;
    if(vr::VRInput()->GetActionHandle("/actions/main/in/gripclick", &this->vrControllerHandlers.GripBtn) != vr::VRInputError_None) return false;
    if(vr::VRInput()->GetActionHandle("/actions/main/in/gripvalue", &this->vrControllerHandlers.GripValue) != vr::VRInputError_None) return false;
    if (vr::VRInput()->GetActionHandle("/actions/main/in/trackpadclick", &this->vrControllerHandlers.TrackPadBtn) != vr::VRInputError_None) return false;
    if (vr::VRInput()->GetActionHandle("/actions/main/in/trackpadtouch", &this->vrControllerHandlers.TrackPadTouch) != vr::VRInputError_None) return false;
    if (vr::VRInput()->GetActionHandle("/actions/main/in/trackpadposition", &this->vrControllerHandlers.TrackPadPos) != vr::VRInputError_None) return false;
    if (vr::VRInput()->GetActionHandle("/actions/main/in/joystickclick", &this->vrControllerHandlers.JoyStickBtn) != vr::VRInputError_None) return false;
    if (vr::VRInput()->GetActionHandle("/actions/main/in/joystickposition", &this->vrControllerHandlers.JoyStickPos) != vr::VRInputError_None) return false;
    if (vr::VRInput()->GetActionHandle("/actions/main/in/buttona", &this->vrControllerHandlers.A) != vr::VRInputError_None) return false;
    if (vr::VRInput()->GetActionHandle("/actions/main/in/buttonb", &this->vrControllerHandlers.B) != vr::VRInputError_None) return false;
    if (vr::VRInput()->GetActionHandle("/actions/main/in/systembutton", &this->vrControllerHandlers.SystemButton) != vr::VRInputError_None) return false;
    if (vr::VRInput()->GetActionHandle("/actions/main/in/menubutton", &this->vrControllerHandlers.MenuButton) != vr::VRInputError_None) return false;
    if (vr::VRInput()->GetActionHandle("/actions/main/out/haptic", &this->vrControllerHandlers.HapticOut) != vr::VRInputError_None) return false;

    vr::VRActionSetHandle_t actionSet;
    if (vr::VRInput()->GetActionSetHandle("/actions/main", &actionSet) != vr::VRInputError_None) return false;

    this->actionSet = CreateActionSet(actionSet); 

    return true;
}

vr::VRActiveActionSet_t SteamVRIntegrationInput::CreateActionSet(const vr::VRActionHandle_t actionHandle) {

    vr::VRActiveActionSet_t actionSet = {};
    actionSet.ulActionSet = actionHandle;
    actionSet.ulRestrictedToDevice = vr::k_ulInvalidInputValueHandle;
    actionSet.ulSecondaryActionSet = vr::k_ulInvalidInputValueHandle;
    actionSet.nPriority = 0;

    return actionSet;
}


void SteamVRIntegrationInput::UpdateButtonsState(vJoyIntegration* vjoy) {


    //
    // Scheme of connection right controller to VJoy buttons
    // see: https://github.com/maxdurov/SteamVR_Steering_Wheel/blob/main/images/prevy_controller_right.png

    
    vr::VRInput()->UpdateActionState(&this->actionSet, sizeof(this->actionSet), 1);

    vr::InputDigitalActionData_t state = {};
    vr::InputAnalogActionData_t state_analog = {};
    vr::InputPoseActionData_t pose = {};

    vr::VRInput()->GetPoseActionDataRelativeToNow(this->vrControllerHandlers.Pose, vr::TrackingUniverseStanding, 0, &pose, sizeof(pose), this->vrControllerHandlers.SelectController.right);
    this->rightController.pos = pose;

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.TriggerBtn, &state, sizeof(state), this->vrControllerHandlers.SelectController.right);
    vjoy->SetPosition(0x0001, state.bState);
    this->rightController.TriggerBtn = state;

    vr::VRInput()->GetAnalogActionData(this->vrControllerHandlers.TriggerValue, &state_analog, sizeof(state_analog), this->vrControllerHandlers.SelectController.right);
    vjoy->SetPosition(HID_USAGE_Y, state_analog.x);
    this->rightController.TriggerValue = state_analog;

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.GripBtn, &state, sizeof(state), this->vrControllerHandlers.SelectController.right);
    vjoy->SetPosition(0x0002, state.bState);
    this->rightController.GripBtn = state;

    vr::VRInput()->GetAnalogActionData(this->vrControllerHandlers.GripValue, &state_analog, sizeof(state_analog), this->vrControllerHandlers.SelectController.right);
    vjoy->SetPosition(HID_USAGE_RY, state_analog.x);
    this->rightController.GripValue = state_analog;

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.TrackPadBtn, &state, sizeof(state), this->vrControllerHandlers.SelectController.right);
    vjoy->SetPosition(0x0003, state.bState);
    this->rightController.TrackPadBtn = state;

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.TrackPadTouch, &state, sizeof(state), this->vrControllerHandlers.SelectController.right);
    this->rightController.TrackPadTouch = state;

    vr::VRInput()->GetAnalogActionData(this->vrControllerHandlers.TrackPadPos, &state_analog, sizeof(state_analog), this->vrControllerHandlers.SelectController.right);
    this->rightController.TrackPadPos = state_analog; // NO USE

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.JoyStickBtn, &state, sizeof(state), this->vrControllerHandlers.SelectController.right);
    vjoy->SetPosition(0x0004, state.bState);
    this->rightController.JoyStickBtn = state;

    vr::VRInput()->GetAnalogActionData(this->vrControllerHandlers.JoyStickPos, &state_analog, sizeof(state_analog), this->vrControllerHandlers.SelectController.right);
    this->rightController.JoyStickPos = state_analog;
    vjoy->JoyStickInput(HID_USAGE_RX, state_analog.x);
    vjoy->JoyStickInput(HID_USAGE_RY, state_analog.y);

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.A, &state, sizeof(state), this->vrControllerHandlers.SelectController.right);
    vjoy->SetPosition(0x0005, state.bState);
    vjoy->JoyStickInput(0x0001, state.bState);
    this->rightController.A = state;

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.B, &state, sizeof(state), this->vrControllerHandlers.SelectController.right);
    vjoy->SetPosition(0x0006, state.bState);
    vjoy->JoyStickInput(0x0002, state.bState);
    this->rightController.B = state;

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.SystemButton, &state, sizeof(state), this->vrControllerHandlers.SelectController.right);
    vjoy->SetPosition(0x0007, state.bState);
    this->rightController.SystemButton = state;

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.MenuButton, &state, sizeof(state), this->vrControllerHandlers.SelectController.right);
    vjoy->SetPosition(0x0008, state.bState);
    this->rightController.MenuButton = state;

    //
    // Scheme of connection left controller to VJoy buttons
    // see: https://github.com/maxdurov/SteamVR_Steering_Wheel/blob/main/images/prevy_controller_left.png

    vr::VRInput()->GetPoseActionDataRelativeToNow(this->vrControllerHandlers.Pose, vr::TrackingUniverseStanding, 0, &pose, sizeof(pose), this->vrControllerHandlers.SelectController.left);
    this->leftController.pos = pose;

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.TriggerBtn, &state, sizeof(state), this->vrControllerHandlers.SelectController.left);
    vjoy->SetPosition(0x0000A, state.bState);
    this->leftController.TriggerBtn = state;

    vr::VRInput()->GetAnalogActionData(this->vrControllerHandlers.TriggerValue, &state_analog, sizeof(state_analog), this->vrControllerHandlers.SelectController.left);
    vjoy->SetPosition(HID_USAGE_Z, state_analog.x);
    this->leftController.TriggerValue = state_analog;

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.GripBtn, &state, sizeof(state), this->vrControllerHandlers.SelectController.left);
    vjoy->SetPosition(0x0000B, state.bState);
    this->leftController.GripBtn = state;

    vr::VRInput()->GetAnalogActionData(this->vrControllerHandlers.GripValue, &state_analog, sizeof(state_analog), this->vrControllerHandlers.SelectController.left);
    vjoy->SetPosition(HID_USAGE_RX, state_analog.x);
    this->leftController.GripValue = state_analog;

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.TrackPadBtn, &state, sizeof(state), this->vrControllerHandlers.SelectController.left);
    vjoy->SetPosition(0x0000C, state.bState);
    this->leftController.TrackPadBtn = state;

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.TrackPadTouch, &state, sizeof(state), this->vrControllerHandlers.SelectController.left);
    this->leftController.TrackPadTouch = state;

    vr::VRInput()->GetAnalogActionData(this->vrControllerHandlers.TrackPadPos, &state_analog, sizeof(state_analog), this->vrControllerHandlers.SelectController.left);
    this->leftController.TrackPadPos = state_analog; // NO USE

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.JoyStickBtn, &state, sizeof(state), this->vrControllerHandlers.SelectController.left);
    vjoy->SetPosition(0x0000D, state.bState);
    this->leftController.JoyStickBtn = state;

    vr::VRInput()->GetAnalogActionData(this->vrControllerHandlers.JoyStickPos, &state_analog, sizeof(state_analog), this->vrControllerHandlers.SelectController.left);
    this->leftController.JoyStickPos = state_analog;
    vjoy->JoyStickInput(HID_USAGE_X, state_analog.x);
    vjoy->JoyStickInput(HID_USAGE_Y, state_analog.y);

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.A, &state, sizeof(state), this->vrControllerHandlers.SelectController.left);
    vjoy->SetPosition(0x0000E, state.bState);
    vjoy->JoyStickInput(0x00003, state.bState);
    this->leftController.A = state;

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.B, &state, sizeof(state), this->vrControllerHandlers.SelectController.left);
    vjoy->SetPosition(0x0000F, state.bState);
    vjoy->JoyStickInput(0x00004, state.bState);
    this->leftController.B = state;

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.SystemButton, &state, sizeof(state), this->vrControllerHandlers.SelectController.left);
    vjoy->SetPosition(0x00010, state.bState);
    this->leftController.SystemButton = state;

    vr::VRInput()->GetDigitalActionData(this->vrControllerHandlers.MenuButton, &state, sizeof(state), this->vrControllerHandlers.SelectController.left);
    vjoy->SetPosition(0x00011, state.bState);
    this->leftController.MenuButton = state;
}
