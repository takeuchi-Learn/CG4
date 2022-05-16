#include "PostEffect.h"
#include <d3dx12.h>
using namespace DirectX;

const float PostEffect::clearColor[4] = { 1.f, 0.25f, 0.25f, 0.f };

PostEffect::PostEffect(UINT texNumber,
					   const SpriteCommon *spriteCommon,
					   const DirectX::XMFLOAT2 &anchorpoint,
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
									 &CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM,
														  clearColor),
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
		UINT *img = new UINT[pixelCount];
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

	// RTV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = 1;
	// RTV�p�f�X�N���v�^�q�[�v�𐶐�
	result = dev->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&descHeapRTV));
	assert(SUCCEEDED(result));
	// �f�X�N���v�^�q�[�v��RTV���쐬
	dev->CreateRenderTargetView(texbuff.Get(),
								nullptr,
								descHeapRTV->GetCPUDescriptorHandleForHeapStart());

	// �[�x�o�b�t�@�̃��\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC depthResDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			WinAPI::window_width,
			WinAPI::window_height,
			1, 0,
			1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);
	// �[�x�o�b�t�@�̐���
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.f, 0),
		IID_PPV_ARGS(&depthBuff)
	);
	assert(SUCCEEDED(result));

	// DSV�p�̃f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descHeapDesc.NumDescriptors = 1;
	// DSV�p�̃f�X�N���v�^�q�[�v���쐬
	result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeapDSV));
	assert(SUCCEEDED(result));

	// �f�X�N���v�^�q�[�v��DSV���쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	// �[�x�l�t�H�[�}�b�g
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(depthBuff.Get(),
								&dsvDesc,
								descHeapDSV->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::draw(DirectXCommon *dxCom, SpriteCommon *spCom) {
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
	ID3D12DescriptorHeap *ppHeaps[] = { descHeapSRV.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

#pragma endregion �`��ݒ�


#pragma region �`��
	// ���_�o�b�t�@���Z�b�g
	dxCom->getCmdList()->IASetVertexBuffers(0, 1, &vbView);

	// �萔�o�b�t�@���Z�b�g
	dxCom->getCmdList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	// �V�F�[�_���\�[�X�r���[���Z�b�g
	dxCom->getCmdList()->SetGraphicsRootDescriptorTable(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());

	// �|���S���̕`��i4���_�Ŏl�p�`�j
	dxCom->getCmdList()->DrawInstanced(4, 1, 0, 0);


#pragma endregion �`��
}


void PostEffect::startDrawScene(DirectXCommon *dxCom) {
	auto cmdList = dxCom->getInstance()->getCmdList();

	// ���\�[�X�o���A��ύX(�V�F�[�_�[���\�[�X -> �`��\)
	cmdList->ResourceBarrier(1,
							 &CD3DX12_RESOURCE_BARRIER::Transition(texbuff.Get(),
																   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
																   D3D12_RESOURCE_STATE_RENDER_TARGET));

	// �����_�[�^�[�Q�b�g�r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH =
		descHeapRTV->GetCPUDescriptorHandleForHeapStart();
	// �[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		descHeapDSV->GetCPUDescriptorHandleForHeapStart();

	// �����_�[�^�[�Q�b�g���Z�b�g
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
	// �r���[�|�[�g�̐ݒ�
	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.f, 0.f,
												 WinAPI::window_width,
												 WinAPI::window_height));
	// �V�U�����O��`�̐ݒ�
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0,
												WinAPI::window_width, WinAPI::window_height));
	// �S��ʃN���A
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	// �[�x�o�b�t�@�̃N���A
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.F, 0, 0, nullptr);
}

void PostEffect::endDrawScene(DirectXCommon *dxCom) {
	auto cmdList = dxCom->getInstance()->getCmdList();
	cmdList->ResourceBarrier(1,
							 &CD3DX12_RESOURCE_BARRIER::Transition(texbuff.Get(),
																   D3D12_RESOURCE_STATE_RENDER_TARGET,
																   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}