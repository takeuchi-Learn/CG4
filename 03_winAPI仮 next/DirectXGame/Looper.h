#pragma once
class Looper {
private:
	Looper(const Looper& looper) = delete;
	Looper& operator=(const Looper& looper) = delete;

	Looper();
	~Looper();

public:
	static Looper* getInstance();


	// @return ˆÙí‚Ì—L–³(false‚Å³í)
	bool loop();
};

