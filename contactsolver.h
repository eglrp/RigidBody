#ifndef CONTACTSOLVER_H
#define CONTACTSOLVER_H
#include "collisionfinder.h"
#include <vector>
class ContactSolver
{
    int velIterations;
    int posIterations;
    void OneVelocitySolveIteration(std::vector<ContactConstraint> &allConstraints,std::vector<MyShape*>& shapes);
    void OnePositionSolveIteration(std::vector<ContactConstraint> &allConstraints,std::vector<MyShape*>& shapes);
public:
    void SolveVel(std::vector<ContactConstraint> &allConstraints,std::vector<MyShape*>& shapes);
    void SolvePos(std::vector<ContactConstraint> &allConstraints,std::vector<MyShape*>& shapes);
    ContactSolver(int VelIterations,int PosIterations);
    ~ContactSolver();
};

#endif // CONTACTSOLVER_H
