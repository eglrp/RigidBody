#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <vector>
#include "MyShape.h"




class Simulator
{
    int velIterations;
    int posIterations;
public:
    Simulator(int velocityIterations,int positionIterations);
    void solve(std::vector<MyShape*>& shapeList,double dt);

};

#endif // SIMULATOR_H
