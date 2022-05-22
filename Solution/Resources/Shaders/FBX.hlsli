cbuffer cbuff0 : register(b0) {
	matrix viewproj;	// �r���[�v���W�F�N�V�����s��
	matrix world;		// ���[���h�s��	
	float3 cameraPos;	// �J�������W(���[���h���W)
};

// �{�[���̍ő吔(FbxObj3d.h�̒萔�ƍ��킹��)
static const int MAX_BONES = 32;

// �{�[���̃X�L�j���O�s�񂪓���
cbuffer skinning : register(b3) {
	matrix matSkinning[MAX_BONES];
}

struct VSInput {
	float4 pos : POSITION;	// �ʒu
	float3 normal : NORMAL;	// ���_�̖@��
	float2 uv : TEXCOORD;	// �e�N�X�`���\���W
	uint4 boneIndices : BONEINDICES;	// �{�[���̔ԍ�
	float4 boneWeights : BONEWEIGHTS;	// �{�[���̃X�L���E�F�C�g
};

struct VSOutput {
	float4 svpos : SV_POSITION;	// �V�X�e���p���_���W
	float3 normal : NORMAL;		// �@��
	float2 uv : TEXCOORD;		// uv
};

// �����_�[�^�[�Q�b�g�̐���2��
// undone PostEffect�N���X��RenderTargetNum�͍����̌�(2��)�ɍ��킹��
// �z��ɂ���΂�����̂ł�?
struct PSOutput {
	float4 target0 : SV_TARGET0;
	float4 target1 : SV_TARGET1;
};