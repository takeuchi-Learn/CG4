#pragma once
#include "PostEffect.h"
#include <memory>

class Looper {
private:
	Looper(const Looper& looper) = delete;
	Looper& operator=(const Looper& looper) = delete;
	std::unique_ptr<PostEffect> postEffect;

	Looper();
	~Looper();

public:
	static Looper* getInstance();


	// @return �ُ�̗L��(false�Ő���)
	bool loop();
};

