#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <filesystem>
#include <string>
#include <regex>

using namespace std;
using namespace cv;
namespace fs = std::filesystem;

int main() {
    // مدل DNN برای تشخیص چهره
    string faceProto = "face_detector/opencv_face_detector.pbtxt";
    string faceModel = "face_detector/opencv_face_detector_uint8.pb";

    // بارگذاری مدل DNN
    dnn::Net faceNet = dnn::readNetFromTensorflow(faceModel, faceProto);

    // ; // بارگذاری مدل برای تشخیص ویژگی‌های صورت (مثل چشم‌ها، بینی و ...، اگر نیاز باشد)
    // ; FacemarkLBF::create();
    // ; facemark->loadModel("lbfmodel.yaml");

    // پوشه‌های تصاویر ورودی
    vector<string> partFolders = {"part1", "part2", "part3"};
    
    // پوشه برای ذخیره چهره‌های برش خورده
    string outputFolder = "trainImages";
    if (!fs::exists(outputFolder)) {
        fs::create_directory(outputFolder);
    }

    // حلقه برای خواندن تصاویر از پوشه‌های مختلف
    for (const auto& folder : partFolders) {
        for (const auto& entry : fs::directory_iterator(folder)) {
            if (entry.is_regular_file()) {
                string imagePath = entry.path().string();

                // خواندن تصویر
                Mat frame = imread(imagePath);
                if (frame.empty()) {
                    cerr << "Error loading image: " << imagePath << endl;
                    continue;
                }

                // تبدیل تصویر به blob برای پردازش با DNN
                Mat blob = dnn::blobFromImage(frame, 1.0, Size(300, 300), Scalar(104.0, 177.0, 123.0), true, false);
                faceNet.setInput(blob);
                Mat detections = faceNet.forward();
                Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());

                // پردازش هر چهره شناسایی شده
                for (int i = 0; i < detectionMat.rows; i++) {
                    float confidence = detectionMat.at<float>(i, 2);
                    if (confidence > 0.5) { // آستانه اطمینان
                        // مختصات چهره شناسایی شده
                        int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
                        int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
                        int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
                        int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

                        // محدود کردن مختصات
                        x1 = max(0, x1);
                        y1 = max(0, y1);
                        x2 = min(frame.cols - 1, x2);
                        y2 = min(frame.rows - 1, y2);

                        Rect faceBox(Point(x1, y1), Point(x2, y2));

                        // برش چهره از تصویر
                        Mat face = frame(faceBox);

                        // استخراج نام فایل از مسیر کامل
                        string fileName = entry.path().filename().string();

                        // ذخیره چهره برش خورده با همان نام اصلی
                        string outputFilePath = outputFolder + "/" + fileName;
                        imwrite(outputFilePath, face);

                        // رسم مستطیل دور چهره برای بررسی (اختیاری)
                        // rectangle(frame, faceBox, Scalar(0, 255, 0), 2);
                    }
                }
            }
        }
    }

    return 0;
}
