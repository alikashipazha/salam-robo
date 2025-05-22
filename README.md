string command = "mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen \"" + currentVideo + "\" & echo $!";

mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen "/home/salam-robo/test/video/first.mp4"

salam-robo@raspberrypi:~/test/assemble $ g++ Monitor.cpp Camera.cpp Sonar.cpp FSM.cpp  main.cpp -o main1 `pkg-config --cflags --libs opencv4`
cc1plus: fatal error: Sonar.cpp: No such file or directory
compilation terminated.
FSM.cpp: In member function ‘void FSM::update(const Sonar2&, Camera&, Monitor&)’:
FSM.cpp:43:46: error: cannot convert ‘Camera::Mode’ to ‘Camera::Task’
   43 |                 camera.setTask(Camera::Mode::CLOSE_UP);
      |                                ~~~~~~~~~~~~~~^~~~~~~~
      |                                              |
      |                                              Camera::Mode
In file included from FSM.h:6,
                 from FSM.cpp:1:
Camera.h:48:23: note:   initializing argument 1 of ‘void Camera::setTask(Task)’
   48 |     void setTask(Task t);
      |                  ~~~~~^
FSM.cpp:58:46: error: cannot convert ‘Camera::Mode’ to ‘Camera::Task’
   58 |                 camera.setTask(Camera::Mode::DISTANT);
      |                                ~~~~~~~~~~~~~~^~~~~~~
      |                                              |
      |                                              Camera::Mode
Camera.h:48:23: note:   initializing argument 1 of ‘void Camera::setTask(Task)’
   48 |     void setTask(Task t);
      |                  ~~~~~^
FSM.cpp:66:100: error: ‘class Camera’ has no member named ‘getCloth’
   66 | VideosDirectory() + '/' + camera.getGender() + '/' + camera.getCloth();
      |                                                             ^~~~~~~~

FSM.cpp:67:25: error: ‘class Monitor’ has no member named ‘setVideo’
   67 |                 monitor.setVideo(cd);
      |                         ^~~~~~~~
FSM.cpp:75:43: error: ‘class Monitor’ has no member named ‘getVideoFinished’; did you mean ‘getStreamFinished’?
   75 |             if(sonar.getFail() || monitor.getVideoFinished() || watchdog(60000)) { // HERE: cam condition?
      |                                           ^~~~~~~~~~~~~~~~
      |                                           getStreamFinished
FSM.cpp:82:24: error: ‘class Monitor’ has no member named ‘getVideoFinished’; did you mean ‘getStreamFinished’?
   82 |             if(monitor.getVideoFinished() || watchdog(60000)) // ignore this if condition for now
      |                        ^~~~~~~~~~~~~~~~
      |                        getStreamFinished
