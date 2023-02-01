#pragma once
#include "GameScene.h"
#include "Object3d.h"
#include "ObjModel.h"
#include <memory>
class DDSLoadScene :
    public GameScene
{
    std::unique_ptr<Object3d> cubeObj;
    std::unique_ptr<ObjModel> cubeModel;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Light> light;

    Object3d::PipelineSet ppset;

public:
    DDSLoadScene();
    ~DDSLoadScene();

    void update() override;
    void drawObj3d() override;
};

