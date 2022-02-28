#pragma once

#include <DirectXMath.h>

class Camera {
private:
	// �G�C���A�X
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private:
	// �r���[�s��
	XMMATRIX matView = DirectX::XMMatrixIdentity();
	// �r���{�[�h�s��
	XMMATRIX matBillboard = DirectX::XMMatrixIdentity();
	// Y�����r���{�[�h�s��
	XMMATRIX matBillboardY = DirectX::XMMatrixIdentity();
	// �ˉe�s��
	XMMATRIX matProjection = DirectX::XMMatrixIdentity();
	// �r���[�ˉe�s��
	XMMATRIX matViewProjection = DirectX::XMMatrixIdentity();
	// �r���[�s��_�[�e�B�t���O
	bool viewDirty = false;
	// �ˉe�s��_�[�e�B�t���O
	bool projectionDirty = false;
	// ���_���W
	XMFLOAT3 eye = { 0, 0, -20 };
	// �����_���W
	XMFLOAT3 target = { 0, 0, 0 };
	// ������x�N�g��
	XMFLOAT3 up = { 0, 1, 0 };
	// �A�X�y�N�g��
	float aspectRatio = 1.0f;

	float nearZ = 0.1f;
	float farZ = 1000.f;
	float fogAngleYRad = DirectX::XM_PI / 3.f;

	// --------------------
	// �����o�֐�
	// --------------------
private:

	// �r���[�s����X�V
	void updateViewMatrix();

	// �ˉe�s����X�V
	void updateProjectionMatrix();

public:
	// �r���[�s��̎擾
	// @return matView
	inline const XMMATRIX& getViewMatrix() { return matView; }

	// �ˉe�s��̎擾
	// @return matProjection
	inline const XMMATRIX& getProjectionMatrix() { return matProjection; }

	// �r���[�ˉe�s��̎擾
	inline const XMMATRIX& getViewProjectionMatrix() { return matViewProjection; }

	// �r���{�[�h�s��̎擾
	inline const XMMATRIX& getBillboardMatrix() { return matBillboard; }

	// Y���Œ�r���{�[�h�s��̎擾
	inline const XMMATRIX& getBillboardMatrixY() { return matBillboardY; }

	// ���_���W�̎擾
	inline const XMFLOAT3& getEye() { return eye; }

	// ���_���W�̐ݒ�
	inline void setEye(XMFLOAT3 eye) { this->eye = eye; viewDirty = true; }

	// �����_���W�̎擾
	inline const XMFLOAT3& getTarget() { return target; }

	// �����_���W�̐ݒ�
	inline void setTarget(XMFLOAT3 target) { this->target = target; viewDirty = true; }

	// ������x�N�g���̎擾
	inline const XMFLOAT3& getUp() { return up; }
	// ������x�N�g���̐ݒ�
	inline void setUp(XMFLOAT3 up) { this->up = up; viewDirty = true; }

	inline void setNearZ(const float nearZ) { this->nearZ = nearZ; projectionDirty = true; }
	inline void setFarZ(const float farZ) { this->farZ = farZ; projectionDirty = true; }

	inline void setFogAngleYRad(const float fogAngleYRad) { this->fogAngleYRad = fogAngleYRad; projectionDirty = true; }

	inline float getNearZ() { return nearZ; }
	inline float getFarZ() { return farZ; }

	inline float getFogAngleYRad() { return fogAngleYRad; }

	XMFLOAT3 getLook() const;

	/// <summary>
	/// �J��������]
	/// </summary>
	/// <param name="targetlength">�J�������璍���_�܂ł̋���</param>
	/// <param name="angleX">X������̉�]�p(-PI/2 ~ PI/2�͈̔͂ő���)</param>
	/// <param name="angleY">Y������̉�]�p(0 ~ 2PI�͈̔͂ő���)</param>
	void rotation(const float targetlength,
						const float angleX, const float angleY);

	void moveForward(const float speed);

	void moveRight(const float speed);


	/// <summary>
	/// �x�N�g���ɂ�鎋�_�ړ�(eye�݂̂̈ړ��Atarget�͕ς��Ȃ�)
	/// </summary>
	/// <param name="move">�ړ���</param>
	void moveEye(const XMFLOAT3& move);
	void moveEye(const XMVECTOR& move);

	/// <summary>
	/// �x�N�g���ɂ��ړ�(eye��target���ړ�)
	/// </summary>
	/// <param name="move">�ړ���</param>
	void moveCamera(const XMFLOAT3& move);
	void moveCamera(const XMVECTOR& move);

	Camera(const float window_width, const float window_height);

	~Camera();

	void update();
};

