#pragma once

// �����蔻��v���~�e�B�u

#include <DirectXMath.h>

// ��
struct Sphere {
	// ���S���W
	DirectX::XMVECTOR center = { 0, 0, 0, 1 };
	// ���a
	float radius = 1.f;
};

// ����
struct Plane {
	// �@���x�N�g��
	DirectX::XMVECTOR normal = { 0, 1, 0, 0 };
	// ���_(0, 0, 0)����̋���
	float distance = 0.f;
};

