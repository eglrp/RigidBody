#ifndef SHAPEMANAGER_H
#define SHAPEMANAGER_H
#include <list>
#include <vector>
#include "MyShape.h"
#include <assert.h>
struct ShapeDefinition{
    ShapeType shapeType;
    std::vector<Vector2 > vertexList;
    Vector2 move, velocity,circleCenter;
    bool fixToGroud;
    double rotate, angVel, mass,density, restitution,friction,radius;

    ShapeDefinition();
};

class ShapeManager
{
    int shapeCount;
    std::list<MyShape*> shapeList;
    void creatPoly(const ShapeDefinition& shapeDef);
    void creatCircle(const ShapeDefinition& shapeDef);
public:
    ShapeManager();
    std::vector<MyShape*> getShapeList();
    void creatShape(const ShapeDefinition& sDef);
    void moveAllShape(double dt);
    void applyGravity(Vector2 gravity);
    void clearAllForce();
    ~ShapeManager();
};

#endif // SHAPEMANAGER_H
