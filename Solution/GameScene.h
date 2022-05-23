#pragma once
class GameScene {
public:
	virtual ~GameScene() {};	// À‘•‚Í”CˆÓ

	virtual void init() {};		// À‘•‚Í”CˆÓ
	virtual void update() = 0;	// À‘••K{
	virtual void drawObj3d(){};			// À‘•‚Í”CˆÓ
	virtual void drawFrontSprite(){};	// À‘•‚Í”CˆÓ
	virtual void fin() {};		// À‘•‚Í”CˆÓ
};

