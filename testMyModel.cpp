#include <iostream>
#include <opencv2/opencv.hpp>
#include <tensorflow/core/public/session.h>
#include <tensorflow/core/platform/env.h>
#include <tensorflow/core/framework/tensor.h>

using namespace tensorflow;
using namespace std;

int main() {
    // Load the pre-trained model
    string model_path = "gender_age_race_model.h5";
    Session* session;
    Status status = NewSession(SessionOptions(), &session);
    if (!status.ok()) {
        cerr << "Error creating session: " << status.ToString() << endl;
        return -1;
    }

    // Load the model
    GraphDef graph_def;
    status = ReadBinaryProto(Env::Default(), model_path, &graph_def);
    if (!status.ok()) {
        cerr << "Error loading model: " << status.ToString() << endl;
        return -1;
    }
    status = session->Create(graph_def);
    if (!status.ok()) {
        cerr << "Error creating graph: " << status.ToString() << endl;
        return -1;
    }

    // Initialize webcam (use device ID 0 for default webcam)
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Error opening webcam" << endl;
        return -1;
    }

    // Define the input image size (224x224 as used in your model)
    int img_height = 224, img_width = 224;

    while (true) {
        // Capture frame-by-frame
        cv::Mat frame;
        cap >> frame; // Get a new frame from the webcam
        if (frame.empty()) {
            break; // Check if the frame is valid
        }

        // Resize the image to match the model input size
        cv::Mat resized_img;
        cv::resize(frame, resized_img, cv::Size(img_width, img_height));

        // Convert to float and normalize (divide by 255.0)
        resized_img.convertTo(resized_img, CV_32F, 1.0 / 255.0);

        // Convert to tensor
        Tensor input_tensor(DT_FLOAT, TensorShape({1, img_height, img_width, 3}));
        auto input_tensor_mapped = input_tensor.tensor<float, 4>();

        for (int y = 0; y < img_height; y++) {
            for (int x = 0; x < img_width; x++) {
                cv::Vec3f color = resized_img.at<cv::Vec3f>(y, x);
                input_tensor_mapped(0, y, x, 0) = color[2]; // Red
                input_tensor_mapped(0, y, x, 1) = color[1]; // Green
                input_tensor_mapped(0, y, x, 2) = color[0]; // Blue
            }
        }

        // Run inference
        vector<Tensor> outputs;
        status = session->Run({{"conv2d_input", input_tensor}}, 
                              {"gender_output", "age_output", "race_output"}, 
                              {}, &outputs);

        if (!status.ok()) {
            cerr << "Error during inference: " << status.ToString() << endl;
            return -1;
        }

        // Extract the predictions from the outputs
        auto gender_output = outputs[0].flat<float>();
        auto age_output = outputs[1].flat<float>();
        auto race_output = outputs[2].flat<float>();

        int gender_pred = (gender_output(0) > gender_output(1)) ? 0 : 1;
        int age_pred = distance(age_output.data(), max_element(age_output.data(), age_output.data() + age_output.size()));
        int race_pred = distance(race_output.data(), max_element(race_output.data(), race_output.data() + race_output.size()));

        // Display the predictions on the image
        string gender_str = (gender_pred == 0) ? "Male" : "Female";
        string age_str = "Age: " + to_string(age_pred);
        string race_str = "Race: " + to_string(race_pred);

        cv::putText(frame, gender_str, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);
        cv::putText(frame, age_str, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);
        cv::putText(frame, race_str, cv::Point(10, 90), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);

        // Show the image with predictions
        cv::imshow("Webcam - Gender, Age, Race Prediction", frame);

        // Break the loop on 'q' key press
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    // Release resources
    cap.release();
    cv::destroyAllWindows();
    session->Close();

    return 0;
}
