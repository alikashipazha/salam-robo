#include "FSM.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

FSM::FSM() : currentState(SONAR_DETECTION), stateTimer(0), stateStartTimer(0) {
	cout << "FSM constructed" << endl;
	}

unsigned long millis() {
    using namespace chrono;
    static auto start = steady_clock::now();
    auto now = steady_clock::now();
    return duration_cast<milliseconds>(now - start).count();
}

void FSM::setStateMachine(State newState) {
    currentState = newState;
    stateStartTimer = millis();
    stateTimer = 0;
    cout << "FSM: new state is " << currentState << endl;
}

bool FSM::watchdog(unsigned int watchdogTimer) {
    return stateTimer > watchdogTimer;
}

void FSM::stateTimerUpdate() {
    stateTimer = millis() - stateStartTimer;
}

void FSM::update(const Sonar2& sonar, Camera& camera, Monitor& monitor) {
    switch(currentState) {
        case SONAR_DETECTION: {
            // string cd = monitor.getVideosDirectory() + '/' + "gif1.webm"; 
            // monitor.setStream(cd);
            // monitor.setTask(Monitor::Task::PLAY);
            if(camera.getSuccess()) {
                cout << camera.getClothColor() << endl;
                // monitor.setTask(Monitor::Task::READY2PLAY); //HERE

				cout << "FSM(SONAR_DETECTION): camera.getSuccess()" << endl;
                camera.setTask(Camera::Task::IDLE);
                camera.setTimer();
                camera.setNextTask(Camera::Task::FACE_DETECTION);
                string cd = monitor.getAudiosDirectory() + '/' + camera.getClothColor() + "/" + camera.getClothColor() + ".mp3"; 
                // string cd = monitor.getAudiosDirectory() + '/' + "AUDIO.mp3"; 
                monitor.setMode(Monitor::Mode::AUDIO); //HERE
                monitor.setStream(cd);
                monitor.setTask(Monitor::Task::PLAY);
            }
            if(sonar.getSuccess()) {
                // monitor.setTask(Monitor::Task::READY2PLAY); // HERE
				
                cout << "FSM(SONAR_DETECTION): sonar.getSuccess()" << endl;
                
                // /*string*/ cd = monitor.getVideosDirectory() + '/' + "gif2.webm"; 
                // monitor.setStream(cd);
                // monitor.setTask(Monitor::Task::PLAY);
                
                camera.setMode(Camera::Mode::CLOSE_UP);
                // sonar.setFailTimer(2.0);
                setStateMachine(FACE_DETECTION);
            }
            break;
        }
        case FACE_DETECTION:
            stateTimerUpdate();
            if(camera.getSuccess()) {
                // monitor.setTask(Monitor::Task::STOP); //HERE
                
				cout << "FSM(FACE_DETECTION): camera.getSuccess()" << endl;
                cout << camera.getFaceFeatures().gender << camera.getFaceFeatures().age << endl;
                // camera.setMode(Camera::Mode::...);
                string cd = monitor.getVideosDirectory() + '/' + camera.getFaceFeatures().gender + '/' + camera.getFaceFeatures().age;
                // string cd = monitor.getVideosDirectory() + '/' + "first.mp4"; //HERE
                monitor.setStream(cd);
                monitor.setMode(Monitor::Mode::VIDEO);
                monitor.setTask(Monitor::Task::PLAY);
                setStateMachine(PLAY_VIDEO_FILE);
            } else if((watchdog(5000) && sonar.getFail()) || watchdog(10000)) {
                if(sonar.getFail()) cout << "FSM(FACE_DETECTION): sonar.getFail()" << endl;
                else cout << "FSM(FACE_DETECTION): watchdog()" << endl;
                camera.setMode(Camera::Mode::DISTANT);
                monitor.setTask(Monitor::Task::STOP);
                setStateMachine(SONAR_DETECTION);
            }
            break;

        case PLAY_VIDEO_FILE:
            stateTimerUpdate();
            if((sonar.getFail() && watchdog(5000)) || monitor.getStreamFinished() || watchdog(60000)) { // HERE: cam condition?
                if(sonar.getFail()) cout << "FSM(PLAY_VIDEO_FILE): sonar.getFail()" << endl;
                monitor.setTask(Monitor::Task::STOP);
                setStateMachine(BYE);
            }
            break;

        case BYE:
            stateTimerUpdate();
            if(monitor.getStreamFinished() || watchdog(60000)) { // ignore this if condition for now
                monitor.setTask(Monitor::Task::STOP);
                setStateMachine(SONAR_DETECTION);
            }
            break;
    }
}

void FSM::printState() {
    switch(currentState) {
        case START: cout << "START\n"; break;
        case SONAR_DETECTION: cout << "SONAR_DETECTION\n"; break;
        case FACE_DETECTION: cout << "FACE_DETECTION\n"; break;
        case PLAY_VIDEO_FILE: cout << "PLAY_VIDEO_FILE\n"; break;
        case BYE: cout << "BYE\n"; break;
    }
}

FSM::State FSM::getState() {
    return currentState;
}
