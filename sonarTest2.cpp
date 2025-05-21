#include <iostream>
#include <wiringPi.h>
#include "Sonar2.h"

int main() {
    if (wiringPiSetupGpio() == -1) {
        std::cerr << "خطا در راه‌اندازی WiringPi" << std::endl;
        return 1;
    }

    Sonar2 sonar(17, 27); // GPIO17 -> success, GPIO27 -> fail

    std::cout << "شروع تست سنسور Sonar..." << std::endl;

    while (true) {
        // sonar.update();

        if (sonar.getSuccess()) {
            std::cout << "success" << std::endl;
        } else if (sonar.getFail()) {
            std::cout << "fail" << std::endl;
        }

        delay(10); // صبر 200 میلی‌ثانیه برای کاهش سرعت چاپ
    }

    return 0;
}
