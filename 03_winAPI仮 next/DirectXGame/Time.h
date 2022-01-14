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

	// 一拍の時間を取得
	long long getOneBeatTime(const float bpm);

	// 現在までの時間を取得
	// reset()最後に実行した時間が起点
	// 実行していなければクラス生成時の時間が起点
	long long getNowTime();

	// 現在を0とする
	void reset();
};
