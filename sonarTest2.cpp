#include <iostream>
#include <wiringPi.h>
#include "Sonar.h"

int main() {
    if (wiringPiSetupGpio() == -1) {
        std::cerr << "خطا در راه‌اندازی WiringPi" << std::endl;
        return 1;
    }

    Sonar sonar(17, 27); // GPIO17 -> success, GPIO27 -> fail

    std::cout << "شروع تست سنسور Sonar..." << std::endl;

    while (true) {
        sonar.update();

        if (sonar.getSuccess()) {
            std::cout << "✅ سیگنال موفقیت دریافت شد." << std::endl;
        } else if (sonar.getFail()) {
            std::cout << "❌ سیگنال شکست دریافت شد." << std::endl;
        } else {
            std::cout << "⏳ هیچ سیگنالی دریافت نشده." << std::endl;
        }

        delay(200); // صبر 200 میلی‌ثانیه برای کاهش سرعت چاپ
    }

    return 0;
}
