#ifndef ALGEBRA_H
#define ALGEBRA_H

#include "matrix2.h"
#include "vector2.h"
#include <cmath>
const Vector2 operator*(const Matrix2&,const Vector2&);
const Vector2 operator*(const Vector2&,const Matrix2&);
inline void rotate90clockwise(Vector2 &v)
{
    double t=v.x;
    v.x=v.y;v.y=-t;
}
inline void rotate90anticlockwise(Vector2 &v)
{
    double t=v.x;
    v.x=-v.y;v.y=t;
}
inline double crossProd2D(const Vector2 &a, const Vector2 &b)
{
    return a.x*b.y-a.y*b.x;
}
inline Vector2 crossProd2D(double s, const Vector2 &a)
{
    return Vector2(-s*a.y,s*a.x);
}
inline Vector2 crossProd2D(const Vector2&a, double s)
{
    return Vector2(s*a.y,-s*a.x);
}
inline double SqDistPointLine(const Vector2& p, const Vector2& a1,
        const Vector2& a2){
    return (p-a1)*(p-a1)-((p-a1)*(a2-a1))*((p-a1)*(a2-a1))/((a2-a1)*(a2-a1));
}
struct Rotation{
    double ssin;
    double ccos;
    Rotation(double angle){
        ssin=sin(angle);
        ccos=cos(angle);
    }
    Vector2 Apply(const Vector2& point){
        Vector2 v;
        v.x=ccos*point.x-ssin*point.y;
        v.y=ssin*point.x+ccos*point.y;
        return v;
    }
    Vector2 UnApply(const Vector2& point){
        Vector2 v;
        v.x=ccos*point.x+ssin*point.y;
        v.y=-ssin*point.x+ccos*point.y;
        return v;
    }
};
struct Transform{
    Rotation r;
    Vector2 p;
    Transform(double angle,const Vector2& pp):r(Rotation(angle)),p(pp){

    }
    Vector2 Apply(const Vector2& point){
        Vector2 v;
        v.x=r.ccos*point.x-r.ssin*point.y+p.x;
        v.y=r.ssin*point.x+r.ccos*point.y+p.y;
        return v;
    }
    Vector2 UnApply(const Vector2& point){
        Vector2 v;
        double x=point.x-p.x;
        double y=point.y-p.y;
        v.x=r.ccos*x+r.ssin*y;
        v.y=-r.ssin*x+r.ccos*y;
        return v;
    }
};

#endif // ALGEBRA_H
