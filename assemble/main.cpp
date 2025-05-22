#include <thread>
#include <atomic>
#include <chrono>

#include "Sonar2.h"
#include "Camera.h"
#include "Monitor.h"
#include "FSM.h"

using namespace std;

int main() {
    std::atomic<bool> running(true);

    Sonar2 sonar(17, 27);
    Camera camera;
    Monitor monitor("/path/to/videos", "/path/to/audios");
    FSM fsm;

    std::thread cameraThread([&]() {
        while (running.load()) {
            camera.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    });

    std::thread monitorThread([&]() {
        while (running.load()) {
            monitor.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    });

    std::thread fsmThread([&]() {
        while (running.load()) {
            fsm.update(sonar, camera, monitor);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    });

    // اینجا میتونی شرط توقف بذاری، الان برنامه بی‌نهایت اجرا میشه
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }

    // running.store(false);

    cameraThread.join();
    monitorThread.join();
    fsmThread.join();

    return 0;
}
