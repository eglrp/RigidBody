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


    static int IDcount;
    int ID;
    ShapeType shapeType;

    //cinematic
    Vector2 center;
    double angle;

    double radius;//for polygon = 0

    Vector2 vel;
    double angVel;

    //dynamic
    Vector2 force;
    double torque;

    //property
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
    MyCircle(double Radius, Vector2 Center, double Mass, double Density);

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
    MyPolygon(const std::vector<Vector2> &vList, double Mass, double Density);

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
    void updateNormal();


private:
    void calcNormalLocal();
    void calcCenterAndArea();
    void vertexRelativization();
    void calcVertex();
    void calcI();
};



bool TestAABBAABB(const AABB&, const AABB&);
bool PointInPolygon(const MyPolygon&, const Vector2&);
bool PointInCircle(const MyCircle&c, const Vector2&v);
bool TriangleIsCCW(const Vector2&, const Vector2&, const Vector2&);
double ORIENT2D(const Vector2&, const Vector2&, const Vector2&);
int WhichSideIsNearest(const MyPolygon&, const Vector2&);


#endif

