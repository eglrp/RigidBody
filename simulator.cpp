#include "simulator.h"
#include "contactsolver.h"
#include "graphics.h"


Simulator::Simulator(int velocityIterations, int positionIterations)
{
    velIterations=velocityIterations;
    posIterations=positionIterations;
    debugDraw=false;
}

void Simulator::solve(std::vector<MyShape*> &shapeList, double dt, GraphicManager *debugDrawer)
{
    ContactSolver solver(velIterations,posIterations);
    if(debugDraw){
        solver.turnOnDebugDraw();
    }
    solver.setDebugDrawer(debugDrawer);
    solver.FindAndSolve(shapeList,dt);
}

void Simulator::turnOnDebugDraw()
{
    debugDraw=true;
}


