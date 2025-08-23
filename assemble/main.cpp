#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>

#include "Sonar2.h"
#include "Camera.h"
#include "Monitor.h"
#include "FSM.h"

#define SLEEP_TIME 10 //50

using namespace std;

int main() {
    std::atomic<bool> running(true);

    Sonar2 sonar(17, 27);
    Camera camera;
    Monitor monitor("/home/salam-robo/test/assemble/videos", "/home/salam-robo/test/assemble/audios", "/home/salam-robo/test/assemble/gif3.mp4");
    FSM fsm;
    string gif = "home/salam-robo/test/assemble/gif1.mp4";
    // std::string command = "mpv --loop " + gif + " &"; // استفاده از & برای اجرا در پس‌زمینه
    // // std::string command = "nohup mpv --loop " + this->scSaverGif + " &";
    // system(command.c_str());

    // monitor.startPlayback(); //HERE-JUNE : comment shod
    
    // // HERE tabestoon for calibration test
    // camera.setMode(Camera::Mode::CLOSE_UP);

    std::thread cameraThread([&]() {
        // while (running.load()) {
        //     camera.update();
        //     cout << "thread cam" << endl; //HERE 
        //     std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
        // }
        camera.update();
        cout << "thread cam" << endl;
    });

    // std::thread monitorThread([&]() { //HERE test8june
    //     while (running.load()) {
    //         monitor.update();
    //         std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
    //     }
    // });

    // std::thread fsmThread([&]() {
    //     while (running.load()) {
    //         fsm.update(sonar, camera, monitor);
    //         std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
    //     }
    // });

    // HERE tabestoon comment for calibration test
    while (running.load()) {
        fsm.update(sonar, camera, monitor);
        monitor.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
    }

    // اینجا میتونی شرط توقف بذاری، الان برنامه بی‌نهایت اجرا میشه
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // running.store(false);

    cameraThread.join();
    //HERE test8june
    // monitorThread.join();
    // fsmThread.join();

    return 0;
}
