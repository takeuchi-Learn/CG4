#pragma once
class GameScene {
public:
	virtual ~GameScene() {};	// �����͔C��

	virtual void init() {};		// �����͔C��
	virtual void update() = 0;	// �����K�{
	virtual void drawBackSprite() {};	// �����͔C��
	virtual void drawObj3d(){};			// �����͔C��
	virtual void drawFrontSprite(){};	// �����͔C��
	virtual void fin() {};		// �����͔C��
};

