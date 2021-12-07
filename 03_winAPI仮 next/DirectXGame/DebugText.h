#pragma once

#include "Sprite.h"
#include <string>

class DebugText {
public: // �萔�̐錾    
	static const int maxCharCount = 256;    // �ő啶����
	static const int fontWidth = 9;         // �t�H���g�摜��1�������̉���
	static const int fontHeight = 18;       // �t�H���g�摜��1�������̏c��
	static const int fontLineCount = 14;    // �t�H���g�摜��1�s���̕�����

public: // �����o�֐�
	void Initialize(ID3D12Device* dev, int window_width, int window_height, UINT texnumber, const Sprite::SpriteCommon& spriteCommon);

	void Print(const Sprite::SpriteCommon& spriteCommon, const std::string& text, float x, float y, float scale = 1.0f);

	void DrawAll(ID3D12GraphicsCommandList* cmdList, const Sprite::SpriteCommon& spriteCommon, ID3D12Device* dev);

private: // �����o�ϐ�     
	// �X�v���C�g�f�[�^�̔z��
	Sprite sprites[maxCharCount];
	// �X�v���C�g�f�[�^�z��̓Y�����ԍ�
	int spriteIndex = 0;
};