#include "Model.h"

#include <string>

#include <d3dx12.h>

#include <DirectXTex.h>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <sstream>
#include <fstream>

//using namespace std;
template<class T> using vector = std::vector<T>;
using string = std::string;

using namespace DirectX;
using namespace Microsoft::WRL;

namespace {
	constexpr float nearZ = 0.1f, farZ = 1000.f, fog = XM_PI / 3.f;
}

void Model::loadModel(ID3D12Device* dev,
	std::vector<Vertex>& vertices, std::vector<unsigned short>& indices,
	const wchar_t* objPath,
	const int window_width, const int window_height,
	ComPtr<ID3D12Resource>& vertBuff, Vertex* vertMap, D3D12_VERTEX_BUFFER_VIEW& vbView,
	ComPtr<ID3D12Resource>& indexBuff, D3D12_INDEX_BUFFER_VIEW& ibView,
	XMMATRIX& matProjection) {

	std::ifstream file;

	file.open(objPath);

	if (file.fail()) {
		assert(0);
	}
	vector<XMFLOAT3> positions;	//���_���W
	vector<XMFLOAT3> normals;	//�@���x�N�g��
	vector<XMFLOAT2> texcoords;	//�e�N�X�`��UV
	//1�s���ǂݍ���
	string line;
	while (getline(file, line)) {

		//1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		//���p�X�y�[�X��؂�ōs�̐擪�𕶎�����擾
		string key;
		getline(line_stream, key, ' ');

		if (key == "v") {
			//X,Y,Z���W�ǂݍ���
			XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			//���W�f�[�^�ɒǉ�
			positions.emplace_back(position);
		}

		if (key == "vt") {
			//U,V�����ǂݍ���
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			//V�������]
			texcoord.y = 1.0f - texcoord.y;
			//�e�N�X�`�����W�f�[�^�ɒǉ�
			texcoords.emplace_back(texcoord);
		}

		if (key == "vn") {
			//X,Y,Z�����ǂݍ���
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			//�@���x�N�g���f�[�^�ɒǉ�
			normals.emplace_back(normal);
		}

		if (key == "f") {
			//���p�X�y�[�X��؂�ōs�̑�����ǂݍ���
			string index_string;
			while (getline(line_stream, index_string, ' ')) {
				//���_�C���f�b�N�X1���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				index_stream.seekg(1, std::ios_base::cur);//�X���b�V�����Ƃ΂�
				index_stream >> indexTexcoord;
				index_stream.seekg(1, std::ios_base::cur);//�X���b�V�����Ƃ΂�
				index_stream >> indexNormal;

				//���_�f�[�^�̒ǉ�
				Vertex vertex{};
				vertex.pos = positions[indexPosition - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexTexcoord - 1];
				vertices.emplace_back(vertex);
				//�C���f�b�N�X�f�[�^�̒ǉ�
				indices.emplace_back((unsigned short)indices.size());
			}
		}
	}

	file.close();

	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());

	HRESULT result = S_FALSE;

	transVertBuff(dev);

	//���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Vertex);

	//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	//�C���f�b�N�X�o�b�t�@�̐���
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);

	std::copy(indices.begin(), indices.end(), indexMap);
	//�q���������
	indexBuff->Unmap(0, nullptr);


	//�ˉe�ϊ��s��(�������e)
	matProjection = XMMatrixPerspectiveFovLH(
		fog, // �㉺��p60�x
		(float)window_width / window_height, // �A�X�y�N�g��i��ʉ��� / ��ʏc���j
		nearZ, farZ // �O�[�A���[
	);
}

void Model::loadSphere(ID3D12Device* dev, const float r, const int window_width, const int window_height) {

	constexpr UINT vMax = 32, uMax = 32;

	constexpr int vertexNum = uMax * (vMax + 1);
	constexpr int indexNum = 2 * vMax * (uMax + 1);

	vertices.resize(vertexNum);
	indices.resize(indexNum);

	for (UINT v = 0; v <= vMax; v++) {
		for (UINT u = 0; u < uMax; u++) {
			float theata = XM_PI * v / vMax;
			float phi = (XM_PI * 2) * u / uMax;
			float fX = r * sin(theata) * cos(phi);
			float fY = r * cos(theata);
			float fZ = r * sin(theata) * sin(phi);
			vertices[uMax * v + u].pos = XMFLOAT3(fX, fY, fZ);
		}
	}

	for (UINT v = 0, i = 0; v < vMax; v++) {
		for (UINT u = 0; u <= uMax; u++) {
			if (u == uMax) {
				indices[i] = v * uMax;
				i++;
				indices[i] = (v + 1) * uMax;
				i++;
			} else {
				indices[i] = (v * uMax) + u;
				i++;
				indices[i] = indices[i - 1] + uMax;
				i++;
			}
		}
	}

	for (UINT i = 0; i < indices.size() / 3; i++) {// �O�p�`�P���ƂɌv�Z���Ă���
	// �O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
		USHORT index0 = indices[i * 3 + 0];
		USHORT index1 = indices[i * 3 + 1];
		USHORT index2 = indices[i * 3 + 2];
		// �O�p�`���\�����钸�_���W���x�N�g���ɑ��
		XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
		// p0��p1�x�N�g���Ap0��p2�x�N�g�����v�Z�i�x�N�g���̌��Z�j
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		// �O�ς͗������琂���ȃx�N�g��
		XMVECTOR normal = XMVector3Cross(v1, v2);
		// ���K���i������1�ɂ���)
		normal = XMVector3Normalize(normal);
		// ���߂��@���𒸓_�f�[�^�ɑ��
		XMStoreFloat3(&vertices[index0].normal, normal);
		XMStoreFloat3(&vertices[index1].normal, normal);
		XMStoreFloat3(&vertices[index2].normal, normal);
	}

	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());

	HRESULT result = S_FALSE;

	transVertBuff(dev);

	//���_�o�b�t�@�r���[�̍쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Vertex);

	//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	//�C���f�b�N�X�o�b�t�@�̐���
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);

	std::copy(indices.begin(), indices.end(), indexMap);
	//�q���������
	indexBuff->Unmap(0, nullptr);


	//�ˉe�ϊ��s��(�������e)
	matProjection = XMMatrixPerspectiveFovLH(
		fog, // �㉺��p60�x
		(float)window_width / window_height, // �A�X�y�N�g��i��ʉ��� / ��ʏc���j
		nearZ, farZ // �O�[�A���[
	);
}

void Model::transVertBuff(ID3D12Device* dev) {
	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());

	HRESULT result = S_FALSE;

	//���_�o�b�t�@�̐���
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	std::copy(vertices.begin(), vertices.end(), vertMap);

	//�}�b�v������
	vertBuff->Unmap(0, nullptr);
}

XMMATRIX Model::getMatProjection() { return matProjection; }

void Model::setTexture(ID3D12Device* dev, UINT newTexNum) {
	TexMetadata metadata{};

	//�V�F�[�_�[���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	//�q�[�v�̓�ԖڂɃV�F�[�_�[���\�[�X�r���[�쐬
	dev->CreateShaderResourceView(texBuff[newTexNum].Get(),
		&srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeap->GetCPUDescriptorHandleForHeapStart(),
			newTexNum,
			dev->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
			)
		)
	);
}

void Model::loadTexture(ID3D12Device* dev, const wchar_t* texPath, const UINT texNum) {
	assert(texNum <= texBuff.size() - 1);

	//WIC�e�N�X�`���̃��[�h
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	HRESULT result = LoadFromWICFile(
		texPath,
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	const Image* img = scratchImg.GetImage(0, 0, 0);

	//�e�N�X�`���o�b�t�@�̃��\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	//�e�N�X�`���o�b�t�@�̐���
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff[texNum]));

	//�e�N�X�`���o�b�t�@�ւ̃f�[�^�]��
	result = texBuff[texNum]->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch
	);

	setTexture(dev, texNum);
}

Model::Model(ID3D12Device* dev,
	const wchar_t* objPath, const wchar_t* texPath,
	const int window_width, const int window_height,
	const unsigned int constantBufferNum,
	const int texNum = 0) {

	texBuff.resize(constantBufferNum);

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = constantBufferNum + 1;


	// ����
	HRESULT result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));

	loadModel(dev, vertices, indices, objPath,
		window_width, window_height,
		vertBuff, vertMap, vbView,
		indexBuff, ibView, matProjection
	);


	//InitializeObject3d(&obj3d, 0, dev, descHeap.Get());

	loadTexture(dev, texPath, texNum);
	//setTexture(dev, texNum, constantBufferNum);
}

Model::Model(ID3D12Device* dev,
			 const wchar_t* texPath,
			 const float r,
			 const int window_width, const int window_height,
			 const unsigned int constantBufferNum,
			 const int texNum) {
	texBuff.resize(constantBufferNum);

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = constantBufferNum + 1;


	// ����
	HRESULT result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));

	loadSphere(dev, r, window_width, window_height);

	loadTexture(dev, texPath, texNum);
}

#pragma region �N���X���ō폜

void Model::update(const XMMATRIX& matView) {
	// UpdateObject3d(&obj3d, matView, matProjection);
}

#pragma endregion �N���X���ō폜
void Model::draw(ID3D12Device* dev, ID3D12GraphicsCommandList* cmdList, ComPtr<ID3D12Resource> constBuff, const int constantBufferNum, const UINT texNum) {
	transVertBuff(dev);

	// �f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// ���_�o�b�t�@�̐ݒ�
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	// �C���f�b�N�X�o�b�t�@�̐ݒ�
	cmdList->IASetIndexBuffer(&ibView);

	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	// �V�F�[�_���\�[�X�r���[���Z�b�g
	cmdList->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			texNum,
			dev->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);
	// �`��R�}���h
	cmdList->DrawIndexedInstanced(indices.size(), 1, 0, 0, 0);
}