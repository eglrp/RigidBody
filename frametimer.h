#ifndef FRAMETIMER_H
#define FRAMETIMER_H
#include "timer.h"

class FrameTimer
{
    Timer timer;
    double oneFrameTime;
    double lastFrameTime;
public:

    FrameTimer(double oneFrameTime);
    void start();
    bool isTimeToGo();
    void finishOneFrame();
    ~FrameTimer();
};

#endif // FRAMETIMER_H
