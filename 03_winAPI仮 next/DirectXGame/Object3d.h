#pragma once

#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <d3d12.h>

#include "Model.h"
#include "DirectXCommon.h"

class Object3d {
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	enum class BLEND_MODE : short {
		ALPHA,
		ADD,
		SUB,
		REVERSE
	};

	//�p�C�v���C���Z�b�g
	struct PipelineSet {
		//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
		ComPtr<ID3D12PipelineState> pipelinestate;
		//���[�g�V�O�l�`��
		ComPtr<ID3D12RootSignature> rootsignature;
	};

	// ���_�f�[�^�\����
	//struct Vertex {
	//	XMFLOAT3 pos; // xyz���W
	//	XMFLOAT3 normal; // �@���x�N�g��
	//	XMFLOAT2 uv; // uv���W
	//};

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

	inline XMFLOAT3 subFloat3(const XMFLOAT3& left, const XMFLOAT3& right) {
		return XMFLOAT3(left.x - right.x,
						left.y - right.y,
						left.z - right.z);
	}

public:
	// ���_�o�b�t�@�̍ő吔
	static const int constantBufferNum = 128;

	static void startDraw(ID3D12GraphicsCommandList* cmdList, Object3d::PipelineSet& ppSet,
										D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//3D�I�u�W�F�N�g�p�p�C�v���C������
	// �V�F�[�_�[���f���w��� "*s_5_0"
	static Object3d::PipelineSet createGraphicsPipeline(ID3D12Device* dev,
														BLEND_MODE blendMode = BLEND_MODE::ALPHA,
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

	XMFLOAT4 color = { 1, 1, 1, 1 };

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

	// ���C�g->�I�u�W�F�N�g
	inline void setLightDir(XMFLOAT3 light) { model->setLightDir(light); }
	inline XMFLOAT3 getLightDir() { return model->getLightDir(); }

	inline void setLightPos(XMFLOAT3 lightPos) {
		setLightDir(subFloat3(position, lightPos));
	}

	void update(const XMMATRIX& matView);

	void draw(DirectXCommon* dxCom);

	void drawWithUpdate(const XMMATRIX& matView, DirectXCommon* dxCom);


	~Object3d();
};

