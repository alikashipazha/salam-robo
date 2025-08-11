#include "Camera.h"
#include <iostream>
#include <string>

#define CONFIDENCE_THRESHOLD 0.7
#define CLOTH_THRESHOLD 0.7
#define ROI_RATIO 2/3

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
    
    this->ComposureCounter = 1;
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

cv::Vec3b Camera::calculateBGRMean(const cv::Mat& image, const cv::Rect& roi) {
    // Crop the region of interest (ROI) from the image
    cv::Mat roiImage = image(roi);

    // Calculate the mean of the BGR values
    cv::Scalar meanScalar = cv::mean(roiImage);

    // Convert the mean value to Vec3b for BGR
    cv::Vec3b meanBGR(static_cast<uchar>(meanScalar[0]), 
                      static_cast<uchar>(meanScalar[1]), 
                      static_cast<uchar>(meanScalar[2]));

    return meanBGR;
}

std::tuple<std::string, std::string, std::string> Camera::processFrame(cv::Mat& frame, cv::Rect roi) {
    // تقسیم ROI به سه بخش
    // cout << "here in processframe" << endl;
    int roiWidth = roi.width / 3;
    cv::Rect roiLeft(roi.x, roi.y, roiWidth, roi.height);  // بخش چپ
    cv::Rect roiCenter(roi.x + roiWidth, roi.y, roiWidth, roi.height);  // بخش وسط
    cv::Rect roiRight(roi.x + 2 * roiWidth, roi.y, roiWidth, roi.height);  // بخش راست
    // cout << "pf 2" << endl;

    // if(this->getMode() == DISTANT) {
    //     // اعمال افزایش روشنایی به تصویر
    //     frame.convertTo(frame, -1, 1, -50);  // افزایش روشنایی
    // } else {
    //     frame.convertTo(frame, -1, 1, -20);  // افزایش روشنایی
    // }
    // frame.convertTo(frame, -1, 1, -50);
    try {    
        // محاسبه میانگین BGR برای هر بخش
        cv::Vec3b avgBGRLeft = calculateBGRMean(frame, roiLeft);
        cv::Vec3b avgBGRCenter = calculateBGRMean(frame, roiCenter);
        cv::Vec3b avgBGRRight = calculateBGRMean(frame, roiRight);
        
        // کشیدن مستطیل برای هر بخش
        cv::rectangle(frame, roiLeft, cv::Scalar(0, 255, 0), 2);  // مستطیل برای بخش چپ
        cv::rectangle(frame, roiCenter, cv::Scalar(0, 0, 255), 2);  // مستطیل برای بخش وسط
        cv::rectangle(frame, roiRight, cv::Scalar(255, 0, 0), 2);  // مستطیل برای بخش راست

        // نمایش میانگین رنگ BGR برای هر بخش
        std::cout << "Left Section BGR: (" 
                  << static_cast<int>(avgBGRLeft[0]) << ", "
                  << static_cast<int>(avgBGRLeft[1]) << ", "
                  << static_cast<int>(avgBGRLeft[2]) << ")" << std::endl;
        
        std::cout << "Center Section BGR: (" 
                  << static_cast<int>(avgBGRCenter[0]) << ", "
                  << static_cast<int>(avgBGRCenter[1]) << ", "
                  << static_cast<int>(avgBGRCenter[2]) << ")" << std::endl;
        
        std::cout << "Right Section BGR: (" 
                  << static_cast<int>(avgBGRRight[0]) << ", "
                  << static_cast<int>(avgBGRRight[1]) << ", "
                  << static_cast<int>(avgBGRRight[2]) << ")" << std::endl;

        // دریافت رنگ شناسایی شده با استفاده از متد classifyColor
        std::string colorNameLeft = classifyColor(avgBGRLeft);  // گرفتن اسم رنگ برای بخش چپ
        std::string colorNameCenter = classifyColor(avgBGRCenter);  // گرفتن اسم رنگ برای بخش وسط
        std::string colorNameRight = classifyColor(avgBGRRight);  // گرفتن اسم رنگ برای بخش راست
        
        // // نمایش اسم رنگ‌ها
        // std::cout << "Left Section Color: " << colorNameLeft << std::endl;
        // std::cout << "Center Section Color: " << colorNameCenter << std::endl;
        // std::cout << "Right Section Color: " << colorNameRight << std::endl;

        // برگرداندن میانگین رنگ‌ها از سه بخش
        return std::make_tuple(colorNameLeft, colorNameCenter, colorNameRight);
    } catch (const cv::Exception& e) {
        return std::make_tuple("unknown", "unknown", "unknown");
    }
}

std::string Camera::classifyColor(cv::Vec3b bgr) { //new HERE tabestoon
	
    int b = bgr[0]; // Blue component
    int g = bgr[1]; // Green component
    int r = bgr[2]; // Red component

    // تبدیل BGR به HSV
    Mat bgrMat(1, 1, CV_8UC3, Scalar(b, g, r)); // BGR در OpenCV
    Mat hsvMat;
    cvtColor(bgrMat, hsvMat, COLOR_BGR2HSV);  // تبدیل BGR به HSV

    // استخراج مقادیر HSV
    Vec3b hsv = hsvMat.at<Vec3b>(0, 0);
    int h = hsv[0];  // Hue
    int s = hsv[1];  // Saturation
    int v = hsv[2];  // Value

    // نمایش مقادیر HSV (اختیاری برای اشکال‌زدایی)
    // cout << "HSV values - H: " << h << " S: " << s << " V: " << v << endl;

    int max_val = max({r, g, b});  // بیشترین مقدار بین r، g و b
    int min_val = min({r, g, b});  // کمترین مقدار بین r، g و b
    int mid_val = r + g + b - max_val - min_val;

    // محاسبه اختلاف max و min
    int maxDiff = max_val - min_val;  // اختلاف بین max و min
    int upDiff = max_val - mid_val;
    int downDiff = mid_val - min_val;
    // cout << "max_val: " << max_val << " mid_val: " << mid_val << " min_val: " << min_val << endl;
    // cout << "downDiff: " << downDiff << " upDiff: " << upDiff << " maxDiff: " << maxDiff << endl;
    
    // red blue green 255 or 0 ?

    if((max_val < 50) || (maxDiff < 10 && max_val <= 100)) {
        return "black";
    }

    if (v > 90 && s < 10) {  // اگر Value زیاد باشد و Saturation کم باشد
        return "white";
    }

    // تشخیص رنگ خاکی (Gray)
    if (v > 90 && s < 40 && v < 150) {  // اگر Value زیاد باشد و Saturation کم باشد ولی Value کمتر از 150
        return "gray";
    }

    // if(maxDiff < 10) {
    //     if(max_val <= 100) {
    //         return "black";
    //     }
    //     if(min_val >= 160) {
    //         return "whtie";
    //     }
    //     if(min_val > 100 && max_val < 160) {
    //         return "gray";
    //     }
    // }

    if(max_val > 90 && upDiff > 20 && maxDiff > 20) { //(max_val > 130 && upDiff > 20) ||    
        if(max_val == r) {
            return "red";
        }
        if(max_val == b) {
            return "blue";
        }
        if(max_val == g) {
            return "green";
        }
    }

    if(max_val <= 100) {
        if(max_val == r && mid_val == g && upDiff > 20) {
            return "brown";
        }
        if(max_val == g && upDiff > 20) {
            return "green";
        }
    }

    if(upDiff < 30 && maxDiff > 2*upDiff) {
        if(min_val == g) {           
            if((max_val > 170 && max_val == r) || r > 210) {
                return "pink";
            }
            return "purple";
        }


        if(min_val == b) {
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
        if(max_val == r && r < 120 && upDiff > 70) {
            //nmd;
        }
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

    // HSV

    // تشخیص رنگ قهوه‌ای (brown) بر اساس HSV
    if (h >= 10 && h < 30 && s > 40 && v > 100 && v < 150) {
        return "brown";
    }

    // تشخیص رنگ قرمز (red) بر اساس Hue
    if (h >= 0 && h < 10 && s > 100) {
        return "red";
    }

    // تشخیص رنگ سبز (green) بر اساس Hue
    if (h >= 35 && h < 85 && s > 40) {
        return "green";
    }

    // تشخیص رنگ آبی (blue) بر اساس Hue
    if (h >= 85 && h < 175 && s > 40) {
        return "blue";
    }

    // تشخیص رنگ نارنجی (orange)
    if (h >= 10 && h < 35 && s > 100 && v >= 150) {
        return "orange";
    }

    // تشخیص رنگ زرد (yellow)
    if (h >= 35 && h < 65 && s > 100) {
        return "yellow";
    }

    // تشخیص رنگ بنفش (purple)
    if (h >= 125 && h < 175 && s > 40) {
        return "purple";
    }

    // صورتی (pink)
    if (h >= 160 && h < 180 && s > 100 && v > 180) {
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
//outdated NOT
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

bool Camera::openCam(cv::VideoCapture& cap, int camID) {
    if(camID > 10) {
        return false;
    }
    cap.open(camID);
    if (!cap.isOpened()) {
           openCam(cap, camID+1);
    } else {
        return true;
    }
}

void Camera::update() {
    while(true) {
    switch (currentTask) {
        case TURN_ON:
            // cap.open(0);  // باز کردن وبکم پیشفرض10 -> 0
            // if (!cap.isOpened()) {
            //         cerr << "couldn't open camera" << endl;
            //     } else {
            //         cout << "Camera opened successfully.\n" << endl;;
            //         this->setTask(FACE_DETECTION);
            //     }
            // }
            if(openCam(cap, 0)) {
                cout << "Camera opened successfully." << endl;;
                this->setTask(FACE_DETECTION);
            } else {
                cerr << "couldn't open camera." << endl;
            }
            break;

        case FACE_DETECTION: {
            if (!cap.isOpened()) {
                cerr << "Camera is not opened!\n";
                break;
            }
            Mat frame;
            cap >> frame;
            // frame.convertTo(frame, -1, 1, 50);  // HERE tabestoon

            if (frame.empty()) {
                cerr << "Blank frame grabbed\n";
                break;
            }

            // تشخیص چهره
            Mat blob = dnn::blobFromImage(frame, 1.0, Size(300, 300),
                                            Scalar(104.0, 177.0, 123.0), true, false); //5th arg was false HERE tabestoon
            faceNet.setInput(blob);
            Mat detections = faceNet.forward();
            Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());

            for (int i = 0; i < detectionMat.rows; i++) {
                float confidence = detectionMat.at<float>(i, 2);
                // ComposureCounter = 1; //hemaghat
                if (confidence > CONFIDENCE_THRESHOLD && !((ComposureCounter++)%5)) { //HERE tabestoon 20 mordad
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
                        cout << "male: " << genderPreds.at<float>(0) << " | female: " << genderPreds.at<float>(1) << endl;
                        int genderIdx = genderPreds.at<float>(1) >= 0.01 ? 1 : 0; //genderPreds.at<float>(1) ? 0 : 1;
                        this->faceFeatures.gender = genderList[genderIdx];

                        // تشخیص سن
                        ageNet.setInput(faceBlob);
                        Mat agePreds = ageNet.forward();
                        double ageConf;
                        Point classNumber;
                        // this->ageList = { "(0-2)", "(4-6)", "(8-12)", "(15-20)",
                        // "(25-32)", "(38-43)", "(48-53)", "(60-100)" };
                        for (int i = 0; i < 8; i++) {
                            std::cout << "agePreds[" << i << "] = " << agePreds.at<float>(i) << std::endl;
                        }
                        if(agePreds.at<float>(4) + agePreds.at<float>(5) + agePreds.at<float>(6) + agePreds.at<float>(7) > 0.5) {
                            // float summedPreds = agePreds.at<float>(3) + agePreds.at<float>(4) + agePreds.at<float>(5) 
                            //                     + agePreds.at<float>(6) + agePreds.at<float>(7);
                            // float oldPreds = agePreds.at<float>(6) + agePreds.at<float>(7);
                            if((agePreds.at<float>(6) + agePreds.at<float>(7)) > (agePreds.at<float>(3) + agePreds.at<float>(4) + agePreds.at<float>(5))) { //oldPreds/summedPreds > 0.6) {
                                this->faceFeatures.age = "old";
                            } else {
                                this->faceFeatures.age = "adult";
                            }
                        } else {
                            bool probablyKid = false;
                            if(faceBox.y < frame.rows/2) {
                                probablyKid = true;
                            }
                            float summedPreds = agePreds.at<float>(0) + agePreds.at<float>(1) + agePreds.at<float>(2)
                                                + agePreds.at<float>(3) + agePreds.at<float>(4) + agePreds.at<float>(5);
                            // float kidPreds = agePreds.at<float>(0) + agePreds.at<float>(1); //+ agePreds.at<float>(2);
                            if((agePreds.at<float>(2)/summedPreds > 0.3 && probablyKid) 
                                || (agePreds.at<float>(0) + agePreds.at<float>(1))/summedPreds > 0.6) { //oldPreds/summedPreds > 0.6) {
                                this->faceFeatures.age = "kid";
                            } else {
                                this->faceFeatures.age = "adult";
                            }
                        }
                        //     this->faceFeatures.age = "old";
                        // } else if(agePreds.at<float>(3) + agePreds.at<float>(4) + agePreds.at<float>(5) + agePreds.at<float>(6)  > 0.6) {
                        //     this->faceFeatures.age = "adult";
                        // } else {
                        //     this->faceFeatures.age = "kid";
                        // }


                        // minMaxLoc(agePreds, NULL, &ageConf, NULL, &classNumber);
                        // this->faceFeatures.age = ageList[classNumber.x];
                        // if (this->faceFeatures.age == "(4-6)") { // this->faceFeatures.age == "(0-2)" || this->faceFeatures.age == "(8-12)"
                        //     this->faceFeatures.age = "kid";
                        // } else if (this->faceFeatures.age == "(60-100)") {
                        //     this->faceFeatures.age = "old";
                        // } else {
                        //     this->faceFeatures.age = "adult";
                        // }
                        
                        
                        //cout << "race\n" << endl;
                        //race
                        Rect roiFace(x1+(x2-x1)/10, y1+(y2-y1)/10, 4*(x2-x1)/5, 4*(y2-y1)/5); //HERE tabestoon 8/10
                
                        // پردازش فریم‌ها از طریق متد processFrame
                        auto [colorNameLeft, colorNameCenter, colorNameRight] = processFrame(frame, roiFace);
                        cout << "face color : \ncnl = " << colorNameLeft << "\ncnc = " << colorNameCenter << "\ncnr = " << colorNameRight << endl;
                        int countBlack = 0;
                        if(colorNameLeft == "black" || colorNameLeft == "brown") {
                            countBlack++;
                        }
                        if(colorNameCenter == "black" || colorNameCenter == "brown") {
                            countBlack++;
                        }
                        if(colorNameRight == "black" || colorNameRight == "brown") {
                            countBlack++;
                        }
                        
                        if(countBlack > 3) {
                            this->faceFeatures.race = "black";
                        } else {
                            this->faceFeatures.race = "caucasian";
                        }

						//HERE 13 mordad
                        // //cout << "eyes\n" << endl;
                        // // تشخیص چشم ها
                        // // std::vector<std::vector<Point2f>> landmarks;
                        // std::vector<Point2f> landmark;
                        // std::vector<Rect> faces = { faceBox };
                        // std::vector<std::vector<Point2f>> landmarks;

                        // bool facemarkSuccess = facemark->fit(frame, faces, landmarks);
                        // if (facemarkSuccess && !landmarks.empty()) {
                        //     landmark = landmarks[0]; // فقط اولین صورت
                        //     //cout << "n(p) = " << landmark.size() << endl;
                        //     // سپس مستطیل عنبیه چشم‌ها را استخراج کن:
                        //     Rect leftIris = getIrisRect(landmark, 38, 39, 42, 41);
                        //     Rect rightIris = getIrisRect(landmark, 44, 45, 48, 47);
                        //     if(isDominantColor(frame, leftIris, 0.4)) {
                        //         this->faceFeatures.eyesColor = this->dominantColor;
                        //     } else if(isDominantColor(frame, rightIris, 0.4)) {
                        //         this->faceFeatures.eyesColor = this->dominantColor;
                        //     } else {
                        //         this->faceFeatures.eyesColor = "nan";
                        //     }
                        // }
						
						
                        //cout << "hair\n" << endl;
                        // تشخیص مو ها
                        int hairlen = (y2-y1)/8; // /6
                        Rect roiHair(x1+(x2-x1)/10, y1-hairlen, 4*(x2-x1)/5, hairlen);
                        // rectangle(frame, roiHair, Scalar(255, 0, 0), 2); //HERE tabestoon
                        auto [colorHairLeft, colorHairCenter, colorHairRight] = processFrame(frame, roiHair);
                        int countYellowHair = 0;
                        int countBlackHair = 0;
                        int countGrayHair = 0;
                        int countOrangeHair = 0;
                        
                        if(colorHairLeft == "yellow") {
                            countYellowHair++;
                        }
                        if(colorHairCenter == "yellow") {
                            countYellowHair++;
                        }
                        if(colorHairRight == "yellow") {
                            countYellowHair++;
                        }

                        if(colorHairLeft == "black" || colorHairLeft == "brown") {
                            countBlackHair++;
                        }
                        if(colorHairCenter == "black" || colorHairCenter == "brown") {
                            countBlackHair++;
                        }
                        if(colorHairRight == "black" || colorHairRight == "brown") {
                            countBlackHair++;
                        }

                        if(colorHairLeft == "gray" || colorHairLeft == "white") {
                            countGrayHair++;
                        }
                        if(colorHairCenter == "gray" || colorHairCenter == "white") {
                            countGrayHair++;
                        }
                        if(colorHairRight == "gray" || colorHairRight == "white") {
                            countGrayHair++;
                        }

                        if(colorHairLeft == "red" || colorHairLeft == "orange") {
                            countOrangeHair++;
                        }
                        if(colorHairCenter == "red" || colorHairCenter == "orange") {
                            countOrangeHair++;
                        }
                        if(colorHairRight == "red" || colorHairRight == "orange") {
                            countOrangeHair++;
                        }
                        
                        if(countYellowHair > 1) {
                            this->faceFeatures.hairColor = "blonde";
                        } else if(countOrangeHair > 1) {
                            this->faceFeatures.hairColor = "ginger";
                        } else if(countBlackHair > 1) {
                            this->faceFeatures.hairColor = "black_brown";
                        } else if(countGrayHair > 1) {
                            this->faceFeatures.hairColor = "gray_white";
                        } else {
                            this->faceFeatures.hairColor = "unknown";
                        }

                        // متن روی تصویر
                        string label = this->faceFeatures.gender + ", " + this->faceFeatures.age + ", " + this->faceFeatures.race
                                        + ", " + this->faceFeatures.hairColor + ", " + this->faceFeatures.eyesColor;
                        putText(frame, label, Point(x1, y1 - 10),
                                FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 0, 0), 2); //HERE tabestoon
                        this->success = true;
                    } else {
                        // cout << "in distant" << endl; //y2+0.5(y2-y1) = 1.5y2 - 0.5y1
                        Rect roiCloth(1.5*x1 - 0.5*x2, 1.5*y2 - 0.5*y1, 2*(x2 - x1), (frame.rows-y2)*ROI_RATIO);
                        auto [colorClothLeft, colorClothCenter, colorClothRight] = processFrame(frame, roiCloth);
                        int countBlackCloth = 0;
                        int countBlueCloth = 0;
                        int countRedCloth = 0;
                        int countYellowCloth = 0;
                        int countGreenCloth = 0;
                        int countWhiteCloth = 0;

                        if (colorClothLeft == "black") countBlackCloth++;
                        if (colorClothCenter == "black") countBlackCloth++;
                        if (colorClothRight == "black") countBlackCloth++;

                        if (colorClothLeft == "blue") countBlueCloth++;
                        if (colorClothCenter == "blue") countBlueCloth++;
                        if (colorClothRight == "blue") countBlueCloth++;

                        if (colorClothLeft == "red") countRedCloth++;
                        if (colorClothCenter == "red") countRedCloth++;
                        if (colorClothRight == "red") countRedCloth++;

                        if (colorClothLeft == "yellow") countYellowCloth++;
                        if (colorClothCenter == "yellow") countYellowCloth++;
                        if (colorClothRight == "yellow") countYellowCloth++;

                        if (colorClothLeft == "green") countGreenCloth++;
                        if (colorClothCenter == "green") countGreenCloth++;
                        if (colorClothRight == "green") countGreenCloth++;

                        if (colorClothLeft == "white") countWhiteCloth++;
                        if (colorClothCenter == "white") countWhiteCloth++;
                        if (colorClothRight == "white") countWhiteCloth++;

                        if(countBlackCloth > 2) {
                            this->setClothColor("black");
                            this->success = true;
                            // break;
                        } else if(countBlueCloth > 2) {
                            this->setClothColor("blue");
                            this->success = true;
                            // break;
                        } else if(countRedCloth > 2) {
                            this->setClothColor("red");
                            this->success = true;
                            // break;
                        } else if(countYellowCloth > 2) {
                            this->setClothColor("yellow");
                            this->success = true;
                            // break;
                        } else if(countGreenCloth > 2) {
                            this->setClothColor("green");
                            this->success = true;
                            // break;
                        } else if(countWhiteCloth > 2) {
                            this->setClothColor("white");
                            this->success = true;
                            // break;
                        } else {
                            this->setClothColor("unknown");
                            this->success = true;
                            // break;
                        }
                        // cout << this->getClothColor() << endl;
                        // متن روی تصویر
                        string label = this->clothColor;
                        putText(frame, label, Point(x1, y1 - 10),
                                FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 0, 0), 2);
                    }
                }
                // cout << "show frame" << endl;
                imshow("Face, Age & Gender Detection", frame); //HERE tabestoon
                // cout << "CAMERA: current mode: " << this->getMode() << endl;
                if (waitKey(1) == 27) {
                    break;
                    // this->setTask(TURN_OFF);
                } //HERE tabestoon commented
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
            if (elapsed >= 5) //HERE tabestoon ???
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
