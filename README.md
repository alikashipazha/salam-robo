Camera.cpp: In member function ‘void Camera::update()’:
Camera.cpp:220:14: error: jump to case label
  220 |         case ADVANCED_FACE_DETECTION:
      |              ^~~~~~~~~~~~~~~~~~~~~~~
Camera.cpp:157:17: note:   crosses initialization of ‘cv::Mat detectionMat’
  157 |             Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());
      |                 ^~~~~~~~~~~~
Camera.cpp:156:17: note:   crosses initialization of ‘cv::Mat detections’
  156 |             Mat detections = faceNet.forward();
      |                 ^~~~~~~~~~
Camera.cpp:153:17: note:   crosses initialization of ‘cv::Mat blob’
  153 |             Mat blob = dnn::blobFromImage(frame, 1.0, Size(300, 300),
      |                 ^~~~
Camera.cpp:145:17: note:   crosses initialization of ‘cv::Mat frame’
  145 |             Mat frame;
      |                 ^~~~~
Camera.cpp:225:14: error: jump to case label
  225 |         case TURN_OFF:
      |              ^~~~~~~~
Camera.cpp:157:17: note:   crosses initialization of ‘cv::Mat detectionMat’
  157 |             Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());
      |                 ^~~~~~~~~~~~
Camera.cpp:156:17: note:   crosses initialization of ‘cv::Mat detections’
  156 |             Mat detections = faceNet.forward();
      |                 ^~~~~~~~~~
Camera.cpp:153:17: note:   crosses initialization of ‘cv::Mat blob’
  153 |             Mat blob = dnn::blobFromImage(frame, 1.0, Size(300, 300),
      |                 ^~~~
Camera.cpp:145:17: note:   crosses initialization of ‘cv::Mat frame’
  145 |             Mat frame;
      |                 ^~~~~
Camera.cpp:234:14: error: jump to case label
  234 |         case IDLE:
      |              ^~~~
Camera.cpp:157:17: note:   crosses initialization of ‘cv::Mat detectionMat’
  157 |             Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());
      |                 ^~~~~~~~~~~~
Camera.cpp:156:17: note:   crosses initialization of ‘cv::Mat detections’
  156 |             Mat detections = faceNet.forward();
      |                 ^~~~~~~~~~
Camera.cpp:153:17: note:   crosses initialization of ‘cv::Mat blob’
  153 |             Mat blob = dnn::blobFromImage(frame, 1.0, Size(300, 300),
      |                 ^~~~
Camera.cpp:145:17: note:   crosses initialization of ‘cv::Mat frame’
  145 |             Mat frame;
      |                 ^~~~~
