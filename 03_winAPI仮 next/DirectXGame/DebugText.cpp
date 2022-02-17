#include "DebugText.h"

void DebugText::Initialize(ID3D12Device* dev, int window_width, int window_height, UINT texnumber, const Sprite::SpriteCommon& spriteCommon) {
	// �S�ẴX�v���C�g�f�[�^�ɂ���
	for (int i = 0; i < _countof(sprites); i++) {
		// �X�v���C�g�𐶐�����
		sprites[i].create(dev, window_width, window_height, texnumber, spriteCommon, { 0,0 });
	}
}

void DebugText::Print(const Sprite::SpriteCommon& spriteCommon, const std::string& text, const float x, const float y, const float scale) {
	// �S�Ă̕����ɂ���
	for (UINT i = 0; i < text.size(); i++) {
		// �ő啶��������
		if (spriteIndex >= maxCharCount) {
			break;
		}

		// 1�������o��(��ASCII�R�[�h�ł������藧���Ȃ�)
		const unsigned char& character = text[i];

		// ASCII�R�[�h��2�i����΂����ԍ����v�Z
		int fontIndex = character - 32;
		if (character >= 0x7f) {
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		// ���W�v�Z
		sprites[spriteIndex].position = { x + fontWidth * scale * i, y, 0 };
		sprites[spriteIndex].texLeftTop = { (float)fontIndexX * fontWidth, (float)fontIndexY * fontHeight };
		sprites[spriteIndex].texSize = { fontWidth, fontHeight };
		sprites[spriteIndex].size = { fontWidth * scale, fontHeight * scale };
		// ���_�o�b�t�@�]��
		sprites[spriteIndex].SpriteTransferVertexBuffer(spriteCommon);
		// �X�V
		sprites[spriteIndex].update(spriteCommon);

		// �������P�i�߂�
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

// �܂Ƃ߂ĕ`��
void DebugText::DrawAll(DirectXCommon* dxCom, const Sprite::SpriteCommon& spriteCommon) {
	// �S�Ă̕����̃X�v���C�g�ɂ���
	for (int i = 0; i < spriteIndex; i++) {
		// �X�v���C�g�`��
		sprites[i].draw(dxCom->getCmdList(), spriteCommon, dxCom->getDev());
	}

	spriteIndex = 0;
}