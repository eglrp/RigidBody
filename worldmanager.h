#ifndef WORLDMANAGER_H
#define WORLDMANAGER_H
#include "algebra.h"
#include <vector>
#include <string>
#include "MyShape.h"
#include "shapemanager.h"
#include "simulator.h"
#include "graphics.h"
class WorldManager
{
    ShapeManager shapeManager;
    Simulator simulator;
    Vector2 gravity;
    GraphicManager* debugDrawer;
    double dt;
    double recommendFrameTime;
    bool debugDraw;

public:
    WorldManager();
    void OneStep();
    void readFile(std::string fileName);
    std::vector<MyShape*> getShapeList();
    void setDebugDrawer(GraphicManager* debugDrawer);
    ShapeManager* getShapeManager(){
        return &shapeManager;
    }
    void turnOnDebugDraw();
    void turnOffDebugDraw();

    double getRecommendFrameTime(){
        return recommendFrameTime;
    }
};

#endif // WORLDMANAGER_H
