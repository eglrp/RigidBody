#include "myworld.h"
#include "contactsolver.h"
#include "collisionfinder.h"
#include "userinterface.h"
namespace World {
vector<MyShape*> ShapeList;
clock_t currentTime, lastTime,lastFrameTime;
double simulateSpeed=1;
double dt=0.02;
Vector2 gravityAcc(0.0,-9.8);
int framsBetweenDisplay=1;
clock_t frameTime=dt*double(CLOCKS_PER_SEC)/simulateSpeed;//除以模拟速度放大比例
Button buttonDrag,buttonPoly,buttonCircle;
bool debugMode=false;
}
using namespace World;
void stop_all_shape()
{
    for (vector<MyShape*>::iterator i=ShapeList.begin(); i<ShapeList.end(); i++) {
        (*i)->vel=Vector2(0,0);
        (*i)->angVel=0;
    }
    //cout<<"stop!"<<endl;
}
void myinit() {
    readFile(ShapeList);
    lastTime=lastFrameTime=clock();
    buttonDrag.state=false;
    buttonDrag.x=10;
    buttonDrag.y=10;
    buttonDrag.w=20;
    buttonDrag.h=20;
    buttonDrag.name="Drag";
    buttonPoly.state=false;
    buttonPoly.x=buttonDrag.x;
    buttonPoly.y=buttonDrag.y+30;
    buttonPoly.w=20;
    buttonPoly.h=20;
    buttonPoly.name="Poly";
    buttonCircle.x=buttonDrag.x;
    buttonCircle.y=buttonPoly.y+30;
    buttonCircle.w=20;
    buttonCircle.h=20;
    buttonCircle.name="Circle";
}
void oneStep() {

    for_each(ShapeList.begin(),ShapeList.end(),calc_force);
    for (vector<MyShape*>::iterator i=ShapeList.begin(); i<ShapeList.end(); i++) {
        (*i)->calc_AABB();
    }
    ContactSolver solver(10,10);
    CollisionFinder finder;
    vector<ContactConstraint> c;
    c=finder.FindCollisions(ShapeList);
    solver.SolveVel(c,ShapeList);
    eventTreatment();
    for (vector<MyShape*>::iterator i=ShapeList.begin(); i<ShapeList.end(); i++) {
        (*i)->makeMove(dt);
    }
    solver.SolvePos(c,ShapeList);
    for (vector<MyShape*>::iterator i=ShapeList.begin(); i<ShapeList.end(); i++) {
        (*i)->updateVertex();
    }


}
void calc_force(MyShape* s) {
    s->force.x=s->force.y=0;
    s->torque=0;
    s->force=s->force+gravityAcc*s->mass;
}


const Vector2 GetNearestPointToLine(const Vector2& c, const Vector2& a, const Vector2& b) {
    Vector2 ab=b-a;
    double t=((c-a)*ab)/(ab*ab);
    return a+t*ab;
}



