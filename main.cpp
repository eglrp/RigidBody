#include <stdio.h>
#include <string>
#include "graphics.h"
#include "worldmanager.h"
#include "frametimer.h"
#include <math.h>
#include <Imagine/Graphics.h>

using namespace std;
int main()
{
    int width,height;
    width=height=1024;
    long time;
    double dt=0.02;//second
    double simulationSpeed=1.0;
    GraphicManager graphicManager(width,height);
    WorldManager worldManager(dt,Vector2(0,-10));
    double frameTime=dt/simulationSpeed;

    Imagine::openWindow(width,height);
    worldManager.readFile("data.txt");
    FrameTimer timer(frameTime);
    timer.start();

    int frameCount=0;
    while(1){

        if(timer.isTimeToGo()){

            graphicManager.myDisplay(worldManager.getShapeList(),dt);
            worldManager.OneStep();
            timer.finishOneFrame();
            cout<<"frameCount"<<frameCount++<<endl;

        }
    }
    Imagine::endGraphics();
    return 0;
}

