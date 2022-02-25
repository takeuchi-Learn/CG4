#include "Object3d.h"

#include <d3dx12.h>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

void Object3d::createTransferBuffer(ID3D12Device* dev, ComPtr<ID3D12Resource>& constBuff) {
	// �萔�o�b�t�@�̐���
	HRESULT result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),   // �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);
}

XMMATRIX Object3d::getMatWorld() const { return matWorld; }

//void Object3d::setTexture(ID3D12Device* dev, const UINT newTexNum) {
//	texNum = newTexNum;
//	//model->setTexture(dev, newTexNum, constantBufferNum);
//}

Object3d::Object3d(ID3D12Device* dev) {

	matWorld = {};

	// �萔�o�b�t�@�̐���
	createTransferBuffer(dev, constBuff);
}
Object3d::Object3d(ID3D12Device* dev, Model* model, const UINT texNum) : texNum(texNum) {
	matWorld = {};

	// �萔�o�b�t�@�̐���
	createTransferBuffer(dev, constBuff);

	this->model = model;
}

void Object3d::update(const XMMATRIX& matView) {
	XMMATRIX matScale, matRot, matTrans;

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ���[���h�s��̍���
	matWorld = XMMatrixIdentity(); // �ό`�����Z�b�g
	matWorld *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld *= matRot; // ���[���h�s��ɉ�]�𔽉f
	matWorld *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f

	// �e�I�u�W�F�N�g�������
	if (parent != nullptr) {
		// �e�I�u�W�F�N�g�̃��[���h�s����|����
		matWorld *= parent->matWorld;
	}

	// �萔�o�b�t�@�փf�[�^�]��
	ConstBufferData* constMap = nullptr;
	if (SUCCEEDED(constBuff->Map(0, nullptr, (void**)&constMap))) {
		constMap->color = color; // RGBA
		constMap->mat = matWorld * matView * model->getMatProjection();
		constMap->light = light;
		constBuff->Unmap(0, nullptr);
	}

	model->update(matView);
}

void Object3d::draw(DirectXCommon* dxCom) {
	model->draw(dxCom->getDev(), dxCom->getCmdList(), constBuff.Get(), constantBufferNum, texNum);
}

void Object3d::drawWithUpdate(const XMMATRIX& matView, DirectXCommon* dxCom) {
	update(matView);
	draw(dxCom);
}

Object3d::~Object3d() {}



void Object3d::startDraw(ID3D12GraphicsCommandList* cmdList, Object3d::PipelineSet& ppSet, D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology) {
	cmdList->SetPipelineState(ppSet.pipelinestate.Get());
	cmdList->SetGraphicsRootSignature(ppSet.rootsignature.Get());
	//�v���~�e�B�u�`���ݒ�
	cmdList->IASetPrimitiveTopology(PrimitiveTopology);
}

Object3d::PipelineSet Object3d::createGraphicsPipeline(ID3D12Device* dev,
													   BLEND_MODE blendMode,
													   const wchar_t* vsShaderPath, const wchar_t* psShaderPath) {
	HRESULT result = S_FALSE;

	ComPtr<ID3DBlob> vsBlob;	// ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;		// �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob;	// �G���[�I�u�W�F�N�g

	//���_�V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		vsShaderPath,  // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);
	//�G���[�\��
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		psShaderPath,   // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);

	//�G���[�\��
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ���_���C�A�E�g
	// --- ���_�V�F�[�_�[�̈����ƑΉ�
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};	// 1�s�ŏ������ق������₷��

	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	//���_�V�F�[�_�[�A�s�N�Z���V�F�[�_
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
	//�W���I�Ȑݒ�(�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��)
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�

	//���X�^���C�U�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	//�����_�\�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = gpipeline.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //�W���ݒ�
	blenddesc.BlendEnable = true;	//�u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//�\�[�X�̒l��100%�g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//�f�X�g�̒l��0%�g��

	switch (blendMode) {
	case Object3d::BLEND_MODE::ADD:
		//---���Z
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				// ���Z
		blenddesc.SrcBlend = D3D12_BLEND_ONE;				// �\�[�X�̒l��100%�g��
		blenddesc.DestBlend = D3D12_BLEND_ONE;				// �f�X�g�̒l��100%�g��
		break;
	case Object3d::BLEND_MODE::SUB:
		//---���Z
		blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;	// �f�X�g����\�[�X�����Z
		blenddesc.SrcBlend = D3D12_BLEND_ONE;				// �\�[�X�̒l��100%�g��
		blenddesc.DestBlend = D3D12_BLEND_ONE;				// �f�X�g�̒l��100%�g��
		break;
	case Object3d::BLEND_MODE::REVERSE:
		//---���]
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				// ���Z
		blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;	// 1.0 - �f�X�g�J���[�̒l
		blenddesc.DestBlend = D3D12_BLEND_ZERO;
		break;
	default:
		//--����������
		blenddesc.BlendOp = D3D12_BLEND_OP_ADD;				// ���Z
		blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;			// �\�[�X�̃A���t�@�l
		blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	// �f�X�g�̒l��100%�g��
		break;
	}

	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpipeline.NumRenderTargets = 1; // �`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
	//�W���I�Ȑݒ�(�[�x�e�X�g���s���A�������݋��A�[�x����������΍��i)
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//�f�X�N���v�^�e�[�u���̐ݒ�
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV{};
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//t0���W�X�^

	//���[�g�p�����[�^�̐ݒ�
	CD3DX12_ROOT_PARAMETER rootparams[2]{};
	rootparams[0].InitAsConstantBufferView(0); //�萔�o�b�t�@�r���[�Ƃ��ď�����(b0���W�X�^)
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV); //�e�N�X�`���p

	//�e�N�X�`���T���v���[�̐ݒ�
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	Object3d::PipelineSet pipelineSet;

	//���[�g�V�O�l�`���̐ݒ�
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3DBlob> rootSigBlob;
	//�o�[�W������������ł̃V���A���C�Y
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//���[�g�V�O�l�`���̐���
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&pipelineSet.rootsignature));
	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	gpipeline.pRootSignature = pipelineSet.rootsignature.Get();

	//�p�C�v���C���X�e�[�g�̐���
	result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));

	return pipelineSet;
}