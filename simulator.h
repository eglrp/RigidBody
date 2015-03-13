#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <vector>
#include "MyShape.h"




class GraphicManager;
class Simulator
{
    int velIterations;
    int posIterations;
    bool debugDraw;
public:
    Simulator(int velocityIterations,int positionIterations);
    void solve(std::vector<MyShape*>& shapeList,double dt,GraphicManager* debugDrawer);
    void turnOnDebugDraw();
    void turnOffDebugDraw();

};

#endif // SIMULATOR_H
