#ifndef WORLDMANAGER_H
#define WORLDMANAGER_H
#include "algebra.h"
#include <vector>
#include <string>
#include "MyShape.h"
#include "shapemanager.h"
#include "simulator.h"

class WorldManager
{
    ShapeManager shapeManager;
    Simulator simulator;
    Vector2 gravity;
    double dt;

public:
    WorldManager(double dt,Vector2 gravity=Vector2(0,0));
    void OneStep();
    void readFile(std::string fileName);
    std::vector<MyShape*> getShapeList();

};

#endif // WORLDMANAGER_H
