#include "Camera.h"
#include <iostream>
#include <string>

#define CONFIDENCE_THRESHOLD 0.7
#define CLOTH_THRESHOLD 0.7
#define ROI_RATIO 1/2

using namespace cv;
using namespace cv::face;
using namespace std;

Camera::Camera() : currentTask(IDLE) {
    std::string faceProto = "face_detector/opencv_face_detector.pbtxt";
    std::string faceModel = "face_detector/opencv_face_detector_uint8.pb";
    std::string ageProto = "age_detector/deploy_age.prototxt";
    std::string ageModel = "age_detector/age_net.caffemodel";
    std::string genderProto = "gender_detector/deploy_gender.prototxt";
    std::string genderModel = "gender_detector/gender_net.caffemodel";
    facemark = FacemarkLBF::create();
    facemark->loadModel("lbfmodel.yaml");
    faceNet = dnn::readNetFromTensorflow(faceModel, faceProto);
    ageNet = dnn::readNetFromCaffe(ageProto, ageModel);
    genderNet = dnn::readNetFromCaffe(genderProto, genderModel);

    this->ageList = { "(0-2)", "(4-6)", "(8-12)", "(15-20)",
                        "(25-32)", "(38-43)", "(48-53)", "(60-100)" };
    this->genderList = { "Male", "Female" };
    this->currentMode = DISTANT;
    
    this->setTask(TURN_ON);
    
    cout << "Camera constructed" << endl;
}

Camera::Task Camera::getTask() const {
    return currentTask;
}

Camera::Mode Camera::getMode() const {
    return currentMode;
}

std::string Camera::getClothColor() const {
    return this->clothColor;
}

void Camera::setTask(Task t) {
    success = false;
    currentTask = t;
    cout << "Camera: new task is " << currentTask << endl;
}

void Camera::setMode(Mode mode) {
    currentMode = mode;
    cout << "Camera: new Mode is " << currentMode << endl;
}

void Camera::setClothColor(std::string clothColor){
    this->clothColor = clothColor;
}

void Camera::setTimer() {
    idleStartTime = std::chrono::steady_clock::now();
}

void Camera::setNextTask(Task nextTask) {
    this->nextTask = nextTask;
}

std::string Camera::classifyColor(cv::Vec3b hsv) { //new
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

bool Camera::isDominantColor(cv::Mat& frame, Rect roi, float thresh) { //new
	if (frame.empty() || roi.x < 0 || ((roi.x + roi.width) > frame.cols))
		return false;

	// Rect roi(x1, y1, dx, dy);
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
	cout << "Camera: Dominant color: " << this->dominantColor << " - " << ratio * 100 << "% of ROI" << endl;
	return ratio >= thresh;
}

cv::Rect Camera::getIrisRect(const std::vector<Point2f>& points, int p1, int p2, int p3, int p4) {
    Point2f pt1 = points[p1];
    Point2f pt2 = points[p2];
    Point2f pt3 = points[p3];
    Point2f pt4 = points[p4];

    float x_min = std::min(pt1.x, pt3.x);
    float x_max = std::max(pt2.x, pt4.x);
    float y_min = std::min(pt1.y, pt2.y);
    float y_max = std::max(pt3.y, pt4.y);

    return Rect(Point2f(x_min, y_min), Point2f(x_max, y_max));
}

void Camera::update() {
    switch (currentTask) {
        case TURN_ON:
            if (!cap.isOpened()) {
                cap.open(0);  // باز کردن وبکم پیشفرض10 -> 0
                if (!cap.isOpened()) {
                    cerr << "Error: Cannot open camera\n";
                } else {
                    cout << "Camera opened successfully.\n";
                    this->setTask(FACE_DETECTION);
                }
            }
            break;

        case FACE_DETECTION: {
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
					cout << "Camera: face n." << i << " detected" << endl;
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
                        this->faceFeatures.gender = genderList[genderIdx];

                        // تشخیص سن
                        ageNet.setInput(faceBlob);
                        Mat agePreds = ageNet.forward();
                        double ageConf;
                        Point classNumber;
                        minMaxLoc(agePreds, NULL, &ageConf, NULL, &classNumber);
                        this->faceFeatures.age = ageList[classNumber.x];
                        if (this->faceFeatures.age == "(0-2)" || this->faceFeatures.age == "(4-6)" || this->faceFeatures.age == "(8-12)") {
                            this->faceFeatures.age = "kid";
                        } else if (this->faceFeatures.age == "(60-100)") {
                            this->faceFeatures.age = "old";
                        } else {
                            this->faceFeatures.age = "adult";
                        }
                        
                        
                        
                        //race
                        Rect roiFace(x1, y1, x2-x1, y2-y1);
                        if (isDominantColor(frame, roiFace, 0.8)) {
                            this->dominantColor;
                            if(this->dominantColor == "black" || this->dominantColor == "brown") {
                                this->faceFeatures.race = "black";
                            }
                        }

                        // تشخیص چشم ها
                        // std::vector<std::vector<Point2f>> landmarks;
                        std::vector<Point2f> landmark;
                        std::vector<Rect> faces = { faceBox };
                        std::vector<std::vector<Point2f>> landmarks;

                        bool facemarkSuccess = facemark->fit(frame, faces, landmarks);
                        if (facemarkSuccess && !landmarks.empty()) {
                            landmark = landmarks[0]; // فقط اولین صورت
                            // سپس مستطیل عنبیه چشم‌ها را استخراج کن:
                            Rect leftIris = getIrisRect(landmark, 37, 38, 41, 40);
                            Rect rightIris = getIrisRect(landmark, 43, 44, 47, 46);
                            if(isDominantColor(frame, leftIris, 0.4)) {
                                this->faceFeatures.eyesColor = this->dominantColor;
                            } else if(isDominantColor(frame, rightIris, 0.4)) {
                                this->faceFeatures.eyesColor = this->dominantColor;
                            } else {
                                this->faceFeatures.eyesColor = "nan";
                            }
                        }

                        // تشخیص مو ها
                        int hairlen = (y2-y1)/5;
                        Rect roiHair(x1, y1-hairlen, x2-x1, hairlen);
                        if(isDominantColor(frame, roiHair, 0.5)) {
                            this->faceFeatures.hairColor = this->dominantColor;
                        }                      

                        // متن روی تصویر
                        string label = this->faceFeatures.gender + ", " + this->faceFeatures.age + ", " + this->faceFeatures.race
                                        + ", " + this->faceFeatures.hairColor + ", " + this->faceFeatures.eyesColor;
                        putText(frame, label, Point(x1, y1 - 10),
                                FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 0, 0), 2);
                    } else {
                        Rect roiCloth(x1, y1, x2 - x1, (frame.rows-y1)*ROI_RATIO);
                        if(isDominantColor(frame, roiCloth, 0.7)) {
                            //here
                            this->setClothColor(this->dominantColor);
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
            break; //here
		}

        case TURN_OFF:
            if (cap.isOpened()) {
                cap.release();
                destroyAllWindows();
                cout << "Camera released and windows destroyed.\n";
            }
            // this->setTask(IDLE);
            break;

        case IDLE:
            //CHAT you have to set non-blocking timer so that after it's finished Task goes back to FACE_DETECTION
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - idleStartTime).count();
            if (elapsed >= 10)
                this->setTask(this->nextTask);
            break;
    }
}

bool Camera::getSuccess() const {
    return success;
}

Camera::FaceFeatures Camera::getFaceFeatures() const {
    return this->faceFeatures;
}
