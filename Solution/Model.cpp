#include "Model.h"

#include <string>

#include <d3dx12.h>

#include <DirectXTex.h>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <sstream>
#include <fstream>


#include <algorithm>

using namespace std;

using namespace DirectX;
using namespace Microsoft::WRL;

ID3D12Device* Model::dev = nullptr;
UINT Model::descriptorHandleIncrementSize = 0u;

namespace {
	constexpr float nearZ = 0.1f, farZ = 1000.f, fog = XM_PI / 3.f;
}


void Model::loadTextures(const std::string& dirPath, UINT texNum) {
	int textureIndex = 0;
	string directoryPath = dirPath;
	if (dirPath[dirPath.size() - 1] != '/') {
		if (dirPath[dirPath.size() - 1] != '\\') {
			directoryPath += '/';
		}
	}

	for (auto& m : materials) {
		Material* material = m.second;

		// �e�N�X�`������
		if (material->texFileName.size() > 0) {
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), textureIndex, descriptorHandleIncrementSize);
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap->GetGPUDescriptorHandleForHeapStart(), textureIndex, descriptorHandleIncrementSize);
			// �}�e���A���Ƀe�N�X�`���ǂݍ���
			material->loadTexture(directoryPath, texNum, cpuDescHandleSRV, gpuDescHandleSRV);
			textureIndex++;
		}
		// �e�N�X�`���Ȃ�
		else {
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), textureIndex, descriptorHandleIncrementSize);
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeap->GetGPUDescriptorHandleForHeapStart(), textureIndex, descriptorHandleIncrementSize);
			// �}�e���A���Ƀe�N�X�`���ǂݍ���
			material->loadTexture(directoryPath, texNum, cpuDescHandleSRV, gpuDescHandleSRV);
			textureIndex++;
		}
	}
}

void Model::createDescriptorHeap() {
	HRESULT result = S_FALSE;

	// �}�e���A���̐�
	size_t count = materials.size();

	// �f�X�N���v�^�q�[�v�𐶐�	
	if (count > 0) {
		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
		descHeapDesc.NumDescriptors = (UINT)count; // �V�F�[�_�[���\�[�X�r���[�̐�
		result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));//����
		if (FAILED(result)) {
			assert(0);
		}
	}

	// �f�X�N���v�^�T�C�Y���擾
	descriptorHandleIncrementSize = dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void Model::addMaterial(Material* material) {
	// �R���e�i�ɓo�^
	materials.emplace(material->name, material);
}


void Model::loadMaterial(const std::string& directoryPath, const std::string& filename) {
	// �t�@�C���X�g���[��
	std::ifstream file;
	// �}�e���A���t�@�C�����J��
	file.open(directoryPath + filename);
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	Material* material = nullptr;

	// 1�s���ǂݍ���
	string line;
	while (getline(file, line)) {

		// 1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		// ���p�X�y�[�X��؂�ōs�̐擪��������擾
		string key;
		getline(line_stream, key, ' ');

		// �擪�̃^�u�����͖�������
		if (key[0] == '\t') {
			key.erase(key.begin()); // �擪�̕������폜
		}

		// �擪������newmtl�Ȃ�}�e���A����
		if (key == "newmtl") {

			// ���Ƀ}�e���A���������
			if (material) {
				// �}�e���A�����R���e�i�ɓo�^
				addMaterial(material);
			}

			// �V�����}�e���A���𐶐�
			material = new Material();
			// �}�e���A�����ǂݍ���
			line_stream >> material->name;
		}
		// �擪������Ka�Ȃ�A���r�G���g�F
		if (key == "Ka") {
			line_stream >> material->ambient.x;
			line_stream >> material->ambient.y;
			line_stream >> material->ambient.z;
		}
		// �擪������Kd�Ȃ�f�B�t���[�Y�F
		if (key == "Kd") {
			line_stream >> material->diffuse.x;
			line_stream >> material->diffuse.y;
			line_stream >> material->diffuse.z;
		}
		// �擪������Ks�Ȃ�X�y�L�����[�F
		if (key == "Ks") {
			line_stream >> material->specular.x;
			line_stream >> material->specular.y;
			line_stream >> material->specular.z;
		}
		// �擪������map_Kd�Ȃ�e�N�X�`���t�@�C����
		// todo �e�N�X�`���摜�p�X�w��������̒l��baseDir�ōs���悤�ɂ���
		if (key == "map_Kd") {
			// �e�N�X�`���̃t�@�C�����ǂݍ���
			line_stream >> material->texFileName;

			// �t���p�X����t�@�C���������o��
			size_t pos1;
			pos1 = material->texFileName.rfind('\\');
			if (pos1 != string::npos) {
				material->texFileName = material->texFileName.substr(pos1 + 1, material->texFileName.size() - pos1 - 1);
			}

			pos1 = material->texFileName.rfind('/');
			if (pos1 != string::npos) {
				material->texFileName = material->texFileName.substr(pos1 + 1, material->texFileName.size() - pos1 - 1);
			}
		}
	}
	// �t�@�C�������
	file.close();

	if (material) {
		// �}�e���A����o�^
		addMaterial(material);
	}
}

void Model::staticInit(ID3D12Device* device) {
	// �ď������`�F�b�N
	assert(!Model::dev);

	Model::dev = device;

	// ���b�V���̐ÓI������
	Mesh::staticInit(device);
}

Model::Model(const std::string& dirPath, const std::string& objModelName, UINT texNum, bool smoothing) {
	string directoryPath = dirPath;
	if (dirPath[dirPath.size() - 1] != '/') {
		if (dirPath[dirPath.size() - 1] != '\\') {
			directoryPath += '/';
		}
	}
	init(directoryPath, objModelName, texNum, smoothing);
}

Model::~Model() {
	for (auto m : meshes) {
		delete m;
	}
	meshes.clear();

	for (auto m : materials) {
		delete m.second;
	}
	materials.clear();
}

void Model::init(const std::string& dirPath, const std::string& modelname, UINT texNum, bool smoothing) {
	const string filename = modelname + ".obj";
	string directoryPath = dirPath;
	if (dirPath[dirPath.size() - 1] != '/') {
		if (dirPath[dirPath.size() - 1] != '\\') {
			directoryPath += '/';
		}
	}

	// �t�@�C���X�g���[��
	std::ifstream file;
	// .obj�t�@�C�����J��
	file.open(directoryPath + filename);
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	name = modelname;

	// ���b�V������
	Mesh* mesh = new Mesh;
	int indexCountTex = 0;
	int indexCountNoTex = 0;

	vector<XMFLOAT3> positions;	// ���_���W
	vector<XMFLOAT3> normals;	// �@���x�N�g��
	vector<XMFLOAT2> texcoords;	// �e�N�X�`��UV
	// 1�s���ǂݍ���
	string line;
	while (getline(file, line)) {

		// 1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		// ���p�X�y�[�X��؂�ōs�̐擪��������擾
		string key;
		getline(line_stream, key, ' ');

		//�}�e���A��
		if (key == "mtllib") {
			// �}�e���A���̃t�@�C�����ǂݍ���
			string filename;
			line_stream >> filename;
			// �}�e���A���ǂݍ���
			loadMaterial(directoryPath, filename);
		}
		// �擪������g�Ȃ�O���[�v�̊J�n
		if (key == "g") {
			// �J�����g���b�V���̏�񂪑����Ă���Ȃ�
			if (mesh->getName().size() > 0 && mesh->getVertexCount() > 0) {
				// ���_�@���̕��ςɂ��G�b�W������
				if (smoothing) mesh->calculateSmoothedVertexNormals();
				// �R���e�i�ɓo�^
				meshes.emplace_back(mesh);
				// ���̃��b�V������
				mesh = new Mesh;
				indexCountTex = 0;
			}

			// �O���[�v���ǂݍ���
			string groupName;
			line_stream >> groupName;

			// ���b�V���ɖ��O���Z�b�g
			mesh->setName(groupName);
		}
		// �擪������v�Ȃ璸�_���W
		if (key == "v") {
			// X,Y,Z���W�ǂݍ���
			XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			positions.emplace_back(position);
		}
		// �擪������vt�Ȃ�e�N�X�`��
		if (key == "vt") {
			// U,V�����ǂݍ���
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			// V�������]
			texcoord.y = 1.0f - texcoord.y;
			// �e�N�X�`�����W�f�[�^�ɒǉ�
			texcoords.emplace_back(texcoord);
		}
		// �擪������vn�Ȃ�@���x�N�g��
		if (key == "vn") {
			// X,Y,Z�����ǂݍ���
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			// �@���x�N�g���f�[�^�ɒǉ�
			normals.emplace_back(normal);
		}
		// �擪������usemtl�Ȃ�}�e���A�������蓖�Ă�
		if (key == "usemtl") {
			if (mesh->getMaterial() == nullptr) {
				// �}�e���A���̖��ǂݍ���
				string materialName;
				line_stream >> materialName;

				// �}�e���A�����Ō������A�}�e���A�������蓖�Ă�
				auto itr = materials.find(materialName);
				if (itr != materials.end()) {
					mesh->setMaterial(itr->second);
				}
			}
		}
		// �擪������f�Ȃ�|���S���i�O�p�`�j
		if (key == "f") {
			int faceIndexCount = 0;
			// ���p�X�y�[�X��؂�ōs�̑�����ǂݍ���
			string index_string;
			while (getline(line_stream, index_string, ' ')) {
				// ���_�C���f�b�N�X1���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				// ���_�ԍ�
				index_stream >> indexPosition;

				Material* material = mesh->getMaterial();
				index_stream.seekg(1, ios_base::cur); // �X���b�V�����΂�
				// �}�e���A���A�e�N�X�`��������ꍇ
				if (material && material->texFileName.size() > 0) {
					index_stream >> indexTexcoord;
					index_stream.seekg(1, ios_base::cur); // �X���b�V�����΂�
					index_stream >> indexNormal;
					// ���_�f�[�^�̒ǉ�
					Mesh::VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcoords[indexTexcoord - 1];
					mesh->addVertex(vertex);
					// �G�b�W�������p�f�[�^�ǉ�
					if (smoothing) {
						// ���W�f�[�^(v�L�[)�̔ԍ��ƁA�S�č��v���������X�̃C���f�b�N�X���Z�b�g�œo�^����
						mesh->addSmoothData(indexPosition, (unsigned short)mesh->getVertexCount() - 1);
					}
				} else {
					char c;
					index_stream >> c;
					// �X���b�V��2�A���̏ꍇ�A���_�ԍ��̂�
					if (c == '/') {
						// ���_�f�[�^�̒ǉ�
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = { 0, 0, 1 };
						vertex.uv = { 0, 0 };
						mesh->addVertex(vertex);
					} else {
						index_stream.seekg(-1, ios_base::cur); // 1�����߂�
						index_stream >> indexTexcoord;
						index_stream.seekg(1, ios_base::cur); // �X���b�V�����΂�
						index_stream >> indexNormal;
						// ���_�f�[�^�̒ǉ�
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = normals[indexNormal - 1];
						vertex.uv = { 0, 0 };
						mesh->addVertex(vertex);
					}
				}
				// �C���f�b�N�X�f�[�^�̒ǉ�
				if (faceIndexCount >= 3) {
					// �l�p�`�|���S����4�_�ڂȂ̂ŁA
					// �l�p�`��0,1,2,3�̓� 2,3,0�ŎO�p�`���\�z����
					mesh->addIndex(indexCountTex - 1);
					mesh->addIndex(indexCountTex);
					mesh->addIndex(indexCountTex - 3);
				} else {
					mesh->addIndex(indexCountTex);
				}
				indexCountTex++;
				faceIndexCount++;
			}
		}
	}
	file.close();

	// ���_�@���̕��ςɂ��G�b�W������
	if (smoothing) mesh->calculateSmoothedVertexNormals();

	// �R���e�i�ɓo�^
	meshes.emplace_back(mesh);

	// ���b�V���̃}�e���A���`�F�b�N
	for (auto& m : meshes) {
		// �}�e���A���̊��蓖�Ă��Ȃ�
		if (m->getMaterial() == nullptr) {
			if (defaultMaterial == nullptr) {
				// �f�t�H���g�}�e���A���𐶐�
				defaultMaterial = new Material();
				defaultMaterial->name = "no material";
				materials.emplace(defaultMaterial->name, defaultMaterial);
			}
			// �f�t�H���g�}�e���A�����Z�b�g
			m->setMaterial(defaultMaterial);
		}
	}

	// ���b�V���̃o�b�t�@����
	for (auto& m : meshes) {
		m->createBuffers();
	}

	// �}�e���A���̐��l��萔�o�b�t�@�ɔ��f
	for (auto& m : materials) {
		m.second->update();
	}

	// �f�X�N���v�^�q�[�v����
	createDescriptorHeap();

	// �e�N�X�`���̓ǂݍ���
	loadTextures(directoryPath, texNum);
}

void Model::draw(ID3D12GraphicsCommandList* cmdList) {
	// �f�X�N���v�^�q�[�v�̔z��
	if (descHeap) {
		ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
		cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	}

	// �S���b�V����`��
	for (auto& mesh : meshes) {
		mesh->draw(cmdList);
	}
}

