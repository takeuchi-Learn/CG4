#include "DebugText.h"

void DebugText::Initialize(ID3D12Device* dev, int window_width, int window_height, UINT texnumber, const Sprite::SpriteCommon& spriteCommon) {
	// 全てのスプライトデータについて
	for (int i = 0; i < _countof(sprites); i++) {
		// スプライトを生成する
		sprites[i].create(dev, window_width, window_height, texnumber, spriteCommon, { 0,0 });
	}
}

void DebugText::Print(const Sprite::SpriteCommon& spriteCommon, const std::string& text, const float x, const float y, const float scale) {
	// 全ての文字について
	for (UINT i = 0; i < text.size(); i++) {
		// 最大文字数超過
		if (spriteIndex >= maxCharCount) {
			break;
		}

		// 1文字取り出す(※ASCIIコードでしか成り立たない)
		const unsigned char& character = text[i];

		// ASCIIコードの2段分飛ばした番号を計算
		int fontIndex = character - 32;
		if (character >= 0x7f) {
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		// 座標計算
		sprites[spriteIndex].position = { x + fontWidth * scale * i, y, 0 };
		sprites[spriteIndex].texLeftTop = { (float)fontIndexX * fontWidth, (float)fontIndexY * fontHeight };
		sprites[spriteIndex].texSize = { fontWidth, fontHeight };
		sprites[spriteIndex].size = { fontWidth * scale, fontHeight * scale };
		// 頂点バッファ転送
		sprites[spriteIndex].SpriteTransferVertexBuffer(spriteCommon);
		// 更新
		sprites[spriteIndex].update(spriteCommon);

		// 文字を１つ進める
		spriteIndex++;
	}
}

int DebugText::formatPrint(const Sprite::SpriteCommon& spriteCommon, const float x, const float y, const float scale, const char* fmt, ...) {
	
	char outStrChar[maxCharCount]{};

	va_list args;

	va_start(args, fmt);
	const int ret = vsnprintf(outStrChar, maxCharCount - 1, fmt, args);

	Print(spriteCommon, outStrChar, x, y, scale);
	va_end(args);

	return ret;
}

// まとめて描画
void DebugText::DrawAll(DirectXCommon* dxCom, const Sprite::SpriteCommon& spriteCommon) {
	// 全ての文字のスプライトについて
	for (int i = 0; i < spriteIndex; i++) {
		// スプライト描画
		sprites[i].draw(dxCom->getCmdList(), spriteCommon, dxCom->getDev());
	}

	spriteIndex = 0;
}