#pragma once

#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <d3d12.h>

class Model {
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

	// ���_�f�[�^�\����
	struct Vertex {
		XMFLOAT3 pos;		// xyz���W
		XMFLOAT3 normal;	// �@���x�N�g��
		XMFLOAT2 uv;		// uv���W
	};

	// --------------------
	// 2d3d���ʍ\����
	// --------------------

	// �p�C�v���C���Z�b�g
	struct PipelineSet {
		// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
		ComPtr<ID3D12PipelineState> pipelinestate;
		// ���[�g�V�O�l�`��
		ComPtr<ID3D12RootSignature> rootsignature;
	};



	// --------------------
	// static
	// --------------------
public:


	// --------------------
	// �����o�ϐ�
	// --------------------
private:
	// --------------------
	// obj3d���� �s��(�������f��)
	// --------------------
	// �萔�o�b�t�@�p�f�X�N���v�^�[�q�[�v(�f�X�N���v�^���Ǘ�����)
	ComPtr<ID3D12DescriptorHeap> descHeap = nullptr;

	// --------------------
	// obj3d���� ���f��
	// --------------------
	std::vector<Vertex> vertices{};
	bool vertDirty = false;
	ComPtr<ID3D12Resource> vertBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	Vertex* vertMap{};

	std::vector<unsigned short> indices{};
	ComPtr<ID3D12Resource> indexBuff;
	D3D12_INDEX_BUFFER_VIEW ibView;

	std::vector<ComPtr<ID3D12Resource>> texBuff;

	// �ˉe�ϊ��s��
	XMMATRIX matProjection;


	inline std::vector<Vertex> getVertices() const { return vertices; }
	inline void setVertices(const std::vector<Vertex> vertices) { this->vertices = vertices; vertDirty = true; }

	void loadModel(ID3D12Device* dev,
				   const wchar_t* objPath,
				   const int window_width, const int window_height);

	void loadSphere(ID3D12Device* dev, const float r, const int window_width, const int window_height);

	void transVertBuff(ID3D12Device* dev);

public:
	void loadTexture(ID3D12Device* dev, const wchar_t* texPath, const UINT texNum);

	void setTexture(ID3D12Device* dev, UINT newTexNum);

	Model(ID3D12Device* dev,
		const wchar_t* objPath, const wchar_t* texPath,
		const int window_width, const int window_height,
		const unsigned int constantBufferNum,
		const int texNum);

	// ����
	Model(ID3D12Device* dev,
		  const wchar_t* texPath,
		  const float r,
		  const int window_width, const int window_height,
		  const unsigned int constantBufferNum,
		  const int texNum);

	XMMATRIX getMatProjection();

	void update(ID3D12Device* dev);

	void draw(ID3D12Device* dev, ID3D12GraphicsCommandList* cmdList, ComPtr<ID3D12Resource> constBuff, const int constantBufferNum, const UINT texNum);
};

