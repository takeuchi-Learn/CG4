#pragma once

#include <chrono>

class Time {
private:
	std::chrono::steady_clock::time_point  startTimeDir{};


public:
	using timeUnit = std::chrono::microseconds;
	using timeType = long long;

	static const long long oneSec;

	~Time();

	Time();

	// 一拍の時間を取得
	inline static timeType getOneBeatTime(const float bpm) { return (float)std::chrono::duration_cast<timeUnit>(std::chrono::seconds(60ll)).count() / bpm; };

	// 現在までの時間を取得
	// reset()最後に実行した時間が起点
	// 実行していなければクラス生成時の時間が起点
	timeType getNowTime();

	// 現在を0とする
	void reset();
};
