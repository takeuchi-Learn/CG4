#pragma once

#include "Sprite.h"
#include <string>

#include <DirectXMath.h>

class DebugText {
public: // 定数の宣言    
	static const int maxCharCount = 256;    // 最大文字数
	// todo 画像サイズに合わせて変えたい(fontWidth, fontHeight)
	static const int fontWidth = 9 * 2;         // フォント画像内1文字分の横幅
	static const int fontHeight = 18 * 2;       // フォント画像内1文字分の縦幅
	static const int fontLineCount = 14;    // フォント画像内1行分の文字数

public:
	UINT tabSize = 4;	// 初期値はSPACE4つ分

	// メンバ関数
	void Initialize(ID3D12Device* dev, int window_width, int window_height, UINT texnumber, const Sprite::SpriteCommon& spriteCommon, UINT tabSIze = 4);

	// ￥n : X座標をして位置に戻し、Y座標を文字の高さ分加算する
	// ￥t : tabSize文字分右にずらす
	void Print(const Sprite::SpriteCommon& spriteCommon, const std::string& text,
			   const float x, const float y, const float scale = 1.0f,
			   DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1, 1, 1, 1));

	// 内部でvsnprintfを使用
	// @return vsnprintfの戻り値
	int formatPrint(const Sprite::SpriteCommon& spriteCommon, const float x, const float y, const float scale, DirectX::XMFLOAT4 color, const char* fmt, ...);

	void DrawAll(DirectXCommon* dxCom, const Sprite::SpriteCommon& spriteCommon);

private: // メンバ変数     
	// スプライトデータの配列
	Sprite sprites[maxCharCount];
	// スプライトデータ配列の添え字番号
	int spriteIndex = 0;
};