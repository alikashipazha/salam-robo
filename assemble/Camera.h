#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/face.hpp>  // برای facemark
#include <vector>
#include <string>

using namespace cv;
using namespace cv::face;
using namespace std;

class Camera {
public:
    enum Task {
        TURN_ON,
        FACE_DETECTION,
        TURN_OFF,
        IDLE
    };
    enum Mode {
        CLOSE_UP,
        DISTANT
    };
    typedef struct {
        std::string race;        // black, asian, caucasian
        std::string hairColor;   // blonde, ginger, black_brown, gray_white
        std::string eyesColor;   // blue, green, black+brown
        std::string age;         // kid, (teen,) adult, old
        std::string gender;      // woman, man
    } FaceFeatures;
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
    std::string clothColor;
    std::chrono::steady_clock::time_point idleStartTime;
    FaceFeatures faceFeatures;
    Ptr<Facemark> facemark;
    unsigned short ComposureCounter;

public:
    Camera();
    Camera::Task getTask() const;
    Camera::Mode getMode() const;
    std::string getClothColor() const;
    void setTask(Task t);
    void setMode(Mode mode);
    void setClothColor(std::string clothColor);
    void setTimer();
    void setNextTask(Task nextTask);
    cv::Vec3b calculateBGRMean(const cv::Mat& image, const cv::Rect& roi);
    std::tuple<std::string, std::string, std::string> processFrame(cv::Mat& frame, cv::Rect roi);
    std::string classifyColor(cv::Vec3b bgr);
    bool isDominantColor(cv::Mat& frame, Rect roi, float thresh);
    cv::Rect getIrisRect(const std::vector<Point2f>& points, int p1, int p2, int p3, int p4);
    bool openCam(cv::VideoCapture& cap, int camID);
    void update();
    bool getSuccess() const;
    FaceFeatures getFaceFeatures() const;
};

#endif // CAMERA_H
