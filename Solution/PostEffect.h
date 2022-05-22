#pragma once
#include "DirectXCommon.h"

class PostEffect {

public:
	// �����_�[�^�[�Q�b�g�̐� = ���̃N���X�̃e�N�X�`���o�b�t�@�̐�
	// �V�F�[�_�[�ɍ��킹��
	static const UINT renderTargetNum = 2;

private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// ���_�f�[�^
	struct VertexPosUv {
		DirectX::XMFLOAT3 pos; // xyz���W
		DirectX::XMFLOAT2 uv;  // uv���W
	};

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData {
		DirectX::XMFLOAT4 color; // �F (RGBA)
		DirectX::XMMATRIX mat;   // �R�c�ϊ��s��
	};

	// �p�C�v���C���Z�b�g
	struct PipelineSet {
		// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
		ComPtr<ID3D12PipelineState> pipelinestate;
		// ���[�g�V�O�l�`��
		ComPtr<ID3D12RootSignature> rootsignature;
	};


	//���_�o�b�t�@;
	ComPtr<ID3D12Resource> vertBuff;
	//���_�o�b�t�@�r���[;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//�萔�o�b�t�@;
	ComPtr<ID3D12Resource> constBuff;

	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff[renderTargetNum];
	// SRV�p�̃f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;

	// �[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthBuff;
	// RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	// DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;

	// ��ʃN���A�̐F
	static const float clearColor[4];

	ComPtr<ID3D12PipelineState> pipelineState;
	ComPtr<ID3D12RootSignature> rootSignature;

	// �p�C�v���C���ƃ��[�g�V�O�l�`���̃Z�b�g
	PipelineSet pipelineSet;

private:
	void initBuffer();

	void createGraphicsPipelineState(const wchar_t* vsPath = L"Resources/Shaders/PostEffectVS.hlsl",
									 const wchar_t* psPath = L"Resources/Shaders/PostEffectPS.hlsl");

public:
	PostEffect();

	void init();

	void draw(DirectXCommon *dxCom);

	void startDrawScene(DirectXCommon *dxCom);

	void endDrawScene(DirectXCommon *dxCom);
};

