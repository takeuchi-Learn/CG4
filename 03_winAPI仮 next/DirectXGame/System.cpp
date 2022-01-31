#include "System.h"

#include "WinAPI.h"
#include "DirectXCommon.h"

#include "Input.h"
#include "Looper.h"



#include <memory>

#include "Sound.h"

#include "Sprite.h"

#include "DebugText.h"

#include "Object3d.h"

#include "Collision.h"

#include "Time.h"

#include <sstream>
#include <iomanip>

#include <DirectXMath.h>
using namespace DirectX;

namespace {
	constexpr wchar_t* window_title = L"DirectXクラス化";

}

System::System() {
	WinAPI::create(window_title);
	DirectXCommon::create(WinAPI::getInstance());


}

void System::update() {
	if (error == false) {
		// ゲームループ
		while (!WinAPI::getInstance()->processMessage()
			   && Looper::getInstance()->loop() == false) {

		}
	}
}

System::~System() {
	DirectXCommon::destroy();
	WinAPI::destroy();
}
