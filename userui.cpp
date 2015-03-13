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
MyShape *FunDelete::findTargetShape(Vector2 v, std::vector<MyShape *> &shapeList)
{
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
        if(vList.size()==1){
            drawer->myDrawPoint(vList[0],2,Imagine::RED);
        }
    }
}
void FunCircle::takeAction(){
    Imagine::Event ev;
    double x,y;
    do{
        Imagine::getEvent(0,ev);
        if(ev.type==Imagine::EVT_BUT_ON&&ev.button==1){
            centerCreated=true;
            drawer->coordTranslateInverse(ev.pix.x(),ev.pix.y(),x,y);
            center=Vector2(x,y);
        }else if(ev.type==Imagine::EVT_BUT_ON&&ev.button==3){
            centerCreated=false;
            drawer->coordTranslateInverse(ev.pix.x(),ev.pix.y(),x,y);
            Vector2 point;
            point.x=x;point.y=y;
            double r=sqrt((point-center)*(point-center));
            ShapeDefinition sDef;
            sDef.shapeType=CIRCLE;
            sDef.density=1;
            sDef.circleCenter=center;
            sDef.radius=r;
            sDef.mass=0;//use density
            shapeManager->creatShape(sDef);
        }
    }while(ev.type!=Imagine::EVT_NONE);
    if(centerCreated){
        drawer->myDrawPoint(center,2,Imagine::RED);
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
        Imagine::drawRect(b.x,b.y,b.w,b.h,Imagine::RED,3);
    }else{
        Imagine::drawRect(b.x,b.y,b.w,b.h,Imagine::BLUE,3);
    }
    Imagine::drawString(b.x+b.w/2-24,b.y+b.h/2+5,b.name,Imagine::RED);
    Imagine::drawString(b.x,b.y+b.h+15,b.description,Imagine::BLACK);
}

UserUI::UserUI(ShapeManager* sM, GraphicManager* gM, WorldManager *wM)
{
    shapeManager=sM;
    worldManager=wM;
    drawer=gM;
    Button buttonDrag;
    Button buttonPoly;
    Button buttonCircle;
    Button buttonView;
    Button buttonDelete;
    Button buttonDebug;
    int xx=10;
    int w=100,h=40;
    buttonDrag.state=false;
    buttonDrag.x=xx;
    buttonDrag.y=10;
    buttonDrag.w=w;
    buttonDrag.h=h;
    buttonDrag.name="Drag";
    buttonDrag.description="Click on shape and drag mouse";

    buttonPoly.state=false;
    buttonPoly.x=xx;
    buttonPoly.y=buttonDrag.y+h+30;
    buttonPoly.w=w;
    buttonPoly.h=h;
    buttonPoly.name="Poly";
    buttonPoly.description="LeftClick to creat vertex, RIGHT CLICK to finish";

    buttonCircle.state=false;
    buttonCircle.x=xx;
    buttonCircle.y=buttonPoly.y+h+30;
    buttonCircle.w=w;
    buttonCircle.h=h;
    buttonCircle.name="Circle";
    buttonCircle.description="LeftClick to creat center, RIGHT CLICK to finish";


    buttonView.state=false;
    buttonView.x=xx;
    buttonView.y=buttonCircle.y+h+30;
    buttonView.w=w;
    buttonView.h=h;
    buttonView.name="View";
    buttonView.description="LeftDrag to move, Right Drag to zoom";

    buttonDelete.state=false;
    buttonDelete.x=xx;
    buttonDelete.y=buttonView.y+h+30;
    buttonDelete.w=w;
    buttonDelete.h=h;
    buttonDelete.name="Delete";
    buttonDelete.description=" ";

    buttonDebug.state=false;
    buttonDebug.x=xx;
    buttonDebug.y=buttonDelete.y+h+30;
    buttonDebug.w=w;
    buttonDebug.h=h;
    buttonDebug.name="Debug Draw";
    buttonDebug.description="Turn on debug drawing for one frame";

    funPoly=FunPoly(buttonPoly);
    funDrag=FunDrag(buttonDrag);
    funCircle=FunCircle(buttonCircle);
    funView=FunView(buttonView);
    funDelete=FunDelete(buttonDelete);
    funDebug=FunDebug(buttonDebug);


    funPoly.setShapeManager(sM);
    funPoly.setGraphiqueManager(gM);
    funDrag.setShapeManager(sM);
    funDrag.setGraphiqueManager(gM);
    funCircle.setShapeManager(sM);
    funCircle.setGraphiqueManager(gM);
    funView.setShapeManager(sM);
    funView.setGraphiqueManager(gM);
    funDelete.setShapeManager(sM);
    funDelete.setGraphiqueManager(gM);

    funDebug.setWorldManager(wM);
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
            funDrag.turnOn();funPoly.turnOff();funCircle.turnOff();funView.turnOff();funDelete.turnOff();
            return;
        }else if(funPoly.buttonTouched(x,y)&&currentAction!=Poly){
            currentAction=Poly;
            funPoly.turnOn();funDrag.turnOff();funCircle.turnOff();funView.turnOff();funDelete.turnOff();
            return;
        }else if(funCircle.buttonTouched(x,y)&&currentAction!=Circle){
            currentAction=Circle;
            funCircle.turnOn();funPoly.turnOff();funDrag.turnOff();funView.turnOff();funDelete.turnOff();
            return;
        }else if(funView.buttonTouched(x,y)&&currentAction!=View){
            currentAction=View;
            funCircle.turnOff();funPoly.turnOff();funDrag.turnOff();funView.turnOn();funDelete.turnOff();
            return;
        }else if(funDelete.buttonTouched(x,y)&&currentAction!=Delete){
            currentAction=Delete;
            funCircle.turnOff();funPoly.turnOff();funDrag.turnOff();funView.turnOff();funDelete.turnOn();
            return;
        }else if(funDebug.buttonTouched(x,y)){
            funDebug.takeAction();
            return;
        }
    }
    funDebug.autoTurnOff();
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
    case View:
        funView.takeAction();
        break;
    case Delete:
        funDelete.takeAction();
        break;
    default:
        break;
    }
    flushAllEvents();
}

void UserUI::drawButtons()
{
    drawer->myNoRefreshPush();
    drawOneButton(funDrag.getButton());
    drawOneButton(funPoly.getButton());
    drawOneButton(funCircle.getButton());
    drawOneButton(funView.getButton());
    drawOneButton(funDelete.getButton());
    drawOneButton(funDebug.getButton());
    drawer->myNoRefreshPop();
}



void Functionality::setShapeManager(ShapeManager *sM)
{
    shapeManager=sM;
}

void Functionality::setGraphiqueManager(GraphicManager *gM)
{
    drawer=gM;
}


void FunView::takeAction()
{
    Imagine::Event ev;
    do{
        Imagine::getEvent(0,ev);
        //MOVE
        if(ev.type==Imagine::EVT_BUT_ON&&ev.button==1){
            moving=true;zooming=false;
            Vector2 v;
            drawer->coordTranslateInverse(ev.pix.x(),ev.pix.y(),v.x,v.y);
            mousePoint=v;
        }else if(ev.type==Imagine::EVT_MOTION&&moving){
            Vector2 v;
            drawer->coordTranslateInverse(ev.pix.x(),ev.pix.y(),v.x,v.y);
            v=v-mousePoint;
            drawer->ajustViewZoom(-v,1);
        }else if(ev.type==Imagine::EVT_BUT_OFF){
            moving=false;zooming=false;

        }
        // ZOOM
        else if(ev.type==Imagine::EVT_BUT_ON&&ev.button==3){
            zooming=true;moving=false;
            lastY=ev.pix.y();
        }else if(ev.type==Imagine::EVT_MOTION&&zooming){
            int delta=ev.pix.y()-lastY;
            lastY=ev.pix.y();
            if(delta>0){
                double zoom=1+delta*0.005;
                drawer->ajustViewZoom(Vector2(0,0),zoom);
            }else if(delta<0){
                double zoom=1+delta*0.005;
                drawer->ajustViewZoom(Vector2(0,0),zoom);
            }
        }else if(ev.type==Imagine::EVT_BUT_OFF){
            moving=false;zooming=false;
        }
    }while(ev.type!=Imagine::EVT_NONE);


}




void FunDelete::takeAction()
{
    Imagine::Event ev;
    do{
        Imagine::getEvent(0,ev);
        if(ev.type==Imagine::EVT_BUT_ON&&ev.button==1){
            Vector2 v;
            drawer->coordTranslateInverse(ev.pix.x(),ev.pix.y(),v.x,v.y);
            std::vector<MyShape*> shapeList=shapeManager->getShapeList();
            MyShape* pShape=findTargetShape(v,shapeList);
            if(pShape){
                shapeManager->deleteShape(pShape);
            }
        }
    }while(ev.type!=Imagine::EVT_NONE);
}


void FunDebug::setWorldManager(WorldManager *wM)
{
    this->wM=wM;
}

void FunDebug::takeAction()
{




    debugMode=!debugMode;
    if(debugMode){
        turnOn();
        wM->turnOnDebugDraw();
    }else{
        turnOff();
        wM->turnOffDebugDraw();
    }


}

void FunDebug::autoTurnOff()
{
    if(turnOffDebugAfterOneFrame){
        if(debugMode){
            debugMode=false;
            turnOff();
            wM->turnOffDebugDraw();
        }

    }
}
