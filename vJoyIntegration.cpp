#include "Header.h"
#include "lib/public.h"
#include "lib/vjoyinterface.h"
#include <algorithm>

#pragma comment(lib, "vJoyInterface.lib")


vJoyIntegration::vJoyIntegration(int ID, float MAX_ANGLE_ROTATION) {
    this->ID_DEVICE = ID;
    this->MAX_ANGLE_ROTATION = MAX_ANGLE_ROTATION;
    this->isJoySticksInputInit = false;
}

bool vJoyIntegration::Start() {
    bool isWork = vJoyEnabled();
    bool reserve = AcquireVJD(this->ID_DEVICE);

    if (reserve) {
        ResetVJD(this->ID_DEVICE);
        return true;
    }
    return isWork && reserve;
}

void vJoyIntegration::CheckButtons(std::vector<int> btns) {
    int i = 0x0001;
    while (true) {
            this->SetPosition(i, true);
            Sleep(1000);
            this->SetPosition(i, false);
            i++;
            if (i > 10) i = 0;
    }
    

}

void vJoyIntegration::SetPosition(int ID_CONTROL, float value) {


    if (ID_CONTROL == HID_USAGE_X) {
        float angle = value;
        float maxAngle = this->MAX_ANGLE_ROTATION;

        
        LONG min = 0;
        LONG max = 32767; // or 65535

       
        float normalized = glm::clamp(angle / maxAngle, -1.0f, 1.0f);

        
        LONG axisValue = (LONG)(((normalized + 1.0f) / 2.0f) * (max - min) + min);

        axisValue = std::clamp(axisValue, min, max);
        //Helper::Debug({"SEND VALUE TO VJOY: ", std::to_string(axisValue)});
        SetAxis(axisValue, this->ID_DEVICE, ID_CONTROL);
        return;
    }

    if (ID_CONTROL == HID_USAGE_RZ || ID_CONTROL == HID_USAGE_Z || ID_CONTROL == HID_USAGE_Y || ID_CONTROL == HID_USAGE_RY || ID_CONTROL == HID_USAGE_RX){
        LONG min = 0;
        LONG max = 32767; // or 65535


        float normalized = std::clamp(value, 0.0f, 1.0f);


        LONG axisValue = (LONG)(normalized * (max - min) + min);

        axisValue = std::clamp(axisValue, min, max);
        //Helper::Debug({ "SEND VALUE TO VJOY: ", std::to_string(axisValue) });
        SetAxis(axisValue, this->ID_DEVICE, ID_CONTROL);
        return;
    }

    SetAxis(value, this->ID_DEVICE, ID_CONTROL);
    return;
}

bool vJoyIntegration::JoyStickInputInit() {
    bool reserve = AcquireVJD(2);

    if (reserve) {
        this->isJoySticksInputInit = true;
        ResetVJD(this->ID_DEVICE);
        return true;
    }
    else {
        return false;
    }
}

void vJoyIntegration::JoyStickInputRelease() {
    if (this->isJoySticksInputInit) {
        RelinquishVJD(2);
        this->isJoySticksInputInit = false;
    }
}

void vJoyIntegration::JoyStickInput(int ID_CONTROL, float value) {

    if (!this->isJoySticksInputInit) {
        return;
    }

    if (ID_CONTROL == HID_USAGE_RX || ID_CONTROL == HID_USAGE_Z || ID_CONTROL == HID_USAGE_Y || ID_CONTROL == HID_USAGE_RY || ID_CONTROL == HID_USAGE_X) {
        LONG min = 0;
        LONG max = 32767; // or 65535


        float normalized = value;


        LONG axisValue = (LONG)(((normalized + 1.0f) / 2.0f) * (max - min) + min);

        axisValue = std::clamp(axisValue, min, max);
        //Helper::Debug({ "SEND VALUE TO VJOY: ", std::to_string(axisValue) });
        SetAxis(axisValue, 2, ID_CONTROL);
    }

}

void vJoyIntegration::JoyStickInput(int ID_CONTROL, bool value) {

    // A - 1
    // B - 2
    // X - 3
    // Y - 4

    if (!this->isJoySticksInputInit) {
        return;
    }

    
    SetBtn(value, 2, ID_CONTROL);

}


void vJoyIntegration::SetPosition(int ID_CONTROL, bool value) {
    SetBtn(value, this->ID_DEVICE, ID_CONTROL);
}

void vJoyIntegration::Stop() {
    RelinquishVJD(this->ID_DEVICE);
    this->ID_DEVICE = -1;
}

vJoyIntegration::~vJoyIntegration() {
    if(this->ID_DEVICE != -1) RelinquishVJD(this->ID_DEVICE);
    if (this->isJoySticksInputInit) RelinquishVJD(2);
}

void vJoyIntegration::PrintvJoyDevices()
{
    for (UINT i = 1; i <= 16; ++i)
    {
        VjdStat status = GetVJDStatus(i);
        std::string str;
        switch (status) {
        case VJD_STAT_OWN:  str = "OWNED"; break;
        case VJD_STAT_FREE: str = "FREE"; break;
        case VJD_STAT_BUSY: str = "BUSY"; break;
        case VJD_STAT_MISS: str = "MISSING"; break;
        default: str = "UNKNOWN"; break;
        }
        printf("vJoy device %u: %s\n", i, str.c_str());
    }
    return;
}