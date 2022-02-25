#include "Camera.h"

using namespace DirectX;

namespace {
	XMFLOAT3 operator-(const XMFLOAT3& left, const XMFLOAT3& right) {
		return XMFLOAT3(
		left.x - right.x,
		left.y - right.y,
		left.z - right.z);
	}
	XMFLOAT3 operator+(const XMFLOAT3& left, const XMFLOAT3& right) {
		return XMFLOAT3(
		left.x + right.x,
		left.y + right.y,
		left.z + right.z);
	}

	XMFLOAT3 operator*(const XMFLOAT3& left, const float right) {
		return XMFLOAT3(
		left.x * right,
		left.y * right,
		left.z * right);
	}
}

void Camera::updateViewMatrix() {
	// ���_���W
	XMVECTOR eyePosition = XMLoadFloat3(&eye);
	// �����_���W
	XMVECTOR targetPosition = XMLoadFloat3(&target);
	// �i���́j�����
	XMVECTOR upVector = XMLoadFloat3(&up);

	// �J����Z���i���������j
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
	// 0�x�N�g�����ƌ������������O
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));
	// �x�N�g���𐳋K��
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	// �J������X���i�E�����j
	XMVECTOR cameraAxisX;
	// X���͏������Z���̊O�ςŋ��܂�
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	// �x�N�g���𐳋K��
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	// �J������Y���i������j
	XMVECTOR cameraAxisY;
	// Y����Z����X���̊O�ςŋ��܂�
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);

	// �����܂łŒ�������3�����̃x�N�g��������
	//�i���[���h���W�n�ł̃J�����̉E�����A������A�O�����j	

	// �J������]�s��
	XMMATRIX matCameraRot{};
	// �J�������W�n�����[���h���W�n�̕ϊ��s��
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);
	// �]�u�ɂ��t�s��i�t��]�j���v�Z
	matView = XMMatrixTranspose(matCameraRot);

	// ���_���W��-1���|�������W
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	// �J�����̈ʒu���烏�[���h���_�ւ̃x�N�g���i�J�������W�n�j
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);	// X����
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);	// Y����
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);	// Z����
	// ��̃x�N�g���ɂ܂Ƃ߂�
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
	// �r���[�s��ɕ��s�ړ�������ݒ�
	matView.r[3] = translation;

#pragma region �S�����r���{�[�h�s��̌v�Z
	// �r���{�[�h�s��
	matBillboard.r[0] = cameraAxisX;
	matBillboard.r[1] = cameraAxisY;
	matBillboard.r[2] = cameraAxisZ;
	matBillboard.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma region

#pragma region Y�����r���{�[�h�s��̌v�Z
	// �J����X���AY���AZ��
	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	// X���͋���
	ybillCameraAxisX = cameraAxisX;
	// Y���̓��[���h���W�n��Y��
	ybillCameraAxisY = XMVector3Normalize(upVector);
	// Z����X����Y���̊O�ςŋ��܂�
	ybillCameraAxisZ = XMVector3Cross(ybillCameraAxisX, ybillCameraAxisY);

	// Y�����r���{�[�h�s��
	matBillboardY.r[0] = ybillCameraAxisX;
	matBillboardY.r[1] = ybillCameraAxisY;
	matBillboardY.r[2] = ybillCameraAxisZ;
	matBillboardY.r[3] = XMVectorSet(0, 0, 0, 1);
#pragma endregion
}

void Camera::updateProjectionMatrix() {
	// �������e�ɂ��ˉe�s��̐���
	matProjection = XMMatrixPerspectiveFovLH(
		fogAngleYRad,
		aspectRatio,
		nearZ, farZ
	);
}

void Camera::moveEye(const XMFLOAT3& move) {
	// ���_���W���ړ����A���f
	XMFLOAT3 eye_moved = getEye();

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	setEye(eye_moved);
}

void Camera::moveEye(const XMVECTOR& move) {

	// ���_���W���ړ����A���f
	XMFLOAT3 eye_moved = getEye();

	eye_moved.x += move.m128_f32[0];
	eye_moved.y += move.m128_f32[1];
	eye_moved.z += move.m128_f32[2];

	setEye(eye_moved);
}

void Camera::moveCamera(const XMFLOAT3& move) {

	// ���_�ƒ����_���W���ړ����A���f
	XMFLOAT3 eye_moved = getEye();
	XMFLOAT3 target_moved = getTarget();

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	target_moved.x += move.x;
	target_moved.y += move.y;
	target_moved.z += move.z;

	setEye(eye_moved);
	setTarget(target_moved);
}

void Camera::moveCamera(const XMVECTOR& move) {

	// ���_�ƒ����_���W���ړ����A���f
	XMFLOAT3 eye_moved = getEye();
	XMFLOAT3 target_moved = getTarget();

	eye_moved.x += move.m128_f32[0];
	eye_moved.y += move.m128_f32[1];
	eye_moved.z += move.m128_f32[2];

	target_moved.x += move.m128_f32[0];
	target_moved.y += move.m128_f32[1];
	target_moved.z += move.m128_f32[2];

	setEye(eye_moved);
	setTarget(target_moved);
}

Camera::Camera(const float window_width, const float window_height) {

	fogAngleYRad = DirectX::XM_PI / 3.f;

	aspectRatio = window_width / window_height;

	//�r���[�s��̌v�Z
	updateViewMatrix();

	// �ˉe�s��̌v�Z
	updateProjectionMatrix();

	// �r���[�v���W�F�N�V�����̍���
	matViewProjection = matView * matProjection;
}

Camera::~Camera() {
}

void Camera::update() {
	if (viewDirty || projectionDirty) {
		// �Čv�Z�K�v�Ȃ�
		if (viewDirty) {
			// �r���[�s��X�V
			updateViewMatrix();
			viewDirty = false;
		}

		// �Čv�Z�K�v�Ȃ�
		if (projectionDirty) {
			// �r���[�s��X�V
			updateProjectionMatrix();
			projectionDirty = false;
		}
		// �r���[�v���W�F�N�V�����̍���
		matViewProjection = matView * matProjection;
	}
}


XMFLOAT3 Camera::getLook() const {
	// �����x�N�g��
	XMFLOAT3 look = target - eye;
	// XMVECTOR���o�R���Đ��K��
	const XMVECTOR normalLookVec = XMVector3Normalize(XMLoadFloat3(&look));
	//XMFLOAT3�ɖ߂�
	XMStoreFloat3(&look, normalLookVec);

	return look;
}

void Camera::rotation(const float targetlength, const float angleX, const float angleY) {
	// �����x�N�g��
	const auto look = getLook();

	constexpr float lookLen = 50.f;
	auto newTarget = eye;
	newTarget.x += targetlength * sinf(angleY) + look.x * lookLen;
	newTarget.y += targetlength * sinf(angleX) + look.y * lookLen;
	newTarget.z += targetlength * cosf(angleY) + look.z * lookLen;

	setTarget(newTarget);
}

void Camera::moveForward(const float speed) {
	const auto moveVal = getLook() * speed;

	moveCamera(moveVal);
}

void Camera::moveRight(const float speed) {
	const auto moveVal = getLook() * speed;

	const XMFLOAT3 val{ moveVal.z, /*moveVal.y*/0, -moveVal.x };

	moveCamera(val);
}
