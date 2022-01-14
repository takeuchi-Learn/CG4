#pragma once

#include <chrono>

class Time {
private:
	std::chrono::system_clock::time_point  startTimeDir{};
	std::chrono::system_clock::time_point  nowTimeDir{};

public:

	static const long long oneSec;

	~Time();

	Time();

	// �ꔏ�̎��Ԃ��擾
	long long getOneBeatTime(const float bpm);

	// ���݂܂ł̎��Ԃ��擾
	// reset()�Ō�Ɏ��s�������Ԃ��N�_
	// ���s���Ă��Ȃ���΃N���X�������̎��Ԃ��N�_
	long long getNowTime();

	// ���݂�0�Ƃ���
	void reset();
};
