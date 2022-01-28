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

	BYTE mouseState[256]{};
	BYTE preMouseState[256]{};

	POINT mousePos{};

	ComPtr<IDirectInputDevice8> devkeyboard;
	ComPtr<IDirectInputDevice8> devmouse;

	ComPtr<IDirectInput8> dinput;

public:
	Input();

	void init();
	void update();

	bool hitKey(BYTE keyCode);
	bool hitPreKey(BYTE keyCode);
	bool triggerKey(BYTE keyCode);

	enum MOUSE : UINT {
		LEFT = VK_LBUTTON,
		RIGHT = VK_RBUTTON,
		WHEEL = VK_MBUTTON
	};

	// @param Input::MOUSE::なんとか
	bool hitMouseBotton(_In_ BYTE keyCode);
	bool hitPreMouseBotton(_In_ BYTE keyCode);
	bool triggerMouseBotton(_In_ BYTE keyCode);

	// @return POINT型(LONG x とLONG y のみの構造体)
	POINT getMousePos();
	bool setMousePos(int x, int y);

	void changeDispMouseCursorFlag(const  bool dispFlag);
};

