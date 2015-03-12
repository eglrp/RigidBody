#include <stdio.h>
#include <string>
#include "graphics.h"
#include "worldmanager.h"
#include "frametimer.h"
#include <math.h>
#include <Imagine/Graphics.h>
#include "userui.h"

using namespace std;
int main()
{
    int width,height;
    width=height=1024;


    GraphicManager graphicManager(width,height);
    WorldManager worldManager;
    UserUI userUI(worldManager.getShapeManager(),&graphicManager);
    worldManager.setDebugDrawer(&graphicManager);

    Imagine::openWindow(width,height);

    worldManager.readFile("data_circle.txt");
    double frameTime=worldManager.getRecommendFrameTime();
    FrameTimer timer(frameTime);
    timer.start();

    int frameCount=0;
    while(1){
        if(timer.isTimeToGo()){

            graphicManager.myDisplay(worldManager.getShapeList());
            worldManager.OneStep();
            userUI.treatEvents();
            userUI.drawButtons();
            timer.finishOneFrame();
            cout<<"frameCount "<<frameCount++<<endl;
            if(frameCount==134){
                cout<<"stop"<<endl;
            }
            cout<<"fps "<<1000./timer.timeBetweenTwoCalls()<<endl;

        }
    }
    Imagine::endGraphics();
    return 0;
}

