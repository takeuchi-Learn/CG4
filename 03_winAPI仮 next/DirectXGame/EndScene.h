#pragma once
#include "GameScene.h"
class EndScene :
    public GameScene {
public:
    void init() override;
    void update() override;
    void draw() override;
    void fin() override;
};

