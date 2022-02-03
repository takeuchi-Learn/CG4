#include "Time.h"

using namespace std::chrono;

using timeUnit = microseconds;

namespace {
	constexpr auto oneSec = timeUnit(1s);
}

const long long Time::oneSec = duration_cast<timeUnit>(seconds(1)).count();

Time::~Time() {
}

Time::Time() :
	startTimeDir(steady_clock::now()),
	nowTimeDir(startTimeDir) {
}

long long Time::getOneBeatTime(const float bpm) { return 60.f * (float)oneSec / bpm; }

long long Time::getNowTime() {
	nowTimeDir = steady_clock::now();
	return duration_cast<timeUnit>(nowTimeDir - startTimeDir).count();
}

void Time::reset() { startTimeDir = steady_clock::now(); }