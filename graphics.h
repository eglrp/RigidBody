#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <Imagine/Graphics.h>
class MyShape;
class Vector2;
class MyPolygon;
class MyCircle;
void myDisplay(bool DebugDrawing=false,Imagine::Color col=Imagine::BLACK);
void initialView(int width,int height);
void draw_each(MyShape* s, Imagine::Color col=Imagine::BLACK, bool drawID=false);
void drawAxis();
void myDrawPoint(const Vector2&,int radius=2,Imagine::Color col=Imagine::RED);
void myDrawPolygon(MyPolygon &p, Imagine::Color col=Imagine::BLACK, bool drawID=false);
void myDrawCircle(MyCircle &p, Imagine::Color col=Imagine::BLACK, bool drawID=false);
void myDrawLine(double x1, double y1, double x2, double y2, Imagine::Color col, int penWidth=1);
void coordTranslate(double x,double y,int& x_onScreen,int& y_onScreen);
void coordTranslateInverse(int x_onScreen, int y_onScreen, double& realX, double& realY);
#endif // GRAPHICS_H
