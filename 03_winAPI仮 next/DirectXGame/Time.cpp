#include "Time.h"

using namespace std::chrono;

const Time::timeType Time::oneSec = duration_cast<timeUnit>(seconds(1)).count();

Time::~Time() {
}

Time::Time() :
	startTimeDir(steady_clock::now()){
}

Time::timeType Time::getNowTime() {
	return duration_cast<timeUnit>(steady_clock::now() - startTimeDir).count();
}

void Time::reset() { startTimeDir = steady_clock::now(); }