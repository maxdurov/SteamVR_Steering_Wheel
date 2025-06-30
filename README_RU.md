# SteamVR Steering Wheel

![image](https://github.com/maxdurov/SteamVR_Steering_Wheel/blob/main/images/overview.gif?raw=true)

[English documentation](https://github.com/maxdurov/SteamVR_Steering_Wheel/blob/main/README.md)

Этот проект позволяет использовать рулевое колесо в VR и играть с его помощью в такие игры, как Euro Truck Simulator 2 (ETS2), American Truck Simulator (ATS) и другие.

---

## Требования

Для работы программы необходимо установить:

- SteamVR  
- vJoy ([Windows 10 и новее](https://sourceforge.net/projects/vjoystick/files/Beta%202.x/2.1.9.1-160719/vJoySetup.exe/download), [Windows 7](https://sourceforge.net/projects/vjoystick/files/Beta%202.x/2.1.8.39-270518/vJoySetup.exe/download))  

Протестировано на Meta (Oculus) Quest 2.  
Программа работает на Windows 10 и 11. Корректная работа на Windows 7 **не гарантируется**.

---

## Возможности

- Игра с рулевым колесом в VR  
- Настройка кнопок контроллеров для игровых действий  
- Возможность скрывать руль или руки  
- Автоматический возврат руля в центр  
- Интеграция большинства кнопок VR-контроллеров  
- Эмуляция геймпада (поддерживаются 4 кнопки, 2 стика; требуется активация устройства 2 в настройках vJoy)  

---

## Установка

1. Установите SteamVR и vJoy  
2. Скачайте последнюю [версию](https://github.com/maxdurov/SteamVR_Steering_Wheel/releases)  
3. Распакуйте файлы в удобное место  
4. Запустите `SteamVR Steering Wheel.exe`  
5. Подключите VR-шлем  
6. Активируйте опцию `Activate steering wheel` и следуйте инструкциям по настройке  

---

## Схема контроллеров

![image](https://github.com/maxdurov/SteamVR_Steering_Wheel/blob/main/images/prevy_controller_left.png?raw=true)  
![image](https://github.com/maxdurov/SteamVR_Steering_Wheel/blob/main/images/prevy_controller_right.png?raw=true)  

---

## Расширенное описание

Программа разработана на C++ с использованием следующих библиотек:  
[openvr_api (ValveSoftware)](https://github.com/ValveSoftware/openvr)  
[vJoyInterface](https://github.com/shauleiz/vJoy)  
[stb_image](https://github.com/nothings/stb)  

### Описание функций:

- **Activate Steering Wheel** – Включает интеграцию с SteamVR. Если возникнут проблемы, перезапустите ПК, SteamVR или попробуйте запустить программу от имени администратора.  

- **Settings Mode** – Включается автоматически при первом запуске и позволяет настроить положение рулевого колеса.  
  **Управление:**  
  - Правый триггер – перемещение  
  - Оба триггера – изменение размера и угла наклона  

  После настройки необходимо отключить эту опцию, чтобы сохранить выбранное положение рулевого колеса.  

- **Hide Hands** – Скрывает или отображает виртуальные руки, привязанные к контроллерам.  

- **Hide Steering Wheel** – Скрывает или отображает модель рулевого колеса.  

- **Returning Steering Wheel** – Включает или отключает автоматический возврат руля в центральное положение.  

- **Enable Joysticks (Эмуляция геймпада)** – Включает эмуляцию геймпада с поддержкой 4 кнопок (X, Y, B, A) и двух стиков (X+Y и RX+RY).  
  **Важно:** Для работы этой функции необходимо включить устройство 2 в настройках vJoy.  

Также вы можете назначать любые кнопки и триггеры контроллеров непосредственно в настройках игры.  
