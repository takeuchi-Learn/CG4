#pragma once

#include "Sprite.h"
#include <string>

class DebugText {
public: // 定数の宣言    
	static const int maxCharCount = 256;    // 最大文字数
	// todo 画像サイズに合わせて変えたい(fontWidth, fontHeight)
	static const int fontWidth = 9 * 2;         // フォント画像内1文字分の横幅
	static const int fontHeight = 18 * 2;       // フォント画像内1文字分の縦幅
	static const int fontLineCount = 14;    // フォント画像内1行分の文字数

public: // メンバ関数
	void Initialize(ID3D12Device* dev, int window_width, int window_height, UINT texnumber, const Sprite::SpriteCommon& spriteCommon);

	void Print(const Sprite::SpriteCommon& spriteCommon, const std::string& text, const float x, const float y, const float scale = 1.0f);

	// 内部でvsnprintfを使用
	// @return vsnprintfの戻り値
	int formatPrint(const Sprite::SpriteCommon& spriteCommon, const float x, const float y, const float scale, const char* fmt, ...);

	void DrawAll(DirectXCommon* dxCom, const Sprite::SpriteCommon& spriteCommon);

private: // メンバ変数     
	// スプライトデータの配列
	Sprite sprites[maxCharCount];
	// スプライトデータ配列の添え字番号
	int spriteIndex = 0;
};