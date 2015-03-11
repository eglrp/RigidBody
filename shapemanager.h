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
    double rotate, angVel, mass, restitution,friction,radius;

    ShapeDefinition();
};

class ShapeManager
{
    int shapeCount;
    std::list<MyShape*> shapeList;

public:
    ShapeManager();
    std::vector<MyShape*> getShapeList();
    MyPolygon* creatPoly(const ShapeDefinition& shapeDef);
    MyCircle* creatCircle(const ShapeDefinition& shapeDef);
    void moveAllShape(double dt);
    void applyGravity(Vector2 gravity);
    void clearAllForce();
    ~ShapeManager();
};

#endif // SHAPEMANAGER_H
