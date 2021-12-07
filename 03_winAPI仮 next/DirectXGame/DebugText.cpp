#include "DebugText.h"

void DebugText::Initialize(ID3D12Device* dev, int window_width, int window_height, UINT texnumber, const Sprite::SpriteCommon& spriteCommon) {
	// 全てのスプライトデータについて
	for (int i = 0; i < _countof(sprites); i++) {
		// スプライトを生成する
		sprites[i].SpriteCreate(dev, window_width, window_height, texnumber, spriteCommon, { 0,0 });
	}
}

void DebugText::Print(const Sprite::SpriteCommon& spriteCommon, const std::string& text, float x, float y, float scale) {
	// 全ての文字について
	for (int i = 0; i < text.size(); i++) {
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
		sprites[spriteIndex].SpriteUpdate(spriteCommon);

		// 文字を１つ進める
		spriteIndex++;
	}
}

// まとめて描画
void DebugText::DrawAll(ID3D12GraphicsCommandList* cmdList, const Sprite::SpriteCommon& spriteCommon, ID3D12Device* dev) {
	// 全ての文字のスプライトについて
	for (int i = 0; i < spriteIndex; i++) {
		// スプライト描画
		sprites[i].SpriteDraw(cmdList, spriteCommon, dev);
	}

	spriteIndex = 0;
}