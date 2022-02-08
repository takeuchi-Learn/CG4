#pragma once

#include "Sprite.h"
#include <string>

class DebugText {
public: // �萔�̐錾    
	static const int maxCharCount = 256;    // �ő啶����
	// todo �摜�T�C�Y�ɍ��킹�ĕς�����(fontWidth, fontHeight)
	static const int fontWidth = 9 * 2;         // �t�H���g�摜��1�������̉���
	static const int fontHeight = 18 * 2;       // �t�H���g�摜��1�������̏c��
	static const int fontLineCount = 14;    // �t�H���g�摜��1�s���̕�����

public: // �����o�֐�
	void Initialize(ID3D12Device* dev, int window_width, int window_height, UINT texnumber, const Sprite::SpriteCommon& spriteCommon);

	void Print(const Sprite::SpriteCommon& spriteCommon, const std::string& text, const float x, const float y, const float scale = 1.0f);

	// ������vsnprintf���g�p
	// @return vsnprintf�̖߂�l
	int formatPrint(const Sprite::SpriteCommon& spriteCommon, const float x, const float y, const float scale, const char* fmt, ...);

	void DrawAll(DirectXCommon* dxCom, const Sprite::SpriteCommon& spriteCommon);

private: // �����o�ϐ�     
	// �X�v���C�g�f�[�^�̔z��
	Sprite sprites[maxCharCount];
	// �X�v���C�g�f�[�^�z��̓Y�����ԍ�
	int spriteIndex = 0;
};