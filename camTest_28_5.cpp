#include <iostream>
#include "Camera.h"

int main() {
    Camera cam;

    // تنظیم مد مورد نظر (اختیاری)
    cam.setMode(Camera::Mode::CLOSE_UP);

    // تعیین تسک روشن شدن دوربین
    cam.setTask(Camera::Task::TURN_ON);

    int maxFrames = 100;
    int count = 0;

    while (true) { // count < maxFrames && cam.getTask() != Camera::Task::IDLE) {
        cam.update();

        if (cam.getSuccess()) {
            auto features = cam.getFaceFeatures();
            std::cout << "Detected features:\n";
            std::cout << "Gender: " << features.gender << "\n";
            std::cout << "Age: " << features.age << "\n";
            std::cout << "Race: " << features.race << "\n";
            std::cout << "Hair Color: " << features.hairColor << "\n";
            std::cout << "Eye Color: " << features.eyesColor << "\n";
            break;
        }
        count++;
        // اضافه کردن کمی تأخیر برای جلوگیری از مصرف زیاد CPU (اختیاری)
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    if (!cam.getSuccess()) {
        std::cout << "No face detected or analysis failed after " << maxFrames << " frames." << std::endl;
    }

    // خاموش کردن دوربین و آزادسازی منابع
    cam.setTask(Camera::Task::TURN_OFF);
    cam.update();

    return 0;
}
