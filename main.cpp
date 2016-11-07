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

    //Window size
    width=height=500;

    //Create managers: GraphicManager, WorldManager UserUIManager
    //And link them
    GraphicManager graphicManager(width,height);
    WorldManager worldManager;
    UserUI userUI(worldManager.getShapeManager(),&graphicManager,&worldManager);
    worldManager.setDebugDrawer(&graphicManager);


    Imagine::openWindow(width,height);

    //-----------Edit your input file name  here--------------
    //--------------------------------------------------------
    //--------------------------------------------------------
    //worldManager.readFile("data_angrybirds.txt");
    worldManager.readFile("data_tutorial.txt");
    //worldManager.readFile("data10block.txt");
    //--------------------------------------------------------
    //--------------------------------------------------------
    //--------------------------------------------------------

    //worldManager tell us what is a correct frame time
    //And we creat a FrameTimer to tell us when to run one step simulation
    double frameTime=worldManager.getRecommendFrameTime();
    FrameTimer timer(frameTime);
    timer.start();

    int frameCount=0;
    while(1){
        if(timer.isTimeToGo()){

            //Graphic ask World for the shapeList, and he draw all the shapes
            graphicManager.myDisplay(worldManager.getShapeList());
            graphicManager.savePrintScreen();
            //World run one step of simulation
            worldManager.OneStep();

            //UserUI can draw buttons on screen, because he knows GraphicManager
            userUI.drawButtons();

            //UserUI treat user's action like "drag object" and "creat polygon"
            userUI.treatEvents();

            //tell FrameTimer that we have finished one frame
            //he will note this down
            //and we will wait him to say: "is time to run another frame!"
            timer.finishOneFrame();
            cout<<"frameCount "<<frameCount++<<endl;

            //----DEBUG----
            //if(frameCount==134){
            //    cout<<"stop"<<endl;
            //}
            //~DEBUG

            cout<<"fps "<<1000./timer.timeBetweenTwoCalls()<<endl;

        }
    }
    Imagine::endGraphics();
    return 0;
}

