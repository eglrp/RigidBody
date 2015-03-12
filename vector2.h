#ifndef VECTOR2_H
#define VECTOR2_H
class Vector2
{
public:
    double x,y;
    Vector2();
    Vector2(double,double);
    void normalize();
    inline const Vector2 operator+(const Vector2 &)const;
    inline Vector2& operator+=(const Vector2 &);
    inline Vector2& operator-=(const Vector2 &);
    inline const Vector2 operator-(const Vector2 &)const;
    inline const Vector2 operator-()const;
    inline const double operator*(const Vector2 &)const;
    inline const Vector2 operator/(double)const;
    inline Vector2& operator/=(double);
    inline friend const Vector2 operator*(const Vector2 &,double);
    inline friend const Vector2 operator*(double,const Vector2 &);
};
const Vector2 Vector2::operator +(const Vector2 &v)const{
    Vector2 r;
    r.x=x+v.x;
    r.y=y+v.y;
    return r;
}
Vector2& Vector2::operator+=(const Vector2 &v){
    x+=v.x;
    y+=v.y;
    return *this;
}
Vector2& Vector2::operator-=(const Vector2 &v){
    x-=v.x;
    y-=v.y;
    return *this;
}

const Vector2 Vector2::operator -(const Vector2 &v)const{
    Vector2 r;
    r.x=x-v.x;
    r.y=y-v.y;
    return r;
}
const Vector2 Vector2::operator-()const{
    Vector2 r;
    r.x=-x;r.y=-y;
    return r;
}

const double Vector2::operator*(const Vector2 &v)const{
    return x*v.x+y*v.y;
}

const Vector2 Vector2::operator/(double a)const{
    Vector2 r;
    r.x=x/a;r.y=y/a;
    return r;
}
inline Vector2& Vector2::operator/=(double a)
{
    x/=a;
    y/=a;
    return *this;
}
const Vector2 operator*(const Vector2 &v,double a){
    Vector2 r;
    r.x=v.x*a;r.y=v.y*a;
    return r;
}

const Vector2 operator*(double a,const Vector2 &v){
    Vector2 r;
    r.x=v.x*a;r.y=v.y*a;
    return r;
}

#endif // VECTOR2_H
