#pragma once
class GameScene {
public:
	virtual ~GameScene() {};	// �����͔C��

	virtual void init() {};		// �����͔C��
	virtual void update() = 0;	// �����K�{
	virtual void draw() = 0;	// �����K�{
	virtual void fin() {};		// �����͔C��
};

