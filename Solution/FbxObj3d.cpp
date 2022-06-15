#include "FbxObj3d.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include "FbxLoader.h"

#include "PostEffect.h"

using namespace Microsoft::WRL;
using namespace DirectX;

ID3D12Device *FbxObj3d::dev = nullptr;
Camera *FbxObj3d::camera = nullptr;

ComPtr<ID3D12RootSignature> FbxObj3d::rootsignature;
ComPtr<ID3D12PipelineState> FbxObj3d::pipelinestate;

void FbxObj3d::createGraphicsPipeline(const wchar_t *vsPath, const wchar_t *psPath) {
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;    // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob; // �G���[�I�u�W�F�N�g

	assert(dev);

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		vsPath,    // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char *)errorBlob->GetBufferPointer(),
					errorBlob->GetBufferSize(),
					errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		psPath,    // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0",    // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char *)errorBlob->GetBufferPointer(),
					errorBlob->GetBufferSize(),
					errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy���W(1�s�ŏ������ق������₷��)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // �@���x�N�g��(1�s�ŏ������ق������₷��)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv���W(1�s�ŏ������ق������₷��)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{	// �e�����󂯂�{�[���ԍ�(4��)
			"BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{	// �e�����󂯂�{�[���ԍ�(4��)
			"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
	};

	// �O���t�B�b�N�X�p�C�v���C���̗����ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// �T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	// ���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// �f�v�X�X�e���V���X�e�[�g
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;    // RBGA�S�Ẵ`�����l����`��
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// �u�����h�X�e�[�g�̐ݒ�
	for (UINT i = 0, maxSize = _countof(gpipeline.BlendState.RenderTarget);
		 i < PostEffect::renderTargetNum && i < maxSize;
		 i++) {
		gpipeline.BlendState.RenderTarget[i] = blenddesc;
	}

	// �[�x�o�b�t�@�̃t�H�[�}�b�g
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// ���_���C�A�E�g�̐ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// �}�`�̌`��ݒ�i�O�p�`�j
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = PostEffect::renderTargetNum;    // �`��Ώۂ̐�
	for (UINT i = 0, maxSize = _countof(gpipeline.BlendState.RenderTarget);
		 i < PostEffect::renderTargetNum && i < maxSize;
		 i++) {
		gpipeline.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0�`255�w���RGBA
	}

	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	// �f�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 ���W�X�^

	// ���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparams[5]{};
	// CBV�i���W�ϊ��s��p�j
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	// SRV�i�e�N�X�`���j
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	// CBV(�X�L�j���O�p)
	rootparams[2].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[3].InitAsConstantBufferView(1);
	rootparams[4].InitAsConstantBufferView(2);

	// �X�^�e�B�b�N�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// �o�[�W������������̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ���[�g�V�O�l�`���̐���
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(rootsignature.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }

	gpipeline.pRootSignature = rootsignature.Get();

	// �O���t�B�b�N�X�p�C�v���C���̐���
	result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelinestate.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { assert(0); }
}



FbxObj3d::FbxObj3d(bool animLoop) : animLoop(animLoop) {
	init();
}
FbxObj3d::FbxObj3d(FbxModel *model, bool animLoop) : animLoop(animLoop) {
	init();
	setModel(model);
}

void FbxObj3d::init() {
	HRESULT result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTransform) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransform)
	);

	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffSkin)
	);

	// ungdone 1�t���[���̎��Ԃ�60FPS��z�肵�ČŒ�
	frameTime.SetTime(0, 0, 0, 1, 0, FbxTime::EMode::eFrames60);

	// �X�L�������ւ̑Ή��̂��߁A�萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataSkin *constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void **)&constMapSkin);
	for (int i = 0; i < MAX_BONES; i++) {
		constMapSkin->bones[i] = XMMatrixIdentity();
	}
	constBuffSkin->Unmap(0, nullptr);
}

void FbxObj3d::update() {
	// �A�j���[�V�����Đ����Ȃ�t���[������i�߂�
	if (isPlay) {
		currentTime += frameTime;
		// ���[�v����ꍇ�A�I�������珉�߂���
		if (currentTime > endTime) {
			if (animLoop) {
				currentTime = startTime;
			} else {
				isPlay = false;
			}
		}
	}

	XMMATRIX matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	XMMATRIX matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	matWorld = XMMatrixIdentity();
	matWorld *= matScale;
	matWorld *= matRot;
	matWorld *= matTrans;

	const XMMATRIX &matViewProj = camera->getViewProjectionMatrix();
	// ���f���̃��b�V���̃g�����X�t�H�[��
	const XMMATRIX &modelTransform = model->GetModelTransform();
	// �J�������W
	const XMFLOAT3 &cameraPos = camera->getEye();

	// �萔�o�b�t�@�փf�[�^��]��
	ConstBufferDataTransform *constMap = nullptr;
	HRESULT result = constBuffTransform->Map(0, nullptr, (void **)&constMap);
	if (SUCCEEDED(result)) {
		constMap->viewproj = matViewProj;
		constMap->world = modelTransform * matWorld;
		constMap->cameraPos = cameraPos;
		constBuffTransform->Unmap(0, nullptr);
	}

	// �{�[���z��
	std::vector<FbxModel::Bone> &bones = model->getBones();

	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferDataSkin *constMapSkin = nullptr;
	result = constBuffSkin->Map(0, nullptr, (void **)&constMapSkin);
	for (UINT i = 0, loopLen = bones.size(); i < loopLen; i++) {
		// ���̎p��
		XMMATRIX matCurrentPose{};
		// ���̎p�����擾
		FbxAMatrix fbxCurrentPose =
			bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
		// XMMATRIX�ɕϊ�
		FbxLoader::convertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);
		// �������ăX�L�j���O�s���
		// ���b�V���m�[�h�̃O���[�o���g�����X�t�H�[�� *
		// �{�[���̏����p���̋t�s�� *
		// �{�[���̍��̎p�� *
		// ���b�V���m�[�h�̃O���[�o���g�����X�t�H�[���̋t�s��
		constMapSkin->bones[i] = model->GetModelTransform() *
			bones[i].invInitialPose
			* matCurrentPose
			* XMMatrixInverse(nullptr, model->GetModelTransform());
	}
	constBuffSkin->Unmap(0, nullptr);
}

void FbxObj3d::draw(ID3D12GraphicsCommandList *cmdList, Light *light) {
	//�@���f�����Ȃ��Ȃ�`�悵�Ȃ�
	if (model == nullptr) return;

	assert(light != nullptr);

	// �p�C�v���C���X�e�[�g�̐ݒ�
	cmdList->SetPipelineState(pipelinestate.Get());
	// ���[�g�V�O�l�`���̐ݒ�
	cmdList->SetGraphicsRootSignature(rootsignature.Get());
	// �v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffTransform->GetGPUVirtualAddress());
	// --- ��������createGraphicsPipeliine��rootparams�̊Y������v�f�ԍ�
	cmdList->SetGraphicsRootConstantBufferView(2, constBuffSkin->GetGPUVirtualAddress());

	light->draw(DirectXCommon::getInstance(), 4);

	cmdList->SetGraphicsRootConstantBufferView(3, model->getConstBuffB1()->GetGPUVirtualAddress());

	// ���f����`��
	model->draw(cmdList);
}

void FbxObj3d::drawWithUpdate(ID3D12GraphicsCommandList *cmdList, Light *light) {
	update();
	draw(cmdList, light);
}

void FbxObj3d::playAnimation() {
	FbxScene *fbxScene = model->getFbxScene();
	// 0�Ԃ̃A�j���[�V�������擾
	FbxAnimStack *animStack = fbxScene->GetSrcObject<FbxAnimStack>(0);
	// �A�j���[�V�����̖��O���擾
	const char *animStackName = animStack->GetName();
	// �A�j���[�V�����̎��Ԏ擾
	FbxTakeInfo *takeInfo = fbxScene->GetTakeInfo(animStackName);

	// �J�n���Ԃ��擾
	startTime = takeInfo->mLocalTimeSpan.GetStart();
	// �I�����Ԃ��擾
	endTime = takeInfo->mLocalTimeSpan.GetStop();
	// ���݂̎��Ԃ��J�n���Ԃɂ���
	currentTime = startTime;
	// �Đ���Ԃɂ���
	isPlay = true;
}
