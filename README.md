string command = "mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen \"" + currentVideo + "\" & echo $!";

mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen "/home/salam-robo/test/video/first.mp4"

salam-robo@raspberrypi:~/test/salam-robo-main $ g++ Camera.cpp camTest_28_5.cpp -o testCam -lwiringPi `pkg-config --cflags --libs opencv4`
Camera.cpp: In constructor ‘Camera::Camera()’:
Camera.cpp:21:29: error: could not convert ‘((Camera*)this)->Camera::facemark.cv::Ptr<cv::face::Facemark>::operator->()->cv::face::Facemark::loadModel(std::__cxx11::basic_string<char>(((const char*)"lbfmodel.yaml"), std::allocator<char>()))’ from ‘void’ to ‘bool’
   21 |     if (!facemark->loadModel("lbfmodel.yaml")) {
      |          ~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~
      |                             |
      |                             void
Camera.cpp:21:29: error: in argument to unary !
Camera.cpp:23:16: error: returning a value from a constructor
   23 |         return -1;
      |                ^~
Camera.cpp: At global scope:
Camera.cpp:326:1: error: ‘FaceFeatures’ does not name a type; did you mean ‘CpuFeatures’?
  326 | FaceFeatures Camera::getFaceFeatures() const {
      | ^~~~~~~~~~~~
      | CpuFeatures
