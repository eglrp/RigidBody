#ifndef FRAMETIMER_H
#define FRAMETIMER_H
#include "timer.h"

class FrameTimer
{
    Timer timer;
    double oneFrameTime;
    double lastFrameTime;
    double lastCall;
public:

    FrameTimer(double oneFrameTime);
    void start();
    bool isTimeToGo();
    void finishOneFrame();
    double timeBetweenTwoCalls();
    ~FrameTimer();
};

#endif // FRAMETIMER_H
