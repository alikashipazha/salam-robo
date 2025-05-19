string command = "mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen \"" + currentVideo + "\" & echo $!";

mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen "/home/salam-robo/test/video/first.mp4"

salam-robo@raspberrypi:~/test $ g++ Monitor.cpp Camera.cpp test.cpp -o test3 `pkg-config --cflags --libs opencv4`
test.cpp: In function ‘int main()’:
test.cpp:34:33: error: ‘this_thread’ has not been declared
   34 |                                 this_thread::sleep_for(chrono::milliseconds(500));
      |   
