#pragma once

#include <DirectXMath.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <forward_list>

#include "Camera.h"
#include "Time.h"

#include "Object3d.h"

class ParticleManager {
private:
	// �G�C���A�X
   // Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

	// �T�u�N���X
public:
	// ���_�f�[�^�\����
	struct VertexPos {
		XMFLOAT3 pos; // xyz���W
		float scale; // �X�P�[��
		XMFLOAT3 color;
	};

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData {
		XMMATRIX mat;	// �r���[�v���W�F�N�V�����s��
		XMMATRIX matBillboard;	// �r���{�[�h�s��
	};

	// �p�[�e�B�N��1��
	class Particle {
		// Microsoft::WRL::���ȗ�
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		// DirectX::���ȗ�
		using XMFLOAT2 = DirectX::XMFLOAT2;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT4 = DirectX::XMFLOAT4;
		using XMMATRIX = DirectX::XMMATRIX;

	public:
		// ���W
		XMFLOAT3 position = {};
		// ���x
		XMFLOAT3 velocity = {};
		// �����x
		XMFLOAT3 accel = {};
		// �F
		XMFLOAT3 color = {};
		// �X�P�[��
		float scale = 1.0f;
		// ��]
		float rotation = 0.0f;
		// �����l
		XMFLOAT3 s_color = {};
		float s_scale = 1.0f;
		float s_rotation = 0.0f;
		// �ŏI�l
		XMFLOAT3 e_color = {};
		float e_scale = 0.0f;
		float e_rotation = 0.0f;
		// ���݂̎���
		Time::timeType nowTime = 0;
		// �J�n����
		Time::timeType startTime = 0;
		// �I������
		Time::timeType life = 0;

		Time* timer = nullptr;
	};

	// �萔
private:
	static const int vertexCount = 0x10000;

	// �����o�ϐ�
private:
	// �f�o�C�X
	ID3D12Device* dev = nullptr;
	// �f�X�N���v�^�T�C�Y
	UINT descriptorHandleIncrementSize = 0U;
	// ���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootsignature;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	ComPtr<ID3D12PipelineState> pipelinestate;
	// �f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	// �p�[�e�B�N���z��
	std::forward_list<Particle> particles;
	Camera* camera = nullptr;

	// �����o�֐�
public:
	static void ParticleManager::startDraw(ID3D12GraphicsCommandList* cmdList,
										   Object3d::PipelineSet& ppSet,
										   D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �������Ȃ�
	ParticleManager();

	ParticleManager(ID3D12Device* device, const wchar_t* texFilePath, Camera* camera);

	void init(ID3D12Device* device, const wchar_t* texFilePath);
	void update();
	void draw(ID3D12GraphicsCommandList* cmdList);

	void drawWithUpdate(ID3D12GraphicsCommandList* cmdList);

	inline void setCamera(Camera* camera) { this->camera = camera; }

	/// <summary>
	/// �p�[�e�B�N���̒ǉ�
	/// </summary>
	/// <param name="life">��������</param>
	/// <param name="position">�������W</param>
	/// <param name="velocity">���x</param>
	/// <param name="accel">�����x</param>
	/// <param name="start_scale">�J�n���X�P�[��</param>
	/// <param name="end_scale">�I�����X�P�[��</param>
	void add(Time* timer, int life,
			 XMFLOAT3 position, XMFLOAT3 velocity, XMFLOAT3 accel,
			 float start_scale, float end_scale,
			 float start_rotation, float end_rotation,
			 XMFLOAT3 start_color, XMFLOAT3 end_color);

	/// <summary>
	/// �f�X�N���v�^�q�[�v�̏�����
	/// </summary>
	/// <returns></returns>
	void InitializeDescriptorHeap();

	/// <summary>
	/// �O���t�B�b�N�p�C�v���C������
	/// </summary>
	/// <returns>����</returns>
	void InitializeGraphicsPipeline();

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <returns>����</returns>
	void LoadTexture(const wchar_t* filePath);

	/// <summary>
	/// ���f���쐬
	/// </summary>
	void CreateModel();
};

