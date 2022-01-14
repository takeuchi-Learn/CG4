#include "Model.h"

#include <string>

#include <d3dx12.h>

#include <DirectXTex.h>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <sstream>
#include <fstream>

using namespace std;

using namespace DirectX;
using namespace Microsoft::WRL;

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
	vector<XMFLOAT3> positions;	//頂点座標
	vector<XMFLOAT3> normals;	//法線ベクトル
	vector<XMFLOAT2> texcoords;	//テクスチャUV
	//1行ずつ読み込む
	string line;
	while (getline(file, line)) {

		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		//半角スペース区切りで行の先頭を文字列を取得
		string key;
		getline(line_stream, key, ' ');

		if (key == "v") {
			//X,Y,Z座標読み込み
			XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			//座標データに追加
			positions.emplace_back(position);
		}

		if (key == "vt") {
			//U,V成分読み込み
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			//V方向反転
			texcoord.y = 1.0f - texcoord.y;
			//テクスチャ座標データに追加
			texcoords.emplace_back(texcoord);
		}

		if (key == "vn") {
			//X,Y,Z成分読み込み
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			//法線ベクトルデータに追加
			normals.emplace_back(normal);
		}

		if (key == "f") {
			//半角スペース区切りで行の続きを読み込む
			string index_string;
			while (getline(line_stream, index_string, ' ')) {
				//頂点インデックス1個分の文字列をストリームに変換して解析しやすくする
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				index_stream.seekg(1, ios_base::cur);//スラッシュをとばす
				index_stream >> indexTexcoord;
				index_stream.seekg(1, ios_base::cur);//スラッシュをとばす
				index_stream >> indexNormal;

				//頂点データの追加
				Vertex vertex{};
				vertex.pos = positions[indexPosition - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexTexcoord - 1];
				vertices.emplace_back(vertex);
				//インデックスデータの追加
				indices.emplace_back((unsigned short)indices.size());
			}
		}
	}

	file.close();

	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices.size());

	HRESULT result = S_FALSE;

	//頂点バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	//GPU上のバッファに対応した仮想メモリを取得
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	std::copy(vertices.begin(), vertices.end(), vertMap);

	//マップを解除
	vertBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(Vertex);

	//インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices.size());

	//インデックスバッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff)
	);

	//インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	//GPU上のバッファに対応した仮想メモリを取得
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);

	std::copy(indices.begin(), indices.end(), indexMap);
	//繋がりを解除
	indexBuff->Unmap(0, nullptr);


	//射影変換行列(透視投影)
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(60.0f), // 上下画角60度
		(float)window_width / window_height, // アスペクト比（画面横幅 / 画面縦幅）
		0.1f, 1000.0f // 前端、奥端
	);
}

XMMATRIX Model::getMatProjection() { return matProjection; }

void Model::setTexture(ID3D12Device* dev, UINT newTexNum) {
	TexMetadata metadata{};

	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	//ヒープの二番目にシェーダーリソースビュー作成
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

	//WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	HRESULT result = LoadFromWICFile(
		texPath,
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	const Image* img = scratchImg.GetImage(0, 0, 0);

	//テクスチャバッファのリソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	//テクスチャバッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff[texNum]));

	//テクスチャバッファへのデータ転送
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


	// 生成
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

#pragma region クラス化で削除

void Model::update(XMMATRIX& matView) {
	// UpdateObject3d(&obj3d, matView, matProjection);
}

#pragma endregion クラス化で削除
void Model::draw(ID3D12Device* dev, ID3D12GraphicsCommandList* cmdList, ComPtr<ID3D12Resource> constBuff, const int constantBufferNum, const UINT texNum) {
	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// 頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	// インデックスバッファの設定
	cmdList->IASetIndexBuffer(&ibView);

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	// シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			texNum,
			dev->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);
	// 描画コマンド
	cmdList->DrawIndexedInstanced(indices.size(), 1, 0, 0, 0);
}