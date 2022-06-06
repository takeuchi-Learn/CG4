#pragma once
#include "DirectXCommon.h"

#include "Time.h"

#include <memory>

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
		float oneSec;
		float nowTime;
		DirectX::XMFLOAT2 winSize;
		float noiseIntensity = 0.f;
		DirectX::XMFLOAT2 mosaicNum;
		float vignIntensity = 0.9f;
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

	std::unique_ptr<Time> timer;

	float noiseIntensity = 0.f;
	DirectX::XMFLOAT2 mosaicNum;
	float vignIntensity = 0.9f;

private:
	void initBuffer();

	void createGraphicsPipelineState(const wchar_t* vsPath = L"Resources/Shaders/PostEffectVS.hlsl",
									 const wchar_t* psPath = L"Resources/Shaders/PostEffectPS.hlsl");

	void transferConstBuff(float nowTime, float oneSec = Time::oneSec);

public:
	// @param 0 ~ 1
	inline void setNoiseIntensity(float intensity) { noiseIntensity = intensity; }

	inline void setMosaicNum(const DirectX::XMFLOAT2 &mosaicNum) { this->mosaicNum = mosaicNum; }

	PostEffect();

	void init();

	void draw(DirectXCommon *dxCom);

	void startDrawScene(DirectXCommon *dxCom);

	void endDrawScene(DirectXCommon *dxCom);
};

