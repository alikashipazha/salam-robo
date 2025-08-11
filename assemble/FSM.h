#ifndef FSM_H
#define FSM_H

#include <string>
#include "Sonar2.h"
#include "Camera.h"
#include "Monitor.h"

using namespace std;

class FSM {
public:
    enum State {
        START,
        SONAR_DETECTION,
        FACE_DETECTION,
        PLAY_VIDEO_FILE,
        BYE
    };

private:
    State currentState;
    unsigned long stateTimer;
    unsigned long stateStartTimer;
    // bool successInFaceDetection;

public:
    FSM();

    void setStateMachine(State newState);
    bool watchdog(unsigned int watchdogTimer);
    void stateTimerUpdate();

    void update(const Sonar2& sonar, Camera& camera, Monitor& monitor);

    void printState();
    State getState();
};

// تابع کمکی برای زمان برحسب میلی‌ثانیه
unsigned long millis();

#endif // FSM_H
