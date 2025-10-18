#include <iostream>
#include <httplib.h>
#include <opencv2/opencv.hpp>
#include <vector>

int main() {
    // OpenCV: Capture the frame and detect the face (assume face detection is done)
    cv::VideoCapture cap(0);  // Use your camera
    if (!cap.isOpened()) {
        std::cerr << "Failed to open camera!" << std::endl;
        return -1;
    }

    // Detect face and crop (replace this with your own face detection code)
    cv::Mat frame;
    cap >> frame;
    cv::Rect faceRect(100, 100, 200, 200);  // Example rectangle where face was detected
    cv::Mat face = frame(faceRect);

    // Convert the cropped face to JPEG (send as byte data)
    std::vector<uchar> buf;
    cv::imencode(".jpg", face, buf);  // Encode face image to JPG
    std::string imgData(buf.begin(), buf.end());  // Convert to string for sending

    // Set up HTTP client to send the image data to the server (your laptop)
    httplib::Client client("http://<laptop_ip>:5000");  // Replace with your laptop's IP and port

    httplib::MultipartFormDataItems items;
    items.push_back({"image", imgData, "image.jpg", "image/jpeg"});

    // Send POST request with image data
    auto res = client.Post("/predict", items);
    if (res) {
        std::cout << "Response: " << res->body << std::endl;
    } else {
        std::cerr << "Request failed!" << std::endl;
    }

    return 0;
}
