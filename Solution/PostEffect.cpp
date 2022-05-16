#include "PostEffect.h"
#include <d3dx12.h>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

const float PostEffect::clearColor[4] = { 0.f, 0.f, 0.f, 0.f };

PostEffect::PostEffect() { init(); }

void PostEffect::initBuffer() {
	constexpr UINT vertNum = 4;

	// 頂点バッファ生成
	HRESULT result = DirectXCommon::getInstance()->getDev()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * vertNum),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);
	assert(SUCCEEDED(result));

	constexpr DirectX::XMFLOAT2 drawSizeRaito = { 1.f, 1.f };
	VertexPosUv vertices[vertNum]{
		{{ -drawSizeRaito.x, -drawSizeRaito.y, 0.f }, { 0.f, 1.f }},
		{{ -drawSizeRaito.x, +drawSizeRaito.y, 0.f }, { 0.f, 0.f }},
		{{ +drawSizeRaito.x, -drawSizeRaito.y, 0.f }, { 1.f, 1.f }},
		{{ +drawSizeRaito.x, +drawSizeRaito.y, 0.f }, { 1.f, 0.f }},
	};

	// 頂点バッファへデータ転送
	VertexPosUv *vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		vertBuff->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexPosUv) * vertNum;
	vbView.StrideInBytes = sizeof(VertexPosUv);

	// 定数バッファの生成
	result = DirectXCommon::getInstance()->getDev()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);
	assert(SUCCEEDED(result));

	// 定数バッファにデータ転送
	ConstBufferData *constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->color = DirectX::XMFLOAT4(1, 1, 1, 1); // 色指定（RGBA）
	constMap->mat = XMMatrixIdentity();
	constBuff->Unmap(0, nullptr);
}

void PostEffect::createGraphicsPipelineState(const wchar_t *vsPath, const wchar_t *psPath) {
	HRESULT result;

	ComPtr<ID3DBlob> vsBlob = nullptr; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		vsPath,  // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char *)errorBlob->GetBufferPointer(),
					errorBlob->GetBufferSize(),
					errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		psPath,   // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char *)errorBlob->GetBufferPointer(),
					errorBlob->GetBufferSize(),
					errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		assert(0);
	}

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
			0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;              // 背面カリングをしない

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC &blenddesc = gpipeline.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // 標準設定
	blenddesc.BlendEnable = true;                   // ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;    // 加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;      // ソースの値を100% 使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;    // デストの値を   0% 使う

	//--半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				//加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			//ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	//デストの値を100%使う

	// デプスステンシルステートの設定
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthEnable = false;    // 深度テストをしない
	//gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;       // 常に上書きルール
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT; // 深度値フォーマット

	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1; // 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// デスクリプタテーブルの設定
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

	// ルートパラメータの設定
	CD3DX12_ROOT_PARAMETER rootparams[2]{};
	rootparams[0].InitAsConstantBufferView(0); // 定数バッファビューとして初期化(b0レジスタ)
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの生成
	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定でのシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc,
												   D3D_ROOT_SIGNATURE_VERSION_1_0,
												   &rootSigBlob,
												   &errorBlob);
	assert(SUCCEEDED(result));

	auto dev = DirectXCommon::getInstance()->getDev();

	// ルートシグネチャの生成
	result = dev->CreateRootSignature(0,
									  rootSigBlob->GetBufferPointer(),
									  rootSigBlob->GetBufferSize(),
									  IID_PPV_ARGS(&pipelineSet.rootsignature));
	assert(SUCCEEDED(result));

	// パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = pipelineSet.rootsignature.Get();

	result = dev->CreateGraphicsPipelineState(&gpipeline,
											  IID_PPV_ARGS(&pipelineSet.pipelinestate));
	assert(SUCCEEDED(result));
}

void PostEffect::init() {
	createGraphicsPipelineState();

	initBuffer();

	// テクスチャリソース設定
	CD3DX12_RESOURCE_DESC texresDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM,
									 WinAPI::window_width, WinAPI::window_height,
									 1, 0, 1, 0,
									 D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	auto dev = DirectXCommon::getInstance()->getDev();

	// テクスチャバッファ設定
	HRESULT result =
		dev->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
															  D3D12_MEMORY_POOL_L0),
									 D3D12_HEAP_FLAG_NONE,
									 &texresDesc,
									 D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
									 &CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM,
														  clearColor),
									 IID_PPV_ARGS(&texbuff));

	assert(SUCCEEDED(result));

	{
		// 画素数
		const UINT pixelCount = WinAPI::window_width * WinAPI::window_height;
		// 一行分のデータサイズ
		const UINT rowPitch = sizeof(UINT) * WinAPI::window_width;
		// 画像全体のデータサイズ
		const UINT depthPitch = rowPitch * WinAPI::window_height;
		// 画像イメージ
		UINT *img = new UINT[pixelCount];
		// 0xrrggbbaaの色にする
		for (UINT i = 0; i < pixelCount; i++) {
			img[i] = 0xff0000ff;
		}
		// テクスチャバッファにデータ転送
		result = texbuff->WriteToSubresource(0, nullptr, img, rowPitch, depthPitch);
		delete[] img;

		assert(SUCCEEDED(result));
	}

	// SRV用デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc{};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 1;
	// SRV用デスクリプタヒープを生成
	result = dev->CreateDescriptorHeap(&srvDescHeapDesc,
									   IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));

	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// テクスチャバッファと同じ
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	// デスクリプタヒープにSRV作成
	dev->CreateShaderResourceView(texbuff.Get(),
								  &srvDesc,
								  descHeapSRV->GetCPUDescriptorHandleForHeapStart());

	// RTV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = 1;
	// RTV用デスクリプタヒープを生成
	result = dev->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&descHeapRTV));
	assert(SUCCEEDED(result));
	// デスクリプタヒープにRTVを作成
	dev->CreateRenderTargetView(texbuff.Get(),
								nullptr,
								descHeapRTV->GetCPUDescriptorHandleForHeapStart());

	// 深度バッファのリソース設定
	CD3DX12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			WinAPI::window_width,
			WinAPI::window_height,
			1, 0,
			1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);
	// 深度バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.f, 0),
		IID_PPV_ARGS(&depthBuff)
	);
	assert(SUCCEEDED(result));

	// DSV用のデスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descHeapDesc.NumDescriptors = 1;
	// DSV用のデスクリプタヒープを作成
	result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeapDSV));
	assert(SUCCEEDED(result));

	// デスクリプタヒープにDSVを作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	// 深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(depthBuff.Get(),
								&dsvDesc,
								descHeapDSV->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::draw(DirectXCommon *dxCom) {
#pragma region 描画設定

	static auto cmdList = dxCom->getCmdList();

	// パイプラインステートの設定
	cmdList->SetPipelineState(pipelineSet.pipelinestate.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(pipelineSet.rootsignature.Get());
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ用デスクリプタヒープの設定
	ID3D12DescriptorHeap *ppHeaps[] = { descHeapSRV.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

#pragma endregion 描画設定


#pragma region 描画
	// 頂点バッファをセット
	dxCom->getCmdList()->IASetVertexBuffers(0, 1, &vbView);

	// 定数バッファをセット
	dxCom->getCmdList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	// シェーダリソースビューをセット
	dxCom->getCmdList()->SetGraphicsRootDescriptorTable(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());

	// ポリゴンの描画（4頂点で四角形）
	dxCom->getCmdList()->DrawInstanced(4, 1, 0, 0);


#pragma endregion 描画
}


void PostEffect::startDrawScene(DirectXCommon *dxCom) {
	auto cmdList = dxCom->getInstance()->getCmdList();

	// リソースバリアを変更(シェーダーリソース -> 描画可能)
	cmdList->ResourceBarrier(1,
							 &CD3DX12_RESOURCE_BARRIER::Transition(texbuff.Get(),
																   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
																   D3D12_RESOURCE_STATE_RENDER_TARGET));

	// レンダーターゲットビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH =
		descHeapRTV->GetCPUDescriptorHandleForHeapStart();
	// 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		descHeapDSV->GetCPUDescriptorHandleForHeapStart();

	// レンダーターゲットをセット
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
	// ビューポートの設定
	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.f, 0.f,
												 WinAPI::window_width,
												 WinAPI::window_height));
	// シザリング矩形の設定
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0,
												WinAPI::window_width, WinAPI::window_height));
	// 全画面クリア
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	// 深度バッファのクリア
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.F, 0, 0, nullptr);
}

void PostEffect::endDrawScene(DirectXCommon *dxCom) {
	auto cmdList = dxCom->getInstance()->getCmdList();
	cmdList->ResourceBarrier(1,
							 &CD3DX12_RESOURCE_BARRIER::Transition(texbuff.Get(),
																   D3D12_RESOURCE_STATE_RENDER_TARGET,
																   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}