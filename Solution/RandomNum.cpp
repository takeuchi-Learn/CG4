#include <random>
#include "RandomNum.h"

RandomNum* RandomNum::getInstance() {
    static RandomNum myRand{};
    return &myRand;
}

int RandomNum::local_getRand(const int min, const int max) {
	int minLocal = min, maxLocal = max;
	if (max < min) {
		minLocal = max;
		maxLocal = min;
	}
	std::uniform_int_distribution<> myRand(minLocal, maxLocal);	// �͈͎w��̗���
	return myRand(mt);
}

double RandomNum::local_getRand(const double min, const double max) {
	double minLocal = min, maxLocal = max;
	if (max < min) {
		minLocal = max;
		maxLocal = min;
	}
	std::uniform_real_distribution<> myRand(minLocal, maxLocal);	// �͈͎w��̗���
	return myRand(mt);
}

double RandomNum::local_getRandNormally(const double center, const double range) {
	double rangeLocal = range;
	if (range < 0.0) rangeLocal = -rangeLocal;
	else if (range == 0.0) rangeLocal = 1.f;

	std::normal_distribution<> myRand(center, rangeLocal);	// �͈͎w��̗���
	return myRand(mt);
}

int RandomNum::getRand(const int min, const int max) {
	return getInstance()->local_getRand(min, max);
}

double RandomNum::getRand(const double min, const double max) {
	return getInstance()->local_getRand(min, max);
}

float RandomNum::getRandf(const float min, const float max) {
	return (float)getRand((double)min, (double)max);
}

double RandomNum::getRandNormally(const double center, const double range) {
	return getInstance()->local_getRandNormally(center, range);
}

float RandomNum::getRandNormallyf(const float center, const float range) {
	return (float)getRandNormally((double)center, (double)range);
}
