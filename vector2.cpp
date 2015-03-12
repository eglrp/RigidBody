#include "vector2.h"
#include <cmath>
Vector2::Vector2()
{
    x=y=0;
}
Vector2::Vector2(double a,double b)
{
    x=a;y=b;
}

void Vector2::normalize()
{
    double length=sqrt(x*x+y*y);
    x/=length;
    y/=length;
}



