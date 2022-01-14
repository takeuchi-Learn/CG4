#include "Time.h"

using namespace std::chrono;

using timeUnit = microseconds;

const long long Time::oneSec = duration_cast<timeUnit>(seconds(1)).count();

Time::~Time() {
}

Time::Time() :
	startTimeDir(system_clock::now()),
	nowTimeDir(startTimeDir) {
}

long long Time::getOneBeatTime(const float bpm) { return 60.0 * (float)oneSec / bpm; }

long long Time::getNowTime() {
	nowTimeDir = system_clock::now();
	return duration_cast<timeUnit>(nowTimeDir - startTimeDir).count();
}

void Time::reset() { startTimeDir = system_clock::now(); }