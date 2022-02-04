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

	// 一拍の時間を取得
	inline static long long getOneBeatTime(const float bpm) { return (float)std::chrono::duration_cast<timeUnit>(std::chrono::seconds(60ll)).count() / bpm; };

	// 現在までの時間を取得
	// reset()最後に実行した時間が起点
	// 実行していなければクラス生成時の時間が起点
	long long getNowTime();

	// 現在を0とする
	void reset();
};
