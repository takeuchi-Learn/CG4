#pragma once
class GameScene {
public:
	virtual ~GameScene() {};	// À‘•‚Í”CˆÓ

	virtual void init() {};		// À‘•‚Í”CˆÓ
	virtual void update() = 0;	// À‘••K{
	virtual void draw() = 0;	// À‘••K{
	virtual void fin() {};		// À‘•‚Í”CˆÓ
};

