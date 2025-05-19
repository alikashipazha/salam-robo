#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <vector>
#include <string>

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
    Mode currentMode;
    bool success = false;
    cv::dnn::Net faceNet;
    cv::dnn::Net ageNet;
    cv::dnn::Net genderNet;
    std::vector<std::string> ageList;
    std::vector<std::string> genderList;
    cv::VideoCapture cap;
    string dominantColor;

public:
    Camera();
    Camera::Task getTask() const;
    Camera::Mode getMode() const;
    string getColor() const;
    void setTask(Task t);
    void setMode(Mode mdoe);
    void setColor(string color);
    string classifyColor(Vec3b hsv);
    bool isDominantColor(cv::Mat& frame, int x1, int x2, double threshold = 0.7);
    void update();
    bool getSuccess() const;
};

#endif // CAMERA_H
