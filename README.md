string command = "mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen \"" + currentVideo + "\" & echo $!";

mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen "/home/salam-robo/test/video/first.mp4"

salam-robo@raspberrypi:~/test/salam-robo-main $ g++ Camera.cpp camTest_28_5.cpp -o testCam -lwiringPi `pkg-config --cflags --libs opencv4`
In file included from Camera.cpp:1:
Camera.h:61:92: error: expected ‘;’ at end of member declaration
   61 | ect(const std::vector<Point2f>& points, int p1, int p2, int p3, int p4)
      |                                                                       ^
      |                                                                        ;
Camera.cpp: In constructor ‘Camera::Camera()’:
Camera.cpp:21:29: error: could not convert ‘facemark.cv::Ptr<cv::face::Facemark>::operator->()->cv::face::Facemark::loadModel(std::__cxx11::basic_string<char>(((const char*)"lbfmodel.yaml"), std::allocator<char>()))’ from ‘void’ to ‘bool’
   21 |     if (!facemark->loadModel("lbfmodel.yaml")) {
      |          ~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~
      |                             |
      |                             void
Camera.cpp:21:29: error: in argument to unary !
Camera.cpp:23:16: error: returning a value from a constructor
   23 |         return -1;
      |                ^~
Camera.cpp: At global scope:
Camera.cpp:107:6: error: no declaration matches ‘bool Camera::isDominantColor(cv::Mat&, cv::Rect, float)’
  107 | bool Camera::isDominantColor(cv::Mat& frame, Rect roi, float thresh) { //new
      |      ^~~~~~
Camera.h:60:10: note: candidate is: ‘bool Camera::isDominantColor(cv::Mat&, cv::Rect)’
   60 |     bool isDominantColor(cv::Mat& frame, Rect roi);
      |          ^~~~~~~~~~~~~~~
Camera.h:14:7: note: ‘class Camera’ defined here
   14 | class Camera {
      |       ^~~~~~
Camera.cpp: In member function ‘void Camera::update()’:
Camera.cpp:240:44: error: no matching function for call to ‘Camera::isDominantColor(cv::Mat&, cv::Rect&, double)’
  240 |                         if (isDominantColor(frame, roiFace, 0.8)) {
      |                             ~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~
Camera.h:60:10: note: candidate: ‘bool Camera::isDominantColor(cv::Mat&, cv::Rect)’
   60 |     bool isDominantColor(cv::Mat& frame, Rect roi);
      |          ^~~~~~~~~~~~~~~
Camera.h:60:10: note:   candidate expects 2 arguments, 3 provided
Camera.cpp:243:39: error: invalid use of ‘Camera::FaceFeatures’
  243 |                                 this->FaceFeatures.race = "black";
      |                                       ^~~~~~~~~~~~
Camera.cpp:253:48: error: ‘facemark’ was not declared in this scope
  253 |                         bool facemarkSuccess = facemark->fit(frame, faces, landmarks);
      |                                                ^~~~~~~~
Camera.cpp:259:47: error: no matching function for call to ‘Camera::isDominantColor(cv::Mat&, cv::Rect&, double)’
  259 |                             if(isDominantColor(frame, leftIris, 0.4)) {
      |                                ~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~
Camera.h:60:10: note: candidate: ‘bool Camera::isDominantColor(cv::Mat&, cv::Rect)’
   60 |     bool isDominantColor(cv::Mat& frame, Rect roi);
      |          ^~~~~~~~~~~~~~~
Camera.h:60:10: note:   candidate expects 2 arguments, 3 provided
Camera.cpp:261:54: error: no matching function for call to ‘Camera::isDominantColor(cv::Mat&, cv::Rect&, double)’
  261 |                             } else if(isDominantColor(frame, rightIris, 0.4)) {
      |                                       ~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~
Camera.h:60:10: note: candidate: ‘bool Camera::isDominantColor(cv::Mat&, cv::Rect)’
   60 |     bool isDominantColor(cv::Mat& frame, Rect roi);
      |          ^~~~~~~~~~~~~~~
Camera.h:60:10: note:   candidate expects 2 arguments, 3 provided
Camera.cpp:269:29: error: expected unqualified-id before ‘=’ token
  269 |                         int = (y2-y1)/5;
      |                             ^
Camera.cpp:270:45: error: ‘hairlen’ was not declared in this scope
  270 |                         Rect roiHair(x1, y1-hairlen, x2-x1, hairlen);
      |                                             ^~~~~~~
Camera.cpp:271:43: error: no matching function for call to ‘Camera::isDominantColor(cv::Mat&, cv::Rect&, double)’
  271 |                         if(isDominantColor(frame, roiHair, 0.5)) {
      |                            ~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~
Camera.h:60:10: note: candidate: ‘bool Camera::isDominantColor(cv::Mat&, cv::Rect)’
   60 |     bool isDominantColor(cv::Mat& frame, Rect roi);
      |          ^~~~~~~~~~~~~~~
Camera.h:60:10: note:   candidate expects 2 arguments, 3 provided
Camera.cpp:282:43: error: no matching function for call to ‘Camera::isDominantColor(cv::Mat&, cv::Rect&, double)’
  282 |                         if(isDominantColor(frame, roiCloth, 0.7)) {
      |                            ~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~
Camera.h:60:10: note: candidate: ‘bool Camera::isDominantColor(cv::Mat&, cv::Rect)’
   60 |     bool isDominantColor(cv::Mat& frame, Rect roi);
      |          ^~~~~~~~~~~~~~~
Camera.h:60:10: note:   candidate expects 2 arguments, 3 provided
Camera.cpp: At global scope:
Camera.cpp:326:1: error: ‘FaceFeatures’ does not name a type; did you mean ‘CpuFeatures’?
  326 | FaceFeatures Camera::getFaceFeatures() const {
      | ^~~~~~~~~~~~
      | CpuFeatures
In file included from camTest_28_5.cpp:2:
Camera.h:61:92: error: expected ‘;’ at end of member declaration
   61 | ect(const std::vector<Point2f>& points, int p1, int p2, int p3, int p4)
      |                                                                       ^
      |                                                                        ;
camTest_28_5.cpp: In function ‘int main()’:
camTest_28_5.cpp:31:14: error: ‘std::this_thread’ has not been declared
   31 |         std::this_thread::sleep_for(std::chrono::milliseconds(30));
      |              ^~~~~~~~~~~

