#include "simulator.h"


#include "contactsolver.h"



Simulator::Simulator(int velocityIterations, int positionIterations)
{
    velIterations=velocityIterations;
    posIterations=positionIterations;
}

void Simulator::solve(std::vector<MyShape*> &shapeList,double dt)
{
    ContactSolver solver(velIterations,posIterations);
    solver.FindAndSolve(shapeList,dt);
}


