#include "Input.h"

#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Input::Input(HINSTANCE hInstance, HWND hwnd) {
	init(hInstance, hwnd);
}

void Input::init(HINSTANCE hInstance, HWND hwnd) {

	HRESULT result;

	result = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);

	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);

	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard); // •W€Œ`Ž®

	result = devkeyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
}

void Input::updateHitState() {
	HRESULT result;
	result = devkeyboard->Acquire();

	memcpy(preKey, key, sizeof(key));
	result = devkeyboard->GetDeviceState(sizeof(key), key);
}

bool Input::hitKey(BYTE keyCode) { return (bool)key[keyCode]; }
bool Input::hitPreKey(BYTE keyCode) { return (bool)preKey[keyCode]; }

bool Input::triggerKey(BYTE keyCode) { return (bool)(key[keyCode] && preKey[keyCode] == false); }
