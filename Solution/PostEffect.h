#pragma once
#include "Sprite.h"

#include "DirectXCommon.h"

class PostEffect :
	public Sprite {

private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;
	// SRV用のデスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;

public:
	PostEffect(UINT texNumber,
			   const SpriteCommon* spriteCommon,
			   const DirectX::XMFLOAT2& anchorpoint = { 0.5f,0.5f },
			   bool isFlipX = false, bool isFlipY = false);

	void init();

	void draw(DirectXCommon* dxCom, SpriteCommon* spCom);
};

