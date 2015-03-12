#ifndef CONTACTSOLVER_H
#define CONTACTSOLVER_H
#include "collisionfinder.h"
#include <vector>
class GraphicManager;
class ContactSolver
{
    int velIterations;
    int posIterations;
    GraphicManager* debugDrawer;
    bool debugDraw;

    void OneVelocitySolveIteration(std::vector<ContactConstraint> &allConstraints,std::vector<MyShape*>& shapes);
    bool OnePositionSolveIteration(std::vector<ContactConstraint> &allConstraints,std::vector<MyShape*>& shapes);
    void SolveVel(std::vector<ContactConstraint> &allConstraints,std::vector<MyShape*>& shapes);
    void SolvePos(std::vector<ContactConstraint> &allConstraints,std::vector<MyShape*>& shapes);
    void integratePosAndVel(std::vector<MyShape*>& shapes,double dt);

public:
    void turnOnDebugDraw();
    ContactSolver(int VelIterations,int PosIterations);

    void FindAndSolve(std::vector<MyShape*>& shapeList,double dt);
    void setDebugDrawer(GraphicManager* debugDrawer);

};

#endif // CONTACTSOLVER_H
