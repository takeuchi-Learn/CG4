#include "System.h"

#include "WinAPI.h"
#include "DirectXCommon.h"

#include "Input.h"
#include "Looper.h"

namespace {
	constexpr wchar_t* window_title = L"DirectX�N���X��";
}

System::System() {
	WinAPI::create(window_title);
	DirectXCommon::create(WinAPI::getInstance());
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
	DirectXCommon::destroy();
	WinAPI::destroy();
}
