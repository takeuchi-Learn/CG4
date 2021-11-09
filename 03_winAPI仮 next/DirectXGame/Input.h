#pragma once

#include <Windows.h>

#include <wrl.h>	//ComPtr

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

class Input {
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
private:
	BYTE key[256];
	BYTE preKey[256];

	ComPtr<IDirectInputDevice8> devkeyboard;

	ComPtr<IDirectInput8> dinput;

public:
	Input(HINSTANCE hInstance, HWND hwnd);

	void init(HINSTANCE hInstance, HWND hwnd);
	void updateHitState();

	bool hitKey(BYTE keyCode);
	bool hitPreKey(BYTE keyCode);
	bool triggerKey(BYTE keyCode);
};

