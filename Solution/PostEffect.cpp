#include "PostEffect.h"
#include <d3dx12.h>
using namespace DirectX;

PostEffect::PostEffect(UINT texNumber,
					   const SpriteCommon* spriteCommon,
					   const DirectX::XMFLOAT2& anchorpoint,
					   bool isFlipX, bool isFlipY)
	: Sprite(texNumber,
			 spriteCommon,
			 anchorpoint,
			 isFlipX, isFlipY) {
	setSize({ WinAPI::window_width, WinAPI::window_height });
	update(spriteCommon);

	init();
}

void PostEffect::init() {
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
									 nullptr,
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
		UINT* img = new UINT[pixelCount];
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
}

void PostEffect::draw(DirectXCommon* dxCom, SpriteCommon* spCom) {
	if (isInvisible) {
		return;
	}
#pragma region 描画設定

	static auto cmdList = dxCom->getCmdList();

	// パイプラインステートの設定
	cmdList->SetPipelineState(spCom->pipelineSet.pipelinestate.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(spCom->pipelineSet.rootsignature.Get());
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ用デスクリプタヒープの設定
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

#pragma endregion 描画設定


#pragma region 描画
	// 頂点バッファをセット
	dxCom->getCmdList()->IASetVertexBuffers(0, 1, &vbView);

	// 定数バッファをセット
	dxCom->getCmdList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	// シェーダリソースビューをセット
	dxCom->getCmdList()->SetGraphicsRootDescriptorTable(1,
														descHeapSRV->GetGPUDescriptorHandleForHeapStart()
	);

	// ポリゴンの描画（4頂点で四角形）
	dxCom->getCmdList()->DrawInstanced(4, 1, 0, 0);


#pragma endregion 描画
}
