#ifndef COLLISIONFINDER_H
#define COLLISIONFINDER_H
#include <vector>
#include "algebra.h"
#include "MyShape.h"
#include <iostream>
class GraphicManager;
struct ContactPoint{
    Vector2 position;
    Vector2 rA,rB;
    double normalMass;
    double tangentMass;
    double normalImpulse;
    double tangentImpulse;
    double separatingVel;

};
struct ContactConstraint{
    ContactPoint points[2];
    int pointCount;// 1 or 2
    int shapeIndexA;
    int shapeIndexB;
    double invMassA, invMassB;
    double invIA,invIB;
    double friction;
    double restitution;
    Vector2 normal;
    Matrix2 A;
    Matrix2 invA;
    //for position correction:
    enum Type{circles,faceA,faceB};
    Type type;
    Vector2 localFacePoint;
    Vector2 localContact[2];
    Vector2 localNormal;
};



class CollisionFinder
{
    GraphicManager* debugDrawer;
    bool debugDraw;
    bool shouldContact(const MyShape& a,const MyShape& b);
    bool SATtwoShape(MyShape *a, MyShape *b, Vector2& n, Vector2& contactPoint1, Vector2& contactPoint2, int& numOfContact, ContactConstraint::Type& contactType, Vector2 &facePoint);
    bool GetConstraint(int indexA, int indexB, MyShape *pa, MyShape *pb, ContactConstraint& constraint);
    bool SATPolygon(MyPolygon *pa, MyPolygon *pb, Vector2& n, Vector2& contactPoint1, Vector2& contactPoint2, int& numOfContact, ContactConstraint::Type& contactType, Vector2& facePoint);
    bool SATCircle(MyCircle *pa, MyCircle *pb,Vector2& n, Vector2& contactPoint);
    bool SATPolygonCircle(MyPolygon *pa, MyCircle *pb, Vector2& n, Vector2& contactPoint, Vector2& facePoint);
    void clipSegment(Vector2& l1,Vector2& l2,Vector2 clipPoint,Vector2 n);
    void getMinFromProjection(const std::vector<Vector2>& v,Vector2 n,double& min,std::vector<Vector2>::const_iterator& min_i);

public:
    CollisionFinder();
    void turnOnDebugDraw();

    std::vector<ContactConstraint>  FindCollisions(std::vector<MyShape *> &shapes);
    void setDebugDrawer(GraphicManager *debugDrawer);

};

#endif // COLLISIONFINDER_H
