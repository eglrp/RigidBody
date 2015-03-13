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
    std::string description;
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
    Vector2 center;
    bool centerCreated;
public:
    FunCircle(Button button):Functionality(button){centerCreated=false;}
    FunCircle(){}
    void start(){}
    void terminate(){}
    void takeAction();
};
class FunView:public Functionality{
private:
    Vector2 mousePoint;
    int lastY;
    bool moving;
    bool zooming;
public:
    FunView(Button button):Functionality(button){
        moving=false;
        zooming=false;
    }
    FunView(){}
    void start(){}
    void terminate(){}
    void takeAction();
};
class FunDelete:public Functionality{
private:
    MyShape* findTargetShape(Vector2 v, std::vector<MyShape *> &shapeList);
public:
    FunDelete(Button button):Functionality(button){}
    FunDelete(){}
    void start(){}
    void terminate(){}
    void takeAction();
};
class FunDebug:public Functionality{
private:
    bool debugMode;
    bool turnOffDebugAfterOneFrame;
    WorldManager* wM;
public:
    void setWorldManager(WorldManager* wM);
    FunDebug(Button button):Functionality(button){debugMode=false;turnOffDebugAfterOneFrame=true;}
    FunDebug(){}
    void start(){}
    void terminate(){}
    void takeAction();
    void autoTurnOff();
};

class UserUI
{
    enum CurrentAction{Free,Drag,Poly,Circle,View,Delete};
    CurrentAction currentAction;
    ShapeManager* shapeManager;
    GraphicManager* drawer;
    WorldManager* worldManager;
    FunDrag funDrag;
    FunPoly funPoly;
    FunCircle funCircle;
    FunView funView;
    FunDelete funDelete;
    FunDebug funDebug;
    void flushAllEvents();
    void drawOneButton(Button b);
public:

    UserUI(ShapeManager* sM,GraphicManager* gM,WorldManager* wM);

    void treatEvents();
    void drawButtons();
};

#endif // USERUI_H
