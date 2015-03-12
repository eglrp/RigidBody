#include "graphics.h"
#include "MyShape.h"
#include "algebra.h"
#include <Imagine/Graphics.h>
#include <iostream>
#include <sstream>
//#include <sstream>

using namespace std;
void ViewLayout::coordTranslate(double x,double y,int& x_onScreen,int& y_onScreen){
    x_onScreen=(x-viewZoneLeft)*ratioWidth;
    y_onScreen=(viewZoneUp-y)*ratioHeight;
}
void ViewLayout::coordTranslateInverse(int x_onScreen, int y_onScreen, double& realX,double& realY)
{
    realX=x_onScreen/ratioWidth+viewZoneLeft;
    realY=viewZoneUp-y_onScreen/ratioHeight;
}

GraphicManager::GraphicManager(int windowW,int windowH):
    viewLayout(windowW,windowH)
{

}

void GraphicManager::coordTranslate(double x, double y, int &x_onScreen, int &y_onScreen)
{
    viewLayout.coordTranslate(x,y,x_onScreen,y_onScreen);
}

void GraphicManager::coordTranslateInverse(int x_onScreen, int y_onScreen, double &realX, double &realY)
{
    viewLayout.coordTranslateInverse(x_onScreen,y_onScreen,realX,realY);
}

void GraphicManager::myDisplay(std::vector<MyShape *> shapeList, Imagine::Color col)
{
    Imagine::noRefreshPush();
    Imagine::setBackGround(Imagine::WHITE);
    drawAxis();
//    if(DebugDrawing){
//        for(vector<MyShape*>::iterator i=shapeList.begin();i<shapeList.end();i++){
//            MyShape* s=*i;
//            switch (s->shapeType) {
//            case POLYGON:
//            {
//                MyPolygon* sP=static_cast<MyPolygon*>(s);
//                MyPolygon tempPoly(*sP);
//                tempPoly.makeMove(dt);
//                draw_each(&tempPoly,Imagine::RED,false);
//                break;
//            }
//            case CIRCLE:
//            {
//                MyCircle* sP=static_cast<MyCircle*>(s);
//                MyCircle tempCircle(*sP);
//                tempCircle.makeMove(dt);
//                draw_each(&tempCircle,Imagine::RED,false);
//            }
//                break;
//            default:
//                break;
//            }
//        }
//    }
    for(vector<MyShape*>::iterator i=shapeList.begin();i<shapeList.end();i++){
        draw_each(*i,col,true);
    }
    Imagine::noRefreshPop();
}
void GraphicManager::draw_each(MyShape* s,Imagine::Color col,bool drawID)
{
    switch (s->shapeType) {
    case POLYGON:
        myDrawPolygon(*static_cast<MyPolygon*>(s),col,drawID);
        break;
    case CIRCLE:
        myDrawCircle(*static_cast<MyCircle*>(s),col,drawID);
        break;
    default:
        break;
    }

}
void GraphicManager::drawAxis()
{
    int i,j;
    int n=5;
    for(i=-n;i<=n;i++){
        myDrawLine(i,-n,i,n,Imagine::Color(200,200,200));
    }
    for(j=-n;j<=n;j++){
        myDrawLine(-n,j,n,j,Imagine::Color(200,200,200));
    }
    myDrawLine(0,-n,0,n,Imagine::Color(200,200,200));
    myDrawLine(-n,0,n,0,Imagine::Color(200,200,200));
}
void GraphicManager::myDrawPoint(const Vector2& p, int radius,Imagine::Color col) {

    int x,y;
    viewLayout.coordTranslate(p.x,p.y,x,y);
    Imagine::fillCircle(x,y,radius,col);
}
void GraphicManager::myDrawPolygon(MyPolygon &p, Imagine::Color col,bool drawID){

    assert(p.shapeType==POLYGON);
    vector<Vector2 >::iterator v;
    for (v=p.vertex.begin(); v<p.vertex.end()-1; ++v)
        myDrawLine((*v).x,(*v).y,(*(v+1)).x,(*(v+1)).y, col);

    myDrawLine(p.vertex.back().x,p.vertex.back().y,p.vertex.front().x,p.vertex.front().y, col);

    if(drawID){
        std::stringstream ss;
        ss<<p.ID;
        int xx1,yy1;
        viewLayout.coordTranslate(p.center.x,p.center.y,xx1,yy1);
        Imagine::drawString(xx1,yy1,(ss.str()),col);
    }
}

void GraphicManager::myDrawCircle(MyCircle &p, Imagine::Color col,bool drawID){
    if(p.shapeType!=CIRCLE){cout<<"drawCircle need circle"<<endl;exit(0);}
    int xx1,yy1,xx2,yy2,r;
    Vector2 pointOnCircle;
    Transform trans(p.angle,p.center);
    pointOnCircle=trans.Apply(Vector2(p.radius,0));
    viewLayout.coordTranslate(p.center.x,p.center.y,xx1,yy1);
    viewLayout.coordTranslate(pointOnCircle.x,pointOnCircle.y,xx2,yy2);
    r=p.radius*viewLayout.oneMeterLengthOnScreen;

    Imagine::drawCircle(xx1,yy1,r,col);
    Imagine::drawLine(xx1,yy1,xx2,yy2,col);
    if(drawID){
        std::stringstream ss;
        ss<<p.ID;
        int xx1,yy1;
        viewLayout.coordTranslate(p.center.x,p.center.y,xx1,yy1);
        Imagine::drawString(xx1,yy1,(ss.str()),col);
    }
}
void GraphicManager::myDrawLine(double x1,double y1,double x2,double y2, Imagine::Color col,int penWidth){
    int xx1,xx2,yy1,yy2;
    viewLayout.coordTranslate(x1,y1,xx1,yy1);
    viewLayout.coordTranslate(x2,y2,xx2,yy2);
    Imagine::drawLine(xx1,yy1,xx2,yy2,col,penWidth);
}

void GraphicManager::myNoRefreshPush()
{
    Imagine::noRefreshPush();
}

void GraphicManager::myNoRefreshPop()
{
    Imagine::noRefreshPop();
}

void GraphicManager::myMilliSleep(int milliSec)
{
    Imagine::milliSleep(milliSec);
}





ViewLayout::ViewLayout(int windowWidth,int windowHeight)
{
    oneMeterLengthOnScreen=75;
    winWidth=windowWidth;
    winHeight=windowHeight;
    viewCenter=Vector2(0,0);
    UpdateViewZone();
}

ViewLayout::UpdateViewZone()
{
    viewZoneLeft    =-winWidth/2/oneMeterLengthOnScreen+viewCenter.x;
    viewZoneRight   =winWidth/2/oneMeterLengthOnScreen+viewCenter.x;
    viewZoneUp      =winHeight/2/oneMeterLengthOnScreen+viewCenter.y;
    viewZoneBottom  =-winHeight/2/oneMeterLengthOnScreen+viewCenter.y;
    viewWidth=viewZoneRight-viewZoneLeft;
    viewHeight=viewZoneUp-viewZoneBottom;
    ratioWidth=winWidth/viewWidth;
    ratioHeight=winHeight/viewHeight;
}



