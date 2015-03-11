#ifndef COLLISIONFINDER_H
#define COLLISIONFINDER_H
#include <vector>
#include "algebra.h"
#include "MyShape.h"
#include "matrix.h"
#include <iostream>

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
    bool shouldContact(const MyShape& a,const MyShape& b);
    bool SATtwoShape(const MyShape *a, const MyShape *b, Vector2& n, Vector2& point1, Vector2& point2, int& numOfContact, ContactConstraint::Type& contactType, Vector2 &facePoint);
    bool GetConstraint(int indexA, int indexB, const MyShape* pa, const MyShape* pb, ContactConstraint& constraint);
    bool SATPolygon(const MyPolygon *pa, const MyPolygon *pb, Vector2& n, Vector2& point1, Vector2& point2, int& numOfContact,ContactConstraint::Type& contactType,Vector2& facePoint);
    bool SATPolygonCircle(const MyPolygon *pa, const MyCircle *pb, Vector2& n, Vector2& point1, Vector2& point2, int& numOfContact,ContactConstraint::Type& contactType,Vector2& facePoint);
    void clipSegment(Vector2& l1,Vector2& l2,Vector2 clipPoint,Vector2 n);
    void getMinFromProjection(const std::vector<Vector2>& v,Vector2 n,double& min,std::vector<Vector2>::const_iterator& min_i);

public:
    std::vector<ContactConstraint>  FindCollisions(const std::vector<MyShape*>& shapes);
    CollisionFinder();
    ~CollisionFinder();
};

#endif // COLLISIONFINDER_H
