#pragma once

#include <chrono>

class Time {
private:
	std::chrono::steady_clock::time_point  startTimeDir{};


public:
	using timeUnit = std::chrono::microseconds;
	using timeType = long long;

	static constexpr long long oneSec = std::chrono::duration_cast<timeUnit>(std::chrono::seconds(1)).count();

	~Time();

	Time();

	// �ꔏ�̎��Ԃ��擾
	inline static timeType getOneBeatTime(const float bpm) { return (float)std::chrono::duration_cast<timeUnit>(std::chrono::seconds(60ll)).count() / bpm; };

	// ���݂܂ł̎��Ԃ��擾
	// reset()�Ō�Ɏ��s�������Ԃ��N�_
	// ���s���Ă��Ȃ���΃N���X�������̎��Ԃ��N�_
	timeType getNowTime();

	// ���݂�0�Ƃ���
	void reset();
};
