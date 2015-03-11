#include "MyShape.h"
using namespace std;
int MyShape::IDcount=0;
double ORIENT2D(const Vector2& a, const Vector2& b, const Vector2& c)
{
    return (a.x-c.x)*(b.y-c.y)-(b.x-c.x)*(a.y-c.y);
}
bool TriangleIsCCW(const Vector2& a, const Vector2& b, const Vector2& c)
{
    return ORIENT2D(a,b,c)>0;
}
bool TestAABBAABB(const AABB& a, const AABB& b)
{
    if (a.left>b.right||a.right<b.left)
        return false;
    if (a.bottom>b.top||a.top<b.bottom)
        return false;
    return true;

}
bool PointInPolygon(const MyPolygon& s, const Vector2& p)
{
    int low=0, high=s.vertex_n;
    do
    {
        int mid=(low+high)/2;
        if (TriangleIsCCW(s.vertex[0],s.vertex[mid],p))
            low=mid;
        else
            high=mid;
    } while (low+1<high);
    if (low==0||high==s.vertex_n)
        return false;
    return TriangleIsCCW(s.vertex[low],s.vertex[high],p);
}
int WhichSideIsNearest(const MyPolygon &s, const Vector2& p)
{
    vector<Vector2 >::const_iterator i, j;
    const vector<Vector2 > &v=s.vertex;
    const int& n=s.vertex_n;
    double dist_min,dist;
    int min_num=n-1;
    dist=dist_min=SqDistPointLine(p,v[n-1],v[0]);
    for (i=v.begin(); i<v.end()-1; i++)
    {
        j=i+1;
        dist=SqDistPointLine(p,(*i),(*j));
        dist<dist_min?dist_min=dist,min_num=i-v.begin():1;
    }
    return min_num;
}



MyShape::MyShape(ShapeType Shapetype, Vector2 Position, double Mass, double Area) :
        angle(0)
{
    ID=IDcount;
    IDcount++;

    shapeType=Shapetype;
    center=Position;

    mass=Mass;
    area=Area;

    //by default
    force.x=0;force.y=0;
    torque=0;
    invMass=1/mass;
    angVel=0;
    vel=Vector2(0,0);

    restitution=0.5;
    friction=0.5;
    fixToGround=false;
}
void MyPolygon::move(Vector2 dx)
{
    center+=dx;
    calcVertex();
    calc_AABB();
}
void MyCircle::move(Vector2 dx)
{
    center+=dx;
}
void MyPolygon::moveTo(Vector2 pos)
{
    center=pos;
    calcVertex();
    calc_AABB();
}
void MyCircle::moveTo(Vector2 pos)
{
    center=pos;
}
void MyPolygon::rotate(double degree){
    angle+=degree;
    calcVertex();
    calc_AABB();
}
void MyCircle::rotate(double degree){
    angle+=degree;
}

MyCircle::MyCircle(double Radius, Vector2 Center, double Mass) :
        MyShape(CIRCLE,Center,Mass,3.1415927*Radius*Radius), radius(Radius)
{
    if(fixToGround){
        mass=0;invMass=0;I=0;invI=0;
    }else{
        I=Mass*Radius*Radius/2;invI=1/I;
    }

}
void MyCircle::draw()
{
    //draw
}
MyPolygon::MyPolygon(double Mass, const vector<Vector2> &vList) :
        MyShape(POLYGON,Vector2(0.0,0.0),Mass,0)
{
    vertex_n=vList.size();
    vertex.resize(vertex_n);
    vertexLocal=vList;
    calcCenterAndArea();

    vertexRelativization();
    calcI();

    calcVertex();

}
void MyPolygon::vertexRelativization()
{
    for (vector<Vector2>::iterator v=vertexLocal.begin(); v<vertexLocal.end();v++)
        *v-=center;
}
void MyPolygon::calcCenterAndArea()
{
    vector<Vector2 >::const_iterator v;
    int n=vertex_n;
    n-=2;
    double sumx=0, sumy=0, sumarea=0;
    double x[3], y[3];
    v=vertexLocal.begin();
    x[0]=(*v).x;
    y[0]=(*v).y;
    v++;
    x[1]=(*v).x;
    y[1]=(*v).y;
    while (n--)
    {
        v++;
        x[2]=(*v).x;
        y[2]=(*v).y;
        double s=((x[1]-x[0])*(y[2]-y[0])-(x[2]-x[0])*(y[1]-y[0]))/2.0;
        sumx+=((x[0]+x[1]+x[2])*s);
        sumy+=((y[0]+y[1]+y[2])*s);
        sumarea+=s;

        x[1]=x[2];
        y[1]=y[2];
    }
    area=sumarea;
    center=Vector2(sumx/sumarea/3,sumy/sumarea/3);
}
void MyPolygon::draw()
{

//draw

}
void MyPolygon::calcVertex()
{
    /*
    Transform trans(angle,center);
    vector<Vector2 >::iterator vL,v;

    for (vL=vertexLocal.begin(),v=vertex.begin();vL<vertexLocal.end();++vL,++v)
        *v=(trans.Apply(*vL));
    */
    Matrix2 m_rotate(cos(angle),-sin(angle),sin(angle),cos(angle));
        Vector2 t;
        int i=0;
        vector<Vector2 >::iterator v;

        for (v=vertexLocal.begin(); v<vertexLocal.end(); ++v)
        {
            t=m_rotate*(*v);

            vertex[i]=t+center;
            ++i;

        }
}
void MyPolygon::calcI()
{
    if(fixToGround){
        I=0;invI=0;return;
    }
    double sum1=0, sum2=0, temp;
    int j;
    vector<Vector2>&v=vertexLocal;
    for (int i=0; i<vertex_n; i++)
    {
        j=(i+1)%vertex_n;
        temp=v[i].x*v[j].y-v[j].x*v[i].y;
        sum1+=temp*(v[j]*v[j]+v[j]*v[i]+v[i]*v[i]);
        sum2+=temp;
    }
    area=sum2;
    I=mass/6*sum1/sum2;
    invI=1/I;
}
void MyPolygon::makeMove(double dt)
{
    if(fixToGround)return;
    Vector2 acc=force*invMass;
    double ang_acc=torque*invI;
    vel+=acc*dt/2;
    center+=vel*dt;
    vel+=acc*dt/2;
    angVel+=ang_acc*dt/2;
    angle+=angVel*dt;
    angVel+=ang_acc*dt/2;
    calcVertex();
}
void MyCircle::makeMove(double dt){
    if(fixToGround)return;
    Vector2 acc=force*invMass;
    double ang_acc=torque*invI;
    vel+=acc*dt/2;
    center+=vel*dt;
    vel+=acc*dt/2;
    angVel+=ang_acc*dt/2;
    angle+=angVel*dt;
    angVel+=ang_acc*dt/2;
}

void MyPolygon::calc_AABB()
{
    vector<Vector2 >::iterator i=vertex.begin();
    aabb.left=aabb.right=(*i).x;
    aabb.top=aabb.bottom=(*i).y;
    for (; i<vertex.end(); i++)
    {
        aabb.right=(*i).x>aabb.right ? (*i).x : aabb.right;
        aabb.left=(*i).x<aabb.left ? (*i).x : aabb.left;
        aabb.top=(*i).y>aabb.top ? (*i).y : aabb.top;
        aabb.bottom=(*i).y<aabb.bottom ? (*i).y : aabb.bottom;
    }
}

void MyPolygon::updateVertex()
{
    calcVertex();
}
void MyCircle::calc_AABB()
{
    aabb.left=center.x-radius;
    aabb.right=center.x+radius;
    aabb.top=center.y+radius;
    aabb.bottom=center.y-radius;
}
