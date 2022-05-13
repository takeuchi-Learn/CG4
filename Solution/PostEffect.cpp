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
	// �e�N�X�`�����\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM,
									 WinAPI::window_width, WinAPI::window_height,
									 1, 0, 1, 0,
									 D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	auto dev = DirectXCommon::getInstance()->getDev();

	// �e�N�X�`���o�b�t�@�ݒ�
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
		// ��f��
		const UINT pixelCount = WinAPI::window_width * WinAPI::window_height;
		// ��s���̃f�[�^�T�C�Y
		const UINT rowPitch = sizeof(UINT) * WinAPI::window_width;
		// �摜�S�̂̃f�[�^�T�C�Y
		const UINT depthPitch = rowPitch * WinAPI::window_height;
		// �摜�C���[�W
		UINT* img = new UINT[pixelCount];
		// 0xrrggbbaa�̐F�ɂ���
		for (UINT i = 0; i < pixelCount; i++) {
			img[i] = 0xff0000ff;
		}
		// �e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = texbuff->WriteToSubresource(0, nullptr, img, rowPitch, depthPitch);
		delete[] img;

		assert(SUCCEEDED(result));
	}

	// SRV�p�f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc{};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 1;
	// SRV�p�f�X�N���v�^�q�[�v�𐶐�
	result = dev->CreateDescriptorHeap(&srvDescHeapDesc,
									   IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));

	// SRV�̐ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �e�N�X�`���o�b�t�@�Ɠ���
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	// �f�X�N���v�^�q�[�v��SRV�쐬
	dev->CreateShaderResourceView(texbuff.Get(),
								  &srvDesc,
								  descHeapSRV->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::draw(DirectXCommon* dxCom, SpriteCommon* spCom) {
	if (isInvisible) {
		return;
	}
#pragma region �`��ݒ�

	static auto cmdList = dxCom->getCmdList();

	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(spCom->pipelineSet.pipelinestate.Get());
	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(spCom->pipelineSet.rootsignature.Get());
	// �v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �e�N�X�`���p�f�X�N���v�^�q�[�v�̐ݒ�
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

#pragma endregion �`��ݒ�


#pragma region �`��
	// ���_�o�b�t�@���Z�b�g
	dxCom->getCmdList()->IASetVertexBuffers(0, 1, &vbView);

	// �萔�o�b�t�@���Z�b�g
	dxCom->getCmdList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	// �V�F�[�_���\�[�X�r���[���Z�b�g
	dxCom->getCmdList()->SetGraphicsRootDescriptorTable(1,
														descHeapSRV->GetGPUDescriptorHandleForHeapStart()
	);

	// �|���S���̕`��i4���_�Ŏl�p�`�j
	dxCom->getCmdList()->DrawInstanced(4, 1, 0, 0);


#pragma endregion �`��
}
