#pragma once

#include "SpriteCommon.h"

#include "DirectXCommon.h"

class Sprite {
private:
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMMATRIX = DirectX::XMMATRIX;
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData {
		XMFLOAT4 color; // �F (RGBA)
		XMMATRIX mat;   // �R�c�ϊ��s��
	};

	// ���_�f�[�^
	struct VertexPosUv {
		XMFLOAT3 pos; // xyz���W
		XMFLOAT2 uv;  // uv���W
	};

	// --------------------
	// �X�v���C�g1�����̃f�[�^
	// --------------------
protected:
	//���_�o�b�t�@;
	ComPtr<ID3D12Resource> vertBuff;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//�萔�o�b�t�@;
	ComPtr<ID3D12Resource> constBuff;
	// ���[���h�s��
	XMMATRIX matWorld;

public:
	// Z�����̉�]�p
	float rotation = 0.0f;
	// ���W
	XMFLOAT3 position = { 0,0,0 };
	// �F(RGBA)
	XMFLOAT4 color = { 1, 1, 1, 1 };
	// ��\��
	bool isInvisible = false;

private:
	// �e�N�X�`���ԍ�
	UINT texNumber = 0;
	// �傫��
	XMFLOAT2 size = { 100, 100 };
	// �A���J�[�|�C���g
	XMFLOAT2 anchorpoint = { 0.5f, 0.5f };
	// ���E���]
	bool isFlipX = false;
	// �㉺���]
	bool isFlipY = false;
	// �e�N�X�`��������W
	XMFLOAT2 texLeftTop = { 0, 0 };
	// �e�N�X�`���؂�o���T�C�Y
	XMFLOAT2 texSize = { 100, 100 };

	bool dirty = false;

public:
	// --------------------
	// �A�N�Z�b�T
	// --------------------
	inline void setAnchorPoint(const XMFLOAT2& ap) { anchorpoint = ap; dirty = true; }
	inline XMFLOAT2 getAnchorPoint() { return anchorpoint; }

	inline void setSize(const XMFLOAT2& size) { this->size = size; dirty = true; }
	inline XMFLOAT2 getSize() { return size; }

	inline void setTexLeftTop(const XMFLOAT2& texLT) { texLeftTop = texLT; dirty = true; }
	inline XMFLOAT2 getTexLeftTop() { return texLeftTop; }

	inline void setTexNum(const UINT& texNum) { texNumber = texNum; dirty = true; }
	inline UINT getTexNum() { return texNumber; }

	inline void setTexSize(const XMFLOAT2& texSize) { this->texSize = texSize; dirty = true; }
	inline XMFLOAT2 getTexSize() { return texSize; }

	inline void setFlipX(const bool isFlipX) { this->isFlipX = isFlipX; dirty = true; }
	inline bool getFlipX() { return isFlipX; }
	inline void setFlipY(const bool isFlipY) { this->isFlipY = isFlipY; dirty = true; }
	inline bool getFlipY() { return isFlipY; }

public:
	// --------------------
	// ��
	// --------------------
protected:
	// �X�v���C�g�P�̒��_�o�b�t�@�̓]��
	void SpriteTransferVertexBuffer(const SpriteCommon* spriteCommon);


	// �X�v���C�g����
	void create(ID3D12Device* dev, int window_width, int window_height,
				UINT texNumber, const SpriteCommon* spriteCommon, XMFLOAT2 anchorpoint = { 0.5f,0.5f },
				bool isFlipX = false, bool isFlipY = false);

public:
	// �������Ȃ�
	Sprite() {};
	// �������L��(create�֐��̌Ăяo��)
	Sprite(UINT texNumber,
		   const SpriteCommon* spriteCommon,
		   XMFLOAT2 anchorpoint = { 0.5f,0.5f },
		   bool isFlipX = false, bool isFlipY = false);

	// �X�v���C�g�P�̍X�V
	void update(const SpriteCommon* spriteCommon);

	// �X�v���C�g�P�̕`��
	void draw(ID3D12GraphicsCommandList* cmdList, const SpriteCommon* spriteCommon, ID3D12Device* dev);

	// �X�V�ƕ`��𓯎��ɍs��
	void drawWithUpdate(DirectXCommon* dxCom, const SpriteCommon* spriteCommon);
};