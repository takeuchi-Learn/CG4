#include "System.h"

#include "WinAPI.h"
#include "DirectXCommon.h"

#include "Input.h"
#include "Looper.h"

#include "Object3d.h"

#include "FbxLoader.h"

System::System() {
	constexpr LPCSTR window_title = "DirectXクラス化";
	WinAPI::getInstance()->setWindowText(window_title);

	Object3d::staticInit(DirectXCommon::getInstance()->getDev());

	FbxLoader::GetInstance()->init(DirectXCommon::getInstance()->getDev());
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
	FbxLoader::GetInstance()->fin();
}
