#include <iostream>
#include "Camera.h"

int main() {
    Camera cam;

    // روشن کردن دوربین
    cam.setTask(Camera::TURN_ON);

    while (true) {
        cam.update();

        // خروج با کلید ESC
        if (cv::waitKey(1) == 27) {
            std::cout << "ESC pressed. Turning off and exiting...\n";
            cam.setTask(Camera::TURN_OFF);
            break;
        }
    }

    return 0;
}
