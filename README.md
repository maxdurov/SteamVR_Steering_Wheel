# SteamVR Steering Wheel

![image](https://github.com/maxdurov/SteamVR_Steering_Wheel/blob/main/images/overview.gif?raw=true)

[Документация на русском](https://github.com/maxdurov/SteamVR_Steering_Wheel/blob/main/README_RU.md)

This project integrates a steering wheel into VR and allows you to play games like Euro Truck Simulator 2 (ETS2), American Truck Simulator (ATS), and others using it.

---

## Requirements

This program works with SteamVR and vJoy. You need to install the following:

- SteamVR  
- vJoy ([Windows 10 and newer](https://sourceforge.net/projects/vjoystick/files/Beta%202.x/2.1.9.1-160719/vJoySetup.exe/download), [Windows 7](https://sourceforge.net/projects/vjoystick/files/Beta%202.x/2.1.8.39-270518/vJoySetup.exe/download))  

Tested with Meta (Oculus) Quest 2.  

Tested on Windows 10 and 11. Correct operation on Windows 7 is **not guaranteed**.

---

## Features

- Play games using a VR steering wheel  
- Configure controller buttons for in-game actions  
- Option to hide the steering wheel or hands  
- Enable steering wheel auto-centering  
- Integrates most VR controller buttons  
- Emulates a gamepad (supports 4 buttons, 2 joysticks; requires enabling Device 2 in vJoy configuration)  

---

## How to Install

1. Install SteamVR and vJoy  
2. Download the latest [version](https://github.com/maxdurov/SteamVR_Steering_Wheel/releases)  
3. Extract the files to a folder  
4. Run `SteamVR Steering Wheel.exe`  
5. Connect your VR headset  
6. Enable `Activate steering wheel` and follow the setup instructions  

---

## Controller Layout

![image](https://github.com/maxdurov/SteamVR_Steering_Wheel/blob/main/images/prevy_controller_left.png?raw=true)  
![image](https://github.com/maxdurov/SteamVR_Steering_Wheel/blob/main/images/prevy_controller_right.png?raw=true)  

---

## Extended Description

The program is developed in C++ using the following libraries:  
[openvr_api (ValveSoftware)](https://github.com/ValveSoftware/openvr)  
[vJoyInterface](https://github.com/shauleiz/vJoy)  
[stb_image](https://github.com/nothings/stb)  

### Functions:

- **Activate Steering Wheel** – Enables integration with SteamVR. If you encounter issues, try restarting your PC, SteamVR, or run the program with administrator rights.  

- **Settings Mode** – Automatically enabled on first launch. Allows you to position your steering wheel as desired.  
  **Controls:**  
  - Right trigger – Move  
  - Both triggers – Adjust size and angle  

  After positioning, uncheck this option to save the steering wheel placement.  

- **Hide Hands** – Show or hide hands linked to the controllers.  

- **Hide Steering Wheel** – Show or hide the steering wheel model.  

- **Returning Steering Wheel** – Enable or disable auto-centering for the steering wheel.  

- **Enable Joysticks (Emulate Gamepad)** – Activates gamepad emulation with 4 buttons (X, Y, B, A) and both joysticks (X+Y and RX+RY).  
  **Important:** You must enable Device 2 in the vJoy configuration for this feature to work.  

Additionally, you can map any VR controller buttons or triggers within your game settings.  
