#include <opencv2/opencv.hpp>
#include <iostream>
#include "Camera.h"  // وارد کردن هدر کلاس Camera

int main() {
    // باز کردن دوربین
    cv::VideoCapture cap(0);  // 0 برای دوربین پیش‌فرض

    if (!cap.isOpened()) {
        std::cerr << "دوربین باز نشد!" << std::endl;
        return -1;
    }

    cv::Mat frame;
    Camera camera;  // ایجاد آبجکت از کلاس Camera

    while (true) {
        // خواندن فریم از دوربین
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "فریم شناسایی نشد!" << std::endl;
            break;
        }

        // اعمال افزایش روشنایی به تصویر
        cv::Mat frameBrightened;
        frame.convertTo(frameBrightened, -1, 1, 75);  // افزایش روشنایی

        // نقطه‌ای که می‌خواهیم رنگش را بررسی کنیم
        int x = 250, y = 250;  // مختصات نقطه (میتوانید تغییر دهید)

        // گرفتن رنگ از نقطه خاص در فضای RGB
        cv::Vec3b colorRGB = frameBrightened.at<cv::Vec3b>(y, x);

        // تبدیل تصویر به فضای HSV
        cv::Mat hsvImage;
        cv::cvtColor(frameBrightened, hsvImage, cv::COLOR_BGR2HSV);

        // گرفتن رنگ از نقطه خاص در فضای HSV
        cv::Vec3b colorHSV = hsvImage.at<cv::Vec3b>(y, x);

        // نمایش رنگ RGB
        std::cout << "رنگ نقطه (" << x << ", " << y << ") در فضای RGB: "
                  << "R=" << (int)colorRGB[2] << ", G=" << (int)colorRGB[1] << ", B=" << (int)colorRGB[0] << std::endl;

        // نمایش رنگ HSV
        std::cout << "رنگ نقطه (" << x << ", " << y << ") در فضای HSV: "
                  << "H=" << (int)colorHSV[0] << ", S=" << (int)colorHSV[1] << ", V=" << (int)colorHSV[2] << std::endl;

        // دریافت رنگ شناسایی شده با استفاده از متد classifyColor
        std::string colorName = camera.classifyColor(colorRGB);  // گرفتن اسم رنگ
        
        // نمایش اسم رنگ
        std::cout << "رنگ شناسایی شده: " << colorName << std::endl;

        // رسم دایره بر روی نقطه مورد نظر برای نمایش
        cv::circle(frameBrightened, cv::Point(x, y), 5, cv::Scalar(0, 255, 0), -1);  // دایره سبز

        // نمایش تصویر
        cv::imshow("Frame", frameBrightened);

        // تاخیر ۲ ثانیه‌ای
        if (cv::waitKey(1000) == 'q') {
            break;
        }
    }

    // آزاد کردن منابع و بستن پنجره‌ها
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
