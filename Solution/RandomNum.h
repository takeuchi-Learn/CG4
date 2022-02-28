#pragma once

#include<random>

class RandomNum {
private:
	RandomNum(const RandomNum& mrnd) = delete;
	RandomNum& operator=(const RandomNum& mrnd) = delete;

	std::random_device rnd{};
	std::mt19937_64 mt{};

	RandomNum() : rnd(), mt(rnd()) {};
	~RandomNum() {};

	static RandomNum* getInstance();

private:
	// ��l����
	int local_getRand(const int min, const int max);
	double local_getRand(const double min, const double max);

	// ���K���z����
	double local_getRandNormally(const double center, const double range);

public:
	// ��l����_����
	static int getRand(const int min, const int max);
	// ��l����_����(double)
	static double getRand(const double min, const double max);
	// ��l����_����(float)
	static float getRandf(const float min, const float max);

	// ���K���z����_double
	static double getRandNormally(const double center, const double range);
	// ���K���z����_float
	static float getRandNormallyf(const float center, const float range);
};

