#include <iostream>
#include <thread>
#include "Camera.h"
#include "Monitor.h"

int main() {
    Camera cam;

    // روشن کردن دوربین
    cam.setTask(Camera::TURN_ON);

    while (true) {
        cam.update();
		if(cam.getSuccess()) {
			cout << cam.getSuccess() << endl;
			// مسیر پوشه ویدیو و فایل ویدیو
			string videosDir = "/home/salam-robo/test/video";
			string videoFile = videosDir + "/first.mp4";

			Monitor monitor(videosDir);

			monitor.setVideo(videoFile);
			monitor.setTask(Monitor::Task::PLAY_VIDEO);

			cout << "Starting video playback: " << videoFile << endl;

			// چک کردن وضعیت پخش هر 500 میلی‌ثانیه
			while (true) {
				monitor.update();

				if (monitor.getVideoFinished()) {
					cout << "Video playback finished." << endl;
					break;
				}

				this_thread::sleep_for(chrono::milliseconds(500));
			}
			cam.setTask(Camera::Task::FACE_DETECTION);
		}
        // خروج با کلید ESC
        if (cv::waitKey(1) == 27) {
            std::cout << "ESC pressed. Turning off and exiting...\n";
            cam.setTask(Camera::TURN_OFF);
            break;
        }
    }

    return 0;
}
