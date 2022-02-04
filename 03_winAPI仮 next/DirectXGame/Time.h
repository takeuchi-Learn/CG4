#pragma once

#include <chrono>

class Time {
private:
	std::chrono::steady_clock::time_point  startTimeDir{};
	std::chrono::steady_clock::time_point  nowTimeDir{};

	using timeUnit = std::chrono::microseconds;

public:

	static const long long oneSec;

	~Time();

	Time();

	// �ꔏ�̎��Ԃ��擾
	inline static long long getOneBeatTime(const float bpm) { return (float)std::chrono::duration_cast<timeUnit>(std::chrono::seconds(60ll)).count() / bpm; };

	// ���݂܂ł̎��Ԃ��擾
	// reset()�Ō�Ɏ��s�������Ԃ��N�_
	// ���s���Ă��Ȃ���΃N���X�������̎��Ԃ��N�_
	long long getNowTime();

	// ���݂�0�Ƃ���
	void reset();
};
