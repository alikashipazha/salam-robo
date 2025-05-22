string command = "mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen \"" + currentVideo + "\" & echo $!";

mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen "/home/salam-robo/test/video/first.mp4"

salam-robo@raspberrypi:~/test/assemble $ g++ Monitor.cpp Camera.cpp Sonar.cpp FSM.cpp  main.cpp -o main1 `pkg-config --cflags --libs opencv4`
Monitor.cpp:5:1: error: no declaration matches ‘Monitor::Monitor(const std::string&, const std::string&)’
    5 | Monitor::Monitor(const string& videosDir, const string& audiosDir)
      | ^~~~~~~
In file included from Monitor.cpp:1:
Monitor.h:13:7: note: candidates are: ‘Monitor::Monitor(Monitor&&)’
   13 | class Monitor {
      |       ^~~~~~~
Monitor.h:13:7: note:                 ‘Monitor::Monitor(const Monitor&)’
Monitor.h:38:14: note:                 ‘Monitor::Monitor(const std::string&)’
   38 |     explicit Monitor(const string& videosDir);
      |              ^~~~~~~
Monitor.h:13:7: note: ‘class Monitor’ defined here
   13 | class Monitor {
      |       ^~~~~~~
Camera.cpp: In function ‘void setTimer()’:
Camera.cpp:57:5: error: ‘idleStartTime’ was not declared in this scope
   57 |     idleStartTime = std::chrono::steady_clock::now();
      |     ^~~~~~~~~~~~~
Camera.cpp: At global scope:
Camera.cpp:60:6: error: variable or field ‘setNextTask’ declared void
   60 | void setNextTask(Task nextTask) {
      |      ^~~~~~~~~~~
Camera.cpp:60:18: error: ‘Task’ was not declared in this scope
   60 | void setNextTask(Task nextTask) {
      |                  ^~~~
Camera.cpp: In member function ‘void Camera::update()’:
Camera.cpp:249:37: error: ‘class Camera’ has no member named ‘nextTask’; did you mean ‘getTask’?
  249 |                 this->setTask(this->nextTask);
      |                                     ^~~~~~~~
      |                                     getTask
cc1plus: fatal error: Sonar.cpp: No such file or directory
compilation terminated.
FSM.cpp: In member function ‘void FSM::update(const Sonar2&, const Camera&, const Monitor&)’:
FSM.cpp:35:31: error: passing ‘const Camera’ as ‘this’ argument discards qualifiers [-fpermissive]
   35 |                 camera.setTask(Camera::Task::IDLE);
      |                 ~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~
In file included from FSM.h:6,
                 from FSM.cpp:1:
Camera.h:46:10: note:   in call to ‘void Camera::setTask(Task)’
   46 |     void setTask(Task t);
      |          ^~~~~~~
FSM.cpp:36:32: error: passing ‘const Camera’ as ‘this’ argument discards qualifiers [-fpermissive]
   36 |                 camera.setTimer();
      |                 ~~~~~~~~~~~~~~~^~
Camera.h:49:10: note:   in call to ‘void Camera::setTimer()’
   49 |     void setTimer();
      |          ^~~~~~~~
FSM.cpp:37:35: error: passing ‘const Camera’ as ‘this’ argument discards qualifiers [-fpermissive]
   37 |                 camera.setNextTask(Camera::Task::FACE_DETECTION);
      |                 ~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Camera.h:50:10: note:   in call to ‘void Camera::setNextTask(Task)’
   50 |     void setNextTask(Task nextTask);
      |          ^~~~~~~~~~~
FSM.cpp:39:34: error: passing ‘const Monitor’ as ‘this’ argument discards qualifiers [-fpermissive]
   39 |                 monitor.setStream(cd);
      |                 ~~~~~~~~~~~~~~~~~^~~~
In file included from FSM.h:7:
Monitor.h:43:10: note:   in call to ‘void Monitor::setStream(const std::string&)’
   43 |     void setStream(const string& streamPath);
      |          ^~~~~~~~~
FSM.cpp:40:32: error: passing ‘const Monitor’ as ‘this’ argument discards qualifiers [-fpermissive]
   40 |                 monitor.setTask(Monitor::Task::PLAY);
      |                 ~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~
Monitor.h:44:10: note:   in call to ‘void Monitor::setTask(Task)’
   44 |     void setTask(Task task);
      |          ^~~~~~~
FSM.cpp:43:46: error: cannot convert ‘Camera::Mode’ to ‘Camera::Task’
   43 |                 camera.setTask(Camera::Mode::CLOSE_UP);
      |                                ~~~~~~~~~~~~~~^~~~~~~~
      |                                              |
      |                                              Camera::Mode
Camera.h:46:23: note:   initializing argument 1 of ‘void Camera::setTask(Task)’
   46 |     void setTask(Task t);
      |                  ~~~~~^
FSM.cpp:53:73: error: ‘const class Camera’ has no member named ‘getGender’
   53 |     string cd = monitor.getVideosDirectory() + '/' + camera.getGender() + '/' + camera.getAge();
      |                                                             ^~~~~~~~~

FSM.cpp:53:100: error: ‘const class Camera’ has no member named ‘getAge’
   53 | etVideosDirectory() + '/' + camera.getGender() + '/' + camera.getAge();
      |                                                               ^~~~~~

FSM.cpp:54:34: error: passing ‘const Monitor’ as ‘this’ argument discards qualifiers [-fpermissive]
   54 |                 monitor.setStream(cd);
      |                 ~~~~~~~~~~~~~~~~~^~~~
Monitor.h:43:10: note:   in call to ‘void Monitor::setStream(const std::string&)’
   43 |     void setStream(const string& streamPath);
      |          ^~~~~~~~~
FSM.cpp:55:32: error: passing ‘const Monitor’ as ‘this’ argument discards qualifiers [-fpermissive]
   55 |                 monitor.setTask(Monitor::Task::PLAY);
      |                 ~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~
Monitor.h:44:10: note:   in call to ‘void Monitor::setTask(Task)’
   44 |     void setTask(Task task);
      |          ^~~~~~~
FSM.cpp:58:46: error: cannot convert ‘Camera::Mode’ to ‘Camera::Task’
   58 |                 camera.setTask(Camera::Mode::DISTANT);
      |                                ~~~~~~~~~~~~~~^~~~~~~
      |                                              |
      |                                              Camera::Mode
Camera.h:46:23: note:   initializing argument 1 of ‘void Camera::setTask(Task)’
   46 |     void setTask(Task t);
      |                  ~~~~~^
FSM.cpp:66:73: error: ‘const class Camera’ has no member named ‘getGender’
   66 |     string cd = monitor.getVideosDirectory() + '/' + camera.getGender() + '/' + camera.getCloth();
      |                                                             ^~~~~~~~~

FSM.cpp:66:100: error: ‘const class Camera’ has no member named ‘getCloth’
   66 | VideosDirectory() + '/' + camera.getGender() + '/' + camera.getCloth();
      |                                                             ^~~~~~~~

FSM.cpp:67:25: error: ‘const class Monitor’ has no member named ‘setVideo’
   67 |                 monitor.setVideo(cd);
      |                         ^~~~~~~~
FSM.cpp:68:32: error: passing ‘const Monitor’ as ‘this’ argument discards qualifiers [-fpermissive]
   68 |                 monitor.setTask(Monitor::Task::PLAY);
      |                 ~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~
Monitor.h:44:10: note:   in call to ‘void Monitor::setTask(Task)’
   44 |     void setTask(Task task);
      |          ^~~~~~~
FSM.cpp:75:43: error: ‘const class Monitor’ has no member named ‘getVideoFinished’; did you mean ‘getStreamFinished’?
   75 |             if(sonar.getFail() || monitor.getVideoFinished() || watchdog(60000)) { // HERE: cam condition?
      |                                           ^~~~~~~~~~~~~~~~
      |                                           getStreamFinished
FSM.cpp:82:24: error: ‘const class Monitor’ has no member named ‘getVideoFinished’; did you mean ‘getStreamFinished’?
   82 |             if(monitor.getVideoFinished() || watchdog(60000)) // ignore this if condition for now
      |                        ^~~~~~~~~~~~~~~~
      |                        getStreamFinished
main.cpp: In function ‘int main()’:
main.cpp:17:87: error: no matching function for call to ‘Monitor::Monitor(const char [31], const char [31])’
   17 | or("/home/salam-robo/test/assemble", "/home/salam-robo/test/assemble");
      |                                                                      ^

In file included from main.cpp:7:
Monitor.h:38:14: note: candidate: ‘Monitor::Monitor(const std::string&)’
   38 |     explicit Monitor(const string& videosDir);
      |              ^~~~~~~
Monitor.h:38:14: note:   candidate expects 1 argument, 2 provided
Monitor.h:13:7: note: candidate: ‘Monitor::Monitor(const Monitor&)’
   13 | class Monitor {
      |       ^~~~~~~
Monitor.h:13:7: note:   candidate expects 1 argument, 2 provided
Monitor.h:13:7: note: candidate: ‘Monitor::Monitor(Monitor&&)’
Monitor.h:13:7: note:   candidate expects 1 argument, 2 provided
