#pragma once

#include "ObjCommon.h"

class Sprite : ObjCommon {
private:
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT2 = DirectX::XMFLOAT2;

public:
	// ���_�f�[�^
	struct VertexPosUv {
		XMFLOAT3 pos; // xyz���W
		XMFLOAT2 uv;  // uv���W
	};

	// �e�N�X�`���̍ő喇��
	static const int spriteSRVCount = 512;

	// �X�v���C�g�̋��ʃf�[�^
	struct SpriteCommon {
		// �p�C�v���C���Z�b�g
		PipelineSet pipelineSet;
		// �ˉe�s��
		XMMATRIX matProjection{};
		// �e�N�X�`���p�f�X�N���v�^�q�[�v�̐���
		ComPtr<ID3D12DescriptorHeap> descHeap;
		// �e�N�X�`�����\�[�X�i�e�N�X�`���o�b�t�@�j�̔z��
		ComPtr<ID3D12Resource> texBuff[spriteSRVCount];
	};

	// --------------------
	// �X�v���C�g1�����̃f�[�^
	// --------------------
private:
	//���_�o�b�t�@;
	ComPtr<ID3D12Resource> vertBuff;
	//���_�o�b�t�@�r���[;
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
	// ��\��
	bool isInvisible = false;

public:
	// --------------------
	// ����
	// --------------------

	// �X�v���C�g�p�p�C�v���C������
	static PipelineSet SpriteCreateGraphicsPipeline(ID3D12Device* dev,
		const wchar_t* vsPath = L"Resources/Shaders/SpriteVS.hlsl",
		const wchar_t* psPath = L"Resources/Shaders/SpritePS.hlsl");

	// �X�v���C�g���ʃf�[�^����
	static SpriteCommon SpriteCommonCreate(ID3D12Device* dev, int window_width, int window_height);

	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	static void SpriteCommonLoadTexture(SpriteCommon& spriteCommon, UINT texnumber, const wchar_t* filename, ID3D12Device* dev);

	// �X�v���C�g���ʃO���t�B�b�N�R�}���h�̃Z�b�g
	static void SpriteCommonBeginDraw(const SpriteCommon& spriteCommon, ID3D12GraphicsCommandList* cmdList);

	// --------------------
	// ��
	// --------------------

	// �X�v���C�g�P�̒��_�o�b�t�@�̓]��
	void SpriteTransferVertexBuffer(const SpriteCommon& spriteCommon);

	// �X�v���C�g����
	void SpriteCreate(ID3D12Device* dev, int window_width, int window_height,
		UINT texNumber, const SpriteCommon& spriteCommon, XMFLOAT2 anchorpoint = { 0.5f,0.5f },
		bool isFlipX = false, bool isFlipY = false);

	// �X�v���C�g�P�̍X�V
	void SpriteUpdate(const SpriteCommon& spriteCommon);

	// �X�v���C�g�P�̕`��
	void SpriteDraw(ID3D12GraphicsCommandList* cmdList, const SpriteCommon& spriteCommon, ID3D12Device* dev);

	// �X�V�ƕ`��𓯎��ɍs��
	void Sprite::SpriteDrawWithUpdate(ID3D12GraphicsCommandList* cmdList, const SpriteCommon& spriteCommon, ID3D12Device* dev);
};

