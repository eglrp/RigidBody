#ifndef USERUI_H
#define USERUI_H
#include "MyShape.h"
#include <vector>
#include <Imagine/Graphics.h>
#include "graphics.h"
#include "worldmanager.h"
#include <string>


struct Button{
    int x,y,w,h;
    bool state;
    std::string name;
};
class Functionality{
private:
    Button button;

protected:
    ShapeManager* shapeManager;
    GraphicManager* drawer;
public:
    Functionality(Button b){button=b;}
    Functionality(){}
    void setShapeManager(ShapeManager* sM);
    void setGraphiqueManager(GraphicManager* gM);
    void turnOn(){button.state=true;start();}
    void turnOff(){button.state=false;terminate();}
    virtual void terminate()=0;
    virtual void start()=0;
    virtual void takeAction()=0;
    bool buttonTouched(int x,int y){
        return x>=button.x&&y>button.y&&x<=button.x+button.w-1&&y<=button.y+button.h-1;
    }
    Button getButton(){
        return button;
    }

};
class FunDrag:public Functionality{
private:
    MyShape *pShape;
    Vector2 dragPointRelative;
    Vector2 mousePoint;
    MyShape* findTargetShape(Vector2 v, std::vector<MyShape *> &shapeList);

public:
    FunDrag(Button button):Functionality(button){pShape=0;}
    FunDrag(){}
    void start(){}
    void terminate(){pShape=0;}
    void takeAction();
};
class FunPoly:public Functionality{
private:
    std::vector<Vector2> vList;
public:
    FunPoly(Button button):Functionality(button){}
    FunPoly(){}
    void start(){vList.clear();}
    void terminate(){vList.clear();}
    void takeAction();


};
class FunCircle:public Functionality{
private:
public:
    FunCircle(Button button):Functionality(button){}
    FunCircle(){}
    void start(){}
    void terminate(){}
    void takeAction(){}
};

class UserUI
{
    enum CurrentAction{Free,Drag,Poly,Circle};
    CurrentAction currentAction;
    ShapeManager* shapeManager;
    GraphicManager* drawer;

    FunDrag funDrag;
    FunPoly funPoly;
    FunCircle funCircle;
    void flushAllEvents();
    void drawOneButton(Button b);
public:

    UserUI(ShapeManager* sM,GraphicManager* gM);

    void treatEvents();
    void drawButtons();
};

#endif // USERUI_H
