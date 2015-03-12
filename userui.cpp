#include "userui.h"
using namespace std;
MyShape*  FunDrag::findTargetShape(Vector2 v,std::vector<MyShape*>& shapeList){
    for (vector<MyShape*>::iterator i=shapeList.begin(); i<shapeList.end(); i++) {
        if((*i)->shapeType==POLYGON){
            if(PointInPolygon(*static_cast<const MyPolygon*>(*i),v))return *i;
        }
        if((*i)->shapeType==CIRCLE){
            if(PointInCircle(*static_cast<const MyCircle*>(*i),v))return *i;
        }
    }
    return 0;//NULL
}

void FunDrag::takeAction(){
    Imagine::Event ev;
    do{
        Imagine::getEvent(0,ev);
        if(ev.type==Imagine::EVT_BUT_ON&&ev.button==1){
            Vector2 v;
            drawer->coordTranslateInverse(ev.pix.x(),ev.pix.y(),v.x,v.y);
            std::vector<MyShape*> shapeList=shapeManager->getShapeList();
            pShape=findTargetShape(v,shapeList);
            if(pShape==0)continue;
            Vector2 c=pShape->center;
            double angle=-pShape->angle;
            Matrix2 m_rotate(cos(angle),-sin(angle),sin(angle),cos(angle));
            dragPointRelative=m_rotate*(v-c);
            mousePoint=v;
        }else if(ev.type==Imagine::EVT_MOTION&&pShape!=0){
            Imagine::Event ev1;
            Imagine::getEvent(0,ev1);
            while(ev1.type==Imagine::EVT_MOTION){
                ev=ev1;
                Imagine::getEvent(0,ev1);
            }
            if(ev1.type!=Imagine::EVT_NONE)Imagine::unGetEvent(ev1);
            Vector2 v;
            drawer->coordTranslateInverse(ev.pix.x(),ev.pix.y(),v.x,v.y);
            mousePoint=v;

        }else if(ev.type==Imagine::EVT_BUT_OFF){
            pShape=0;
        }
    }while(ev.type!=Imagine::EVT_NONE);
    if(pShape!=0){
        Vector2 c=pShape->center;
        double angle=pShape->angle;
        Matrix2 m_rotate(cos(angle),-sin(angle),sin(angle),cos(angle));
        Vector2 dragPointAbsolute=c+m_rotate*dragPointRelative;
        drawer->myDrawLine(dragPointAbsolute.x,dragPointAbsolute.y,mousePoint.x,mousePoint.y,Imagine::BLACK);
        Vector2 dragForce=10*(mousePoint-dragPointAbsolute)*pShape->mass;
        pShape->force+=dragForce;
        pShape->torque+=crossProd2D((dragPointAbsolute-c),dragForce);
    }
}
void FunPoly::takeAction(){
    Imagine::Event ev;
    double x,y;
    do{
        Imagine::getEvent(0,ev);
        if(ev.type==Imagine::EVT_BUT_ON&&ev.button==1){
            drawer->coordTranslateInverse(ev.pix.x(),ev.pix.y(),x,y);
            vList.push_back(Vector2(x,y));
        }else if(ev.type==Imagine::EVT_BUT_ON&&ev.button==3){
            ShapeDefinition sDef;
            sDef.shapeType=POLYGON;
            sDef.vertexList=vList;
            sDef.density=1;
            sDef.mass=0;//use density
            shapeManager->creatShape(sDef);
            vList.clear();
        }
    }while(ev.type!=Imagine::EVT_NONE);
    if(!vList.empty()){
        int xint1,yint1,xint2,yint2;
        for(std::vector<Vector2>::iterator i=vList.begin();i<vList.end()-1;i++){
            drawer->coordTranslate(i->x,i->y,xint1,yint1);
            drawer->coordTranslate((i+1)->x,(i+1)->y,xint2,yint2);
            drawer->myDrawLine(i->x,i->y,(i+1)->x,(i+1)->y,Imagine::RED);
            drawer->myDrawPoint(*i,2,Imagine::RED);
        }
    }
}

void UserUI::flushAllEvents()
{
    Imagine::Event ev;
    Imagine::getEvent(0,ev);
    while(ev.type!=Imagine::EVT_NONE)Imagine::getEvent(0,ev);
}

void UserUI::drawOneButton(Button b)
{
    if(b.state){
        Imagine::fillRect(b.x,b.y,b.w,b.h,Imagine::RED);
    }else{
        Imagine::fillRect(b.x,b.y,b.w,b.h,Imagine::BLUE);
    }
    Imagine::drawString(b.x,b.y,b.name,Imagine::BLACK);
}

UserUI::UserUI(ShapeManager* sM,GraphicManager* gM)
{
    shapeManager=sM;
    drawer=gM;
    Button buttonDrag;
    Button buttonPoly;
    Button buttonCircle;
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
    funPoly=FunPoly(buttonPoly);
    funDrag=FunDrag(buttonDrag);
    funCircle=FunCircle(buttonCircle);
    funPoly.setShapeManager(sM);
    funPoly.setGraphiqueManager(gM);
    funDrag.setShapeManager(sM);
    funDrag.setGraphiqueManager(gM);
    funCircle.setShapeManager(sM);
    funCircle.setGraphiqueManager(gM);
    currentAction=Free;
}


void UserUI::treatEvents()
{
    Imagine::Event ev;
    Imagine::getEvent(0,ev);
    if(ev.type==Imagine::EVT_BUT_ON&&ev.button==1){//按钮检测
        int x=ev.pix.x(),y=ev.pix.y();
        if(funDrag.buttonTouched(x,y)&&currentAction!=Drag){
            currentAction=Drag;
            funDrag.turnOn();funPoly.turnOff();funCircle.turnOff();
            return;
        }else if(funPoly.buttonTouched(x,y)&&currentAction!=Poly){
            currentAction=Poly;
            funPoly.turnOn();funDrag.turnOff();funCircle.turnOff();
            return;
        }else if(funCircle.buttonTouched(x,y)&&currentAction!=Circle){
            currentAction=Circle;
            funCircle.turnOn();funPoly.turnOff();funDrag.turnOff();
            return;
        }
    }
    Imagine::unGetEvent(ev);
    switch (currentAction) {//发送消息
    case Drag:
        funDrag.takeAction();
        break;
    case Poly:
        funPoly.takeAction();
        break;
    case Circle:
        funCircle.takeAction();
        break;
    default:
        break;
    }
    flushAllEvents();
}

void UserUI::drawButtons()
{
    drawOneButton(funDrag.getButton());
    drawOneButton(funPoly.getButton());
    drawOneButton(funCircle.getButton());
}



void Functionality::setShapeManager(ShapeManager *sM)
{
    shapeManager=sM;
}

void Functionality::setGraphiqueManager(GraphicManager *gM)
{
    drawer=gM;
}
