#pragma once

#include <DirectXMath.h>
#include <wrl.h>

#include <d3d12.h>

class SpriteCommon {

private:
	using XMMATRIX = DirectX::XMMATRIX;
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	// �u�����h���[�h
	enum class BLEND_MODE : USHORT {
		ALPHA,	// ����������
		ADD,	// ���Z����
		SUB,	// ���Z����
		REVERSE	// ���]����
	};

	// �p�C�v���C���Z�b�g
	struct PipelineSet {
		// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
		ComPtr<ID3D12PipelineState> pipelinestate;
		// ���[�g�V�O�l�`��
		ComPtr<ID3D12RootSignature> rootsignature;
	};

public:
	// �e�N�X�`���̍ő喇��
	static const UINT spriteSRVCount = 512u;

	// �p�C�v���C���Z�b�g
	PipelineSet pipelineSet;
	// �ˉe�s��
	XMMATRIX matProjection{};
	// �e�N�X�`���p�f�X�N���v�^�q�[�v�̐���
	ComPtr<ID3D12DescriptorHeap> descHeap;
	// �e�N�X�`�����\�[�X�i�e�N�X�`���o�b�t�@�j�̔z��
	ComPtr<ID3D12Resource> texBuff[spriteSRVCount];



	// ���ɓǂݍ��񂾉摜(�e�N�X�`��)�̐�
	static UINT nowTexNum;

private:
	// �X�v���C�g�p�p�C�v���C������(createSpriteCommon���ŌĂяo���Ă���)
	static SpriteCommon::PipelineSet SpriteCreateGraphicsPipeline(ID3D12Device* dev,
																  const wchar_t* vsPath, const wchar_t* psPath,
																  BLEND_MODE blendMode);

public:
	SpriteCommon(BLEND_MODE blendMode = BLEND_MODE::ALPHA,
				 const wchar_t* vsPath = L"Resources/Shaders/SpriteVS.hlsl",
				 const wchar_t* psPath = L"Resources/Shaders/SpritePS.hlsl");

	// �X�v���C�g���ʃe�N�X�`���ǂݍ���
	UINT loadTexture(const wchar_t* filename);

	// �X�v���C�g���ʃO���t�B�b�N�R�}���h�̃Z�b�g
	void drawStart(ID3D12GraphicsCommandList* cmdList);
};

