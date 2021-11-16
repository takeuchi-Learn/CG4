#pragma once

#include <DirectXMath.h>
#include <wrl.h>

#include <d3d12.h>

class ObjCommon {

public:
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData {
		XMFLOAT4 color; // �F (RGBA)
		XMMATRIX mat;   // �R�c�ϊ��s��
	};

	// �p�C�v���C���Z�b�g
	struct PipelineSet {
		// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
		ComPtr<ID3D12PipelineState> pipelinestate;
		// ���[�g�V�O�l�`��
		ComPtr<ID3D12RootSignature> rootsignature;
	};
};

