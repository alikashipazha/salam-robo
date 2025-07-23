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

std::string Camera::classifyColor(cv::Vec3b bgr) { //new HERE tabestoon
	
    int b = bgr[0]; // Blue component
    int g = bgr[1]; // Green component
    int r = bgr[2]; // Red component

    int max_val = max({r, g, b});  // بیشترین مقدار بین r، g و b
    int min_val = min({r, g, b});  // کمترین مقدار بین r، g و b
    int mid_val = r + g + b - max_val - min_val;

    // محاسبه اختلاف max و min
    int maxDiff = max_val - min_val;  // اختلاف بین max و min
    int upDiff = max_val - mid_val;
    int downDiff = mid_val - min_val;

    // red blue green 255 or 0 ?

    if(maxDiff < 20) {
        if(r < 75 && g < 75 && b < 75) {
            return "black";
        }
        if(r > 220 && g > 220 && b > 220) {
            return "whtie";
        }
        if(r > 75 && g > 75 && b > 75 && r < 175 && g < 175 && b < 175) {
            return "gray";
        }
    }

    if(upDiff < 20) {
        if(min_val == g && maxDiff > 60) {
            if(max_val < 170) {   
                return "purple"; 
            }
            return "pink";
        }
        if(min_val == b && maxDiff > 80) {
            if((max_val == r && r > 170) || (max_val == g && g > 230)) {
                return "yellow";
            }
        }
        if(min_val == r && maxDiff > 80) {
            if(max_val > 160) {
                return "cyan";
            }
        }
    }

    if(downDiff < 20) {
        if(max_val == b && b > 140 && upDiff > 100) {
            return "blue";
        }
        if(max_val == r && r < 120 && upDiff > 70)
    }
    //
    if(max_val == r) {
        if(upDiff > 130) {
            return "red";
        }
        if(upDiff < 20 && downDiff > 60 && r > 170) {
            if(mid_val == g) {
                return "yellow";
            }
            
        }
    }
    if(downDiff < 10) {
        if(max_val == r) {

        }
    }

    // تشخیص رنگ سیاه (dark color)
    if (r < 50 && g < 50 && b < 50) {
        return "black";
    }

    // تشخیص رنگ سفید (light color)
    if (r > 200 && g > 200 && b > 200) {
        return "white";
    }

    // تشخیص خاکی یا خاکستری
    if (r > 100 && g > 100 && b > 100 && r < 150 && g < 150 && b < 150) {
        return "gray";
    }

    // قرمز: رنگ قرمز معمولاً بیشترین مقدار را در R دارد
    if (r > 120 && g < 80 && b < 80) {
        return "red";
    }

    // سبز: رنگ سبز معمولاً بیشترین مقدار را در G دارد
    if (g > 120 && r < 80 && b < 80) {
        return "green";
    }

    // آبی: رنگ آبی معمولاً بیشترین مقدار را در B دارد
    if (b > 120 && r < 80 && g < 80) {
        return "blue";
    }

    // نارنجی: ترکیب زیاد قرمز و کمی سبز، کم بودن آبی
    if (r > 150 && g > 100 && b < 80) {
        return "orange";
    }

    // زرد: ترکیب زیاد قرمز و سبز، کم بودن آبی
    if (r > 150 && g > 150 && b < 100) {
        return "yellow";
    }

    // بنفش: ترکیب زیاد قرمز و آبی
    if (r > 100 && b > 100 && g < 80) {
        return "purple";
    }

    // صورتی: ترکیب زیاد قرمز و آبی، کمی سبز
    if (r > 180 && g < 100 && b > 150) {
        return "pink";
    }

    // اگر هیچکدام از شرایط رنگ‌های اصلی و فرعی صدق نکرد، "unknown" برگردانده می‌شود
    return "unknown";


    // old one

    // int b = b[0]; // hue: 0-179
	// int g = g[1]; // saturation: 0-255
	// int r = r[2]; // value (brightness): 0-255

	// if (v < 40)
	// 	return "black";
	// if (s < 40 && v > 200)
	// 	return "white";
	// if (s < 40)
	// 	return "gray";

	// if ((h < 10 || h >= 170) && s > 100)
	// 	return "red";
	// if (h >= 10 && h < 20 && s > 100 && v < 150)
	// 	return "brown";
	// if (h >= 10 && h < 25 && s > 100 && v >= 150)
	// 	return "orange";
	// if (h >= 25 && h < 35)
	// 	return "yellow";
	// if (h >= 35 && h < 85)
	// 	return "green";
	// if (h >= 85 && h < 125)
	// 	return "blue";
	// if (h >= 125 && h < 150)
	// 	return "purple";
	// if (h >= 150 && h < 170 && v > 180)
	// 	return "pink";

	// // fallback
	// return "unknown";
}

bool Camera::isDominantColor(cv::Mat& frame, Rect roi, float thresh) { //new HERE tabestoon : sum up all pixels at bgr, scale the summed result to 0-255 by subtraction and apply classifyColor to the result. batch process
	if (frame.empty() || roi.x < 0 || roi.y < 0 || ((roi.x + roi.width) > frame.cols) || ((roi.y + roi.height) > frame.rows) || roi.width <= 0 || roi.height <= 0)
		return false;

	// Rect roi(x1, y1, dx, dy);
	Mat region = frame(roi);
	if(region.empty())
		return false;
	// // تبدیل به HSV برای دسته‌بندی بهتر رنگ
	// Mat hsv;
	// cvtColor(region, hsv, COLOR_BGR2HSV);

	map<string, int> colorCounts;
	int totalPixels = region.rows * region.cols;

	// شمارش تعداد پیکسل‌ها در هر دسته رنگ
	for (int y = 0; y < region.rows; ++y) {
		for (int x = 0; x < region.cols; ++x) {
			Vec3b pixel = region.at<Vec3b>(y, x);
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
	//cout << "Camera: Dominant color: " << this->dominantColor << " - " << ratio * 100 << "% of ROI" << endl;
	return ratio >= thresh;
}

cv::Rect Camera::getIrisRect(const std::vector<Point2f>& points, int p1, int p2, int p3, int p4) {
    Point2f pt1 = points[p1];
    Point2f pt2 = points[p2];
    Point2f pt3 = points[p3];
    Point2f pt4 = points[p4];
    
    //cout << pt1 << pt2 << pt3 << pt4 << endl;

    float x_min = std::min(pt1.x, pt3.x);
    float x_max = std::max(pt2.x, pt4.x);
    float y_min = std::min(pt1.y, pt2.y);
    float y_max = std::max(pt3.y, pt4.y);

	int x = cvRound(x_min);
	int y = cvRound(y_min);
	int width = cvRound(x_max - x_min);
	int height = cvRound(y_max - y_min);
	
	if(width <= 0 || height <= 0) return Rect();
    
    return Rect(x_min, y_min, width, height);
}

void Camera::update() {
    while(true) {
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
            frame.convertTo(frame, -1, 1, 100);  // HERE tabestoon

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
					// cout << "Camera: face n." << i << " of " << detectionMat.rows << " detected" << endl;
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

                    if(this->currentMode == Camera::Mode::CLOSE_UP) { //new
                        // cout << "in close-up" << endl;
                        Mat faceBlob = dnn::blobFromImage(face, 1.0, Size(227, 227),
                                                        Scalar(78.4263377603, 87.7689143744, 114.895847746),
                                                        false);
                        
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
                        
                        
                        //cout << "race\n" << endl;
                        //race
                        Rect roiFace(x1, y1, x2-x1, y2-y1);
                        if (isDominantColor(frame, roiFace, 0.5)) {
                            this->dominantColor;
                            if(this->dominantColor == "black" || this->dominantColor == "brown") {
                                this->faceFeatures.race = "black";
                            }
                        }

						
                        //cout << "eyes\n" << endl;
                        // تشخیص چشم ها
                        // std::vector<std::vector<Point2f>> landmarks;
                        std::vector<Point2f> landmark;
                        std::vector<Rect> faces = { faceBox };
                        std::vector<std::vector<Point2f>> landmarks;

                        bool facemarkSuccess = facemark->fit(frame, faces, landmarks);
                        if (facemarkSuccess && !landmarks.empty()) {
                            landmark = landmarks[0]; // فقط اولین صورت
                            //cout << "n(p) = " << landmark.size() << endl;
                            // سپس مستطیل عنبیه چشم‌ها را استخراج کن:
                            Rect leftIris = getIrisRect(landmark, 38, 39, 42, 41);
                            Rect rightIris = getIrisRect(landmark, 44, 45, 48, 47);
                            if(isDominantColor(frame, leftIris, 0.4)) {
                                this->faceFeatures.eyesColor = this->dominantColor;
                            } else if(isDominantColor(frame, rightIris, 0.4)) {
                                this->faceFeatures.eyesColor = this->dominantColor;
                            } else {
                                this->faceFeatures.eyesColor = "nan";
                            }
                        }
						
						
                        //cout << "hair\n" << endl;
                        // تشخیص مو ها
                        int hairlen = (y2-y1)/6;
                        Rect roiHair(x1, y1-hairlen, x2-x1, hairlen);
                        rectangle(frame, roiHair, Scalar(255, 0, 0), 2); //HERE tabestoon
                        if(isDominantColor(frame, roiHair, 0.5)) {
                            this->faceFeatures.hairColor = this->dominantColor;
                        }                      

                        // متن روی تصویر
                        string label = this->faceFeatures.gender + ", " + this->faceFeatures.age + ", " + this->faceFeatures.race
                                        + ", " + this->faceFeatures.hairColor + ", " + this->faceFeatures.eyesColor;
                        putText(frame, label, Point(x1, y1 - 10),
                                FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 0, 0), 2); //HERE tabestoon
                        this->success = true;
                    } else {
                        // cout << "in distant" << endl;
                        Rect roiCloth(x1, y1, x2 - x1, (frame.rows-y1)*ROI_RATIO);
                        if(isDominantColor(frame, roiCloth, 0.5)) {
                            //here
                            this->setClothColor(this->dominantColor);
                            this->success = true;
                            break;
                        }
                    }
                }

                imshow("Face, Age & Gender Detection", frame); //HERE tabestoon
                if (waitKey(1) == 27) {
                    this->setTask(TURN_OFF);
                }
                // if(!detectionMat.empty()) {
                //     success = true; //HERE
                // }
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
}

bool Camera::getSuccess() const {
    // return true; //HERE june8
    return success;
}

Camera::FaceFeatures Camera::getFaceFeatures() const {
    return this->faceFeatures;
}
