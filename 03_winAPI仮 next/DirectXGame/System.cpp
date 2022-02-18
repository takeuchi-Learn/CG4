#include "System.h"

#include "WinAPI.h"
#include "DirectXCommon.h"

#include "Input.h"
#include "Looper.h"

System::System() {
	constexpr LPCSTR window_title = "DirectX�N���X��";
	WinAPI::getInstance()->setWindowText(window_title);
}

void System::update() {
	if (error == false) {
		// �Q�[�����[�v
		while (!WinAPI::getInstance()->processMessage()
			   && Looper::getInstance()->loop() == false) {

		}
	}
}

System::~System() {
}
