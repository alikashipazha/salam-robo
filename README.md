string command = "mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen \"" + currentVideo + "\" & echo $!";

mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen "/home/salam-robo/test/video/first.mp4"

salam-robo@raspberrypi:~/test/assemble $ ./main1 
terminate called after throwing an instance of 'cv::Exception'
  what():  OpenCV(4.6.0) ./modules/dnn/src/caffe/caffe_io.cpp:1138: error: (-2:Unspecified error) FAILED: fs.is_open(). Can't open "face_detector/opencv_face_detector_uint8.pb" in function 'ReadProtoFromBinaryFile'

Aborted
