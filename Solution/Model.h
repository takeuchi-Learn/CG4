#pragma once

#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <unordered_map>
#include "Mesh.h"

class Model {
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

	// --------------------
	// static
	// --------------------
public:

private:
	static ID3D12Device* dev;
	// �f�X�N���v�^�T�C�Y
	static UINT descriptorHandleIncrementSize;

	// --------------------
	// �����o�ϐ�
	// --------------------
private:
	std::string name;
	std::vector<Mesh*> meshes;
	std::unordered_map<std::string, Material*> materials;
	// �f�t�H���g�}�e���A��
	Material* defaultMaterial = nullptr;
	// �f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap;

	/// <summary>
	/// �}�e���A���ǂݍ���
	/// </summary>
	void loadMaterial(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// �}�e���A���o�^
	/// </summary>
	void addMaterial(Material* material);

	/// <summary>
	/// �f�X�N���v�^�q�[�v�̐���
	/// </summary>
	void createDescriptorHeap();

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	void loadTextures(const std::string& dirPath, UINT texNum);

public:
	// ----------
	// static
	// ----------

	// �ÓI������
	static void staticInit(ID3D12Device* device);


	// �����o

	// @param dirPath : obj�t�@�C���̂���ꏊ�̃p�X(�� : Resources/player/)
	// @param objModelName : obj�t�@�C���̃t�@�C����(�g���q�Ȃ��B�� : player.obj -> player)
	Model(const std::string& dirPath, const std::string& objModelName, UINT texNum = 0u, bool smoothing = false);
	~Model();

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="dirPath">���f���t�@�C���̂���p�X</param>
	/// <param name="modelname">���f����(�� : Resources/player.obj)</param>
	void init(const std::string& dirPath, const std::string& modelname, UINT texNum = 0u, bool smoothing = false);

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="cmdList">���ߔ��s��R�}���h���X�g</param>
	void draw(ID3D12GraphicsCommandList* cmdList);
};

