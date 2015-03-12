#ifndef MYSHAPE_H
#define MYSHAPE_H
#include <iostream>
#include <vector>
#include <string>
#include "algebra.h"


const double PI=3.141926536;
enum ShapeType{POLYGON,CIRCLE};
class AABB
{
public:
    double left, right, top, bottom;
};
class MyShape
{
public:
    MyShape(ShapeType Shapetype, Vector2 Position, double Mass, double Area);

    Vector2 center, vel, force;
    static int IDcount;
    int ID;
    ShapeType shapeType;
    double angle;
    double angVel; //angular velocity
    double torque;
    double mass;
    double area;
    double invMass;
    double I; //Moment of inertia
    double invI;
    double restitution;
    double friction;
    bool fixToGround;
    AABB aabb;
    virtual void move(Vector2 dx)=0;
    virtual void moveTo(Vector2 pos)=0;
    virtual void rotate(double degree)=0;
    virtual void draw()=0;
    virtual void updateVertex()=0;
    virtual void makeMove(double dt)=0;

    virtual void calc_AABB()=0;
private:
};

class MyCircle: public MyShape
{
public:
    MyCircle(double Radius, Vector2 Center, double Mass);

    double radius;

    void move(Vector2 dx);
    void moveTo(Vector2 pos);
    void rotate(double degree);
    void makeMove(double dt);
    void draw();
    void calc_AABB();
    void updateVertex(){}
private:

};

class MyPolygon: public MyShape
{
public:
    MyPolygon(double Mass, const std::vector<Vector2> &vList);

    int vertex_n;
    std::vector<Vector2> vertexLocal;//must be in the anticlockwise order
    std::vector<Vector2> vertex;
    std::vector<Vector2> normalLocal;
    std::vector<Vector2> normal;


    void move(Vector2 dx);
    void moveTo(Vector2 pos);
    void rotate(double degree);
    void makeMove(double dt);
    void draw();
    void calc_AABB();
    void updateVertex();



private:
    void calcCenterAndArea();
    void vertexRelativization();
    void calcVertex();
    void calcI();
};



bool TestAABBAABB(const AABB&, const AABB&);
bool PointInPolygon(const MyPolygon&, const Vector2&);
bool TriangleIsCCW(const Vector2&, const Vector2&, const Vector2&);
double ORIENT2D(const Vector2&, const Vector2&, const Vector2&);
int WhichSideIsNearest(const MyPolygon&, const Vector2&);


#endif

