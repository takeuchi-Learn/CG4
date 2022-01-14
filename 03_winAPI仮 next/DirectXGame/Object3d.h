#pragma once

#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <d3d12.h>

#include "Model.h"

class Object3d {
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//�p�C�v���C���Z�b�g
	struct PipelineSet {
		//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
		ComPtr<ID3D12PipelineState> pipelinestate;
		//���[�g�V�O�l�`��
		ComPtr<ID3D12RootSignature> rootsignature;
	};

	// ���_�f�[�^�\����
	struct Vertex {
		XMFLOAT3 pos; // xyz���W
		XMFLOAT3 normal; // �@���x�N�g��
		XMFLOAT2 uv; // uv���W
	};

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData {
		XMFLOAT4 color; // �F
		XMMATRIX mat; // �s��
	};

	// --------------------
	// static�����o
	// --------------------
private:
	static void createTransferBuffer(ID3D12Device* dev, ComPtr<ID3D12Resource>& constBuff);

public:
	// ���_�o�b�t�@�̍ő吔
	static const int constantBufferNum = 128;

	static void Object3dCommonBeginDraw(ID3D12GraphicsCommandList* cmdList, ID3D12PipelineState* pPipelineState, ID3D12RootSignature* pRootSignature);

	//3D�I�u�W�F�N�g�p�p�C�v���C������
	// �V�F�[�_�[���f���w��� "*s_5_0"
	static Object3d::PipelineSet Object3dCreateGraphicsPipeline(ID3D12Device* dev,
		const wchar_t* vsShaderPath = L"Resources/Shaders/BasicVS.hlsl",
		const wchar_t* psShaderPath = L"Resources/Shaders/BasicPS.hlsl");

	// --------------------
	// (���I)�����o
	// --------------------
private:
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	// ���[���h�ϊ��s��
	XMMATRIX matWorld;

public:
	UINT texNum = 0;

	// �A�t�B���ϊ����
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };
	// �e�I�u�W�F�N�g�ւ̃|�C���^
	Object3d* parent = nullptr;

	//���f���f�[�^
	Model* model = nullptr;

	XMMATRIX getMatWorld() const;

	//void setTexture(ID3D12Device* dev, const UINT newTexNum);


	// ���f���͌ォ��蓮�œǂݍ���(delete���蓮)
	Object3d(ID3D12Device* dev);

	// ���f���f�[�^�������œn��(delete�͎蓮)
	Object3d(ID3D12Device* dev, Model* model, const UINT texNum);

	void update(XMMATRIX& matView);

	void draw(ID3D12GraphicsCommandList* cmdList, ID3D12Device* dev);


	~Object3d();
};

