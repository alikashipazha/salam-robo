#include "Camera.h"
#include <iostream>

#define CONFIDENCE_THRESHOLD 0.7
#define CLOTH_THRESHOLD 0.5
#define ROI_RATIO 1/2

using namespace cv;
using namespace std;

Camera::Camera() : currentTask(IDLE) {
    string faceProto = "face_detector/opencv_face_detector.pbtxt";
    string faceModel = "face_detector/opencv_face_detector_uint8.pb";
    string ageProto = "age_detector/deploy_age.prototxt";
    string ageModel = "age_detector/age_net.caffemodel";
    string genderProto = "gender_detector/deploy_gender.prototxt";
    string genderModel = "gender_detector/gender_net.caffemodel";

    faceNet = dnn::readNetFromTensorflow(faceModel, faceProto);
    ageNet = dnn::readNetFromCaffe(ageProto, ageModel);
    genderNet = dnn::readNetFromCaffe(genderProto, genderModel);

    this->ageList = { "(0-2)", "(4-6)", "(8-12)", "(15-20)",
                        "(25-32)", "(38-43)", "(48-53)", "(60-100)" };
    this->genderList = { "Male", "Female" };
    this->currentMode = DISTANT;
    this->setTask(IDLE);
}

Camera::Task Camera::getTask() const {
    return currentTask;
}

Camera::Mode Camera::getMode() const {
    return currentMode;
}

string Camera::getColor() const {
    return this->color;
}

void Camera::setTask(Task t) {
    success = false;
    currentTask = t;
}

void Camera::setMode(Mode mode) {
    currentMode = mode;
}

void Camera::setColor(string color){
    this->color = color;
}

string classifyColor(Vec3b hsv) { //new
	int h = hsv[0]; // hue: 0-179
	int s = hsv[1]; // saturation: 0-255
	int v = hsv[2]; // value (brightness): 0-255

	if (v < 40)
		return "black";
	if (s < 40 && v > 200)
		return "white";
	if (s < 40)
		return "gray";

	if ((h < 10 || h >= 170) && s > 100)
		return "red";
	if (h >= 10 && h < 20 && s > 100 && v < 150)
		return "brown";
	if (h >= 10 && h < 25 && s > 100 && v >= 150)
		return "orange";
	if (h >= 25 && h < 35)
		return "yellow";
	if (h >= 35 && h < 85)
		return "green";
	if (h >= 85 && h < 125)
		return "blue";
	if (h >= 125 && h < 150)
		return "purple";
	if (h >= 150 && h < 170 && v > 180)
		return "pink";

	// fallback
	return "unknown";
}

bool isDominantColor(cv::Mat& frame, int x1, int x2, double threshold = CLOTH_THRESHOLD) { //new
	if (frame.empty() || x1 >= x2 || x1 < 0 || x2 > frame.cols)
		return false;

	Rect roi(x1, 0, x2 - x1, frame.rows*ROI_RATIO);
	Mat region = frame(roi);

	// تبدیل به HSV برای دسته‌بندی بهتر رنگ
	Mat hsv;
	cvtColor(region, hsv, COLOR_BGR2HSV);

	map<string, int> colorCounts;
	int totalPixels = hsv.rows * hsv.cols;

	// شمارش تعداد پیکسل‌ها در هر دسته رنگ
	for (int y = 0; y < hsv.rows; ++y) {
		for (int x = 0; x < hsv.cols; ++x) {
			Vec3b pixel = hsv.at<Vec3b>(y, x);
			string colorName = classifyColor(pixel);
			colorCounts[colorName]++;
		}
	}

	// پیدا کردن رنگ غالب
	int maxCount = 0;
	for (auto& pair : colorCounts) {
		if (pair.second > maxCount) {
			maxCount = pair.second;
			this->dominantColor = pair.first;
		}
	}

	double ratio = static_cast<double>(maxCount) / totalPixels;
	cout << "Dominant color: " << this->dominantColor << " - " << ratio * 100 << "% of ROI" << endl;
	return ratio >= threshold;
}

void Camera::update() {
    switch (currentTask) {
        case TURN_ON:
            if (!cap.isOpened()) {
                cap.open(1);  // باز کردن وبکم پیشفرض10 -> 0
                if (!cap.isOpened()) {
                    cerr << "Error: Cannot open camera\n";
                } else {
                    cout << "Camera opened successfully.\n";
                    this->setTask(FACE_DETECTION);
                }
            }
            break;

        case FACE_DETECTION:
            if (!cap.isOpened()) {
                cerr << "Camera is not opened!\n";
                break;
            }
            Mat frame;
            cap >> frame;
            if (frame.empty()) {
                cerr << "Blank frame grabbed\n";
                break;
            }

            // تشخیص چهره
            Mat blob = dnn::blobFromImage(frame, 1.0, Size(300, 300),
                                            Scalar(104.0, 177.0, 123.0), false, false);
            faceNet.setInput(blob);
            Mat detections = faceNet.forward();
            Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());

            for (int i = 0; i < detectionMat.rows; i++) {
                float confidence = detectionMat.at<float>(i, 2);
                if (confidence > CONFIDENCE_THRESHOLD) {
                    int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
                    int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
                    int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
                    int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

                    x1 = max(0, x1);
                    y1 = max(0, y1);
                    x2 = min(frame.cols - 1, x2);
                    y2 = min(frame.rows - 1, y2);

                    Rect faceBox(Point(x1, y1), Point(x2, y2));
                    rectangle(frame, faceBox, Scalar(0, 255, 0), 2);

                    Mat face = frame(faceBox);

                    Mat faceBlob = dnn::blobFromImage(face, 1.0, Size(227, 227),
                                                        Scalar(78.4263377603, 87.7689143744, 114.895847746),
                                                        false);

                    if(this->currentMode == Camera::Mode::CLOSE_UP) { //new
                        // تشخیص جنسیت
                        genderNet.setInput(faceBlob);
                        Mat genderPreds = genderNet.forward();
                        int genderIdx = genderPreds.at<float>(0) > genderPreds.at<float>(1) ? 0 : 1;
                        string gender = genderList[genderIdx];

                        // تشخیص سن
                        ageNet.setInput(faceBlob);
                        Mat agePreds = ageNet.forward();
                        double ageConf;
                        Point classNumber;
                        minMaxLoc(agePreds, NULL, &ageConf, NULL, &classNumber);
                        string age = ageList[classNumber.x];

                        // متن روی تصویر
                        string label = gender + ", " + age;
                        putText(frame, label, Point(x1, y1 - 10),
                                FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 0, 0), 2);
                    } else {
                        if(isDominantColor(frame, x1, x2)) {
                            //here
                            this->setColor(this->dominantColor);
                            this->success = true;
                            break;
                        }
                    }
                }

                imshow("Face, Age & Gender Detection", frame);
                if (waitKey(1) == 27) {
                    this->setTask(TURN_OFF);
                }
                if(!detectionMat.empty()) {
                    success = true;
                }
                break;
            }

        case ADVANCED_FACE_DETECTION:
            cout << "Running Advanced Face Detection...\n";
            success = (rand() % 100) < 90;
            break;

        case TURN_OFF:
            if (cap.isOpened()) {
                cap.release();
                destroyAllWindows();
                cout << "Camera released and windows destroyed.\n";
            }
            this->setTask(IDLE);
            break;

        case IDLE:
            break;
    }
}

bool Camera::getSuccess() const {
    return success;
}
