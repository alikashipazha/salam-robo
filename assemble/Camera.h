#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

class Camera {
public:
    enum Task {
        TURN_ON,
        FACE_DETECTION,
        ADVANCED_FACE_DETECTION,
        TURN_OFF,
        IDLE
    };
    enum Mode {
        CLOSE_UP,
        DISTANT
    };

private:
    Task currentTask;
    Task nextTask;
    Mode currentMode;
    bool success = false;
    cv::dnn::Net faceNet;
    cv::dnn::Net ageNet;
    cv::dnn::Net genderNet;
    std::vector<std::string> ageList;
    std::vector<std::string> genderList;
    cv::VideoCapture cap;
    std::string dominantColor;
    std::string color;
    std::chrono::steady_clock::time_point idleStartTime;
    std::string gender;
    std::string age;

public:
    Camera();
    Camera::Task getTask() const;
    Camera::Mode getMode() const;
    std::string getColor() const;
    void setTask(Task t);
    void setMode(Mode mode);
    void setColor(std::string color);
    void setTimer();
    void setNextTask(Task nextTask);
    std::string classifyColor(cv::Vec3b hsv);
    bool isDominantColor(cv::Mat& frame, int x1, int x2, int y1);
    void update();
    bool getSuccess() const;
    string getGender() const;
    string getAge() const;
};

#endif // CAMERA_H
