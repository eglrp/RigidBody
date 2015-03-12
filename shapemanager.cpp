#include "shapemanager.h"

ShapeManager::ShapeManager()
{
    shapeCount=0;
}

std::vector<MyShape *> ShapeManager::getShapeList()
{
    std::vector<MyShape *> shapeVector;
    for(std::list<MyShape*>::iterator i=shapeList.begin();i!=shapeList.end();i++){
        shapeVector.push_back(*i);
    }
    return shapeVector;
}

void ShapeManager::creatShape(const ShapeDefinition &sDef)
{
    switch (sDef.shapeType) {
    case POLYGON:
        creatPoly(sDef);
        break;
    case CIRCLE:
        creatCircle(sDef);
        break;
    default:
        exit(0);
    }
}

void ShapeManager::creatPoly(const ShapeDefinition& shapeDef)
{
    if(shapeDef.vertexList.size()<3)return;
    assert(shapeDef.mass>0||shapeDef.density>0);
    assert(shapeDef.friction>=0);
    assert(shapeDef.restitution>0);
    assert(shapeDef.shapeType==POLYGON);
    MyPolygon* p=new MyPolygon(shapeDef.vertexList,shapeDef.mass,shapeDef.density);
    p->move(shapeDef.move);
    p->angVel=shapeDef.angVel;
    p->rotate(shapeDef.rotate);
    p->vel=shapeDef.velocity;
    p->restitution=shapeDef.restitution;
    p->friction=shapeDef.friction;
    if(shapeDef.fixToGroud){
        p->fixToGround=true;
        p->mass=0;
        p->invMass=0;
        p->I=0;
        p->invI=0;
        p->vel=Vector2(0,0);
    }
    shapeList.push_back(p);
}

void ShapeManager::creatCircle(const ShapeDefinition &shapeDef)
{
    assert(shapeDef.mass>0||shapeDef.density>0);
    assert(shapeDef.friction>=0);
    assert(shapeDef.restitution>0);
    assert(shapeDef.radius>0);
    assert(shapeDef.shapeType==CIRCLE);
    MyCircle* p=new MyCircle(shapeDef.radius,shapeDef.circleCenter,shapeDef.mass,shapeDef.density);
    p->move(shapeDef.move);
    p->angVel=shapeDef.angVel;
    p->rotate(shapeDef.rotate);
    p->vel=shapeDef.velocity;
    p->restitution=shapeDef.restitution;
    p->friction=shapeDef.friction;
    if(shapeDef.fixToGroud){
        p->fixToGround=true;
        p->mass=0;
        p->invMass=0;
        p->I=0;
        p->invI=0;
        p->vel=Vector2(0,0);
    }
    shapeList.push_back(p);
}

void ShapeManager::moveAllShape(double dt)
{
    for(std::list<MyShape*>::iterator i=shapeList.begin();i!=shapeList.end();i++){
        MyShape* p=*i;
        p->makeMove(dt);
    }
}

void ShapeManager::applyGravity(Vector2 gravity)
{
    for(std::list<MyShape*>::iterator i=shapeList.begin();i!=shapeList.end();i++){
        MyShape* p=*i;
        p->force+=p->mass*gravity;
    }
}

void ShapeManager::clearAllForce()
{
    for(std::list<MyShape*>::iterator i=shapeList.begin();i!=shapeList.end();i++){
        MyShape* p=*i;
        p->force=Vector2(0,0);
        p->torque=0;
    }
}

ShapeManager::~ShapeManager()
{
    for(std::list<MyShape*>::iterator i=shapeList.begin();i!=shapeList.end();i++){
        MyShape* p=*i;
        assert(p->shapeType==POLYGON||p->shapeType==CIRCLE);
        switch (p->shapeType) {
        case POLYGON:
        {
            p=static_cast<MyPolygon*>(p);
            delete p;
            break;
        }
        case CIRCLE:
        {
            p=static_cast<MyCircle*>(p);
            delete p;
            break;
        }

        }
    }
}





ShapeDefinition::ShapeDefinition()
{
    move=Vector2(0,0);
    velocity=Vector2(0,0);
    move=Vector2(0,0);
    circleCenter=Vector2(0,0);
    fixToGroud=false;
    rotate=0;
    angVel=0;
    mass=1;
    restitution=0.5;
    friction=0.5;
    radius=1;
    density=0;
}
