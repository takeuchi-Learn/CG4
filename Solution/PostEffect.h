#pragma once
#include "Sprite.h"

#include "DirectXCommon.h"

class PostEffect :
	public Sprite {

private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	
	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff;
	// SRV�p�̃f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;

	// �[�x�o�b�t�@
	ComPtr<ID3D12Resource> depthBuff;
	// RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	// DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;

	// ��ʃN���A�̐F
	static const float clearColor[4];

public:
	PostEffect(UINT texNumber,
			   const SpriteCommon* spriteCommon,
			   const DirectX::XMFLOAT2& anchorpoint = { 0.5f,0.5f },
			   bool isFlipX = false, bool isFlipY = false);

	void init();

	void draw(DirectXCommon* dxCom, SpriteCommon* spCom);

	void startDrawScene(DirectXCommon *dxCom);

	void endDrawScene(DirectXCommon *dxCom);
};

