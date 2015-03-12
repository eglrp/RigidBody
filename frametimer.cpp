#include "frametimer.h"


FrameTimer::FrameTimer(double oneFrameTime)
{
    this->oneFrameTime=oneFrameTime*1000;
}

void FrameTimer::start()
{
    timer.start();
    lastFrameTime=timer.getElapsedTimeInMilliSec();
    lastCall=lastFrameTime;
}

bool FrameTimer::isTimeToGo()
{
    double elapsedTime=timer.getElapsedTimeInMilliSec()-lastFrameTime;
    //if we have missed too much frame, just let them go.
    if(elapsedTime>oneFrameTime*3){
        lastFrameTime=timer.getElapsedTimeInMilliSec()-oneFrameTime*2;
        return true;
    }
    return timer.getElapsedTimeInMilliSec()-lastFrameTime>oneFrameTime;
}

void FrameTimer::finishOneFrame()
{
    lastFrameTime+=oneFrameTime;
}

double FrameTimer::timeBetweenTwoCalls()
{
    double now=timer.getElapsedTimeInMilliSec();
    double timeGap=now-lastCall;
    lastCall=now;
    return timeGap;
}

FrameTimer::~FrameTimer()
{

}

