#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <Imagine/Graphics.h>
#include "algebra.h"
#include <assert.h>
class MyShape;
class MyPolygon;
class MyCircle;
struct ViewLayout{
    int winWidth,winHeight;
    double viewZoneLeft,viewZoneRight,viewZoneUp,viewZoneBottom;
    double oneMeterLengthOnScreen;
    double viewWidth,viewHeight;
    double ratioWidth,ratioHeight;
    Vector2 viewCenter;
    ViewLayout(int windowWidth, int windowHeight);
    UpdateViewZone();
    void coordTranslate(double x,double y,int& x_onScreen,int& y_onScreen);
    void coordTranslateInverse(int x_onScreen, int y_onScreen, double& realX, double& realY);
};


class GraphicManager{

    ViewLayout viewLayout;

public:
    GraphicManager(int windowW, int windowH);
    void coordTranslate(double x,double y,int& x_onScreen,int& y_onScreen);
    void coordTranslateInverse(int x_onScreen, int y_onScreen, double& realX, double& realY);
    void myDisplay(std::vector<MyShape*> shapeList, Imagine::Color col=Imagine::BLACK);
    void draw_each(MyShape* s, Imagine::Color col=Imagine::BLACK, bool drawID=false);
    void drawAxis();
    void myDrawPoint(const Vector2&,int radius=2,Imagine::Color col=Imagine::RED);
    void myDrawPolygon(MyPolygon &p, Imagine::Color col=Imagine::BLACK, bool drawID=false);
    void myDrawCircle(MyCircle &p, Imagine::Color col=Imagine::BLACK, bool drawID=false);
    void myDrawLine(double x1, double y1, double x2, double y2, Imagine::Color col, int penWidth=1);
    void myNoRefreshPush();
    void myNoRefreshPop();
    void myMilliSleep(int milliSec);

};




#endif // GRAPHICS_H
