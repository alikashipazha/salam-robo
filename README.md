string command = "mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen \"" + currentVideo + "\" & echo $!";

mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen "/home/salam-robo/test/video/first.mp4"

salam-robo@raspberrypi:~/test $ g++ Sonar2.cpp sonarTest2.cpp -o test4 `pkg-config --cflags --libs opencv4`
/usr/bin/ld: /tmp/ccc8Ckvx.o: in function `Sonar2::Sonar2(int, int)':
Sonar2.cpp:(.text+0x2c): undefined reference to `wiringPiSetupGpio'
/usr/bin/ld: Sonar2.cpp:(.text+0x3c): undefined reference to `pinMode'
/usr/bin/ld: Sonar2.cpp:(.text+0x4c): undefined reference to `pinMode'
/usr/bin/ld: /tmp/ccc8Ckvx.o: in function `Sonar2::getSuccess() const':
Sonar2.cpp:(.text+0x70): undefined reference to `digitalRead'
/usr/bin/ld: /tmp/ccc8Ckvx.o: in function `Sonar2::getFail() const':
Sonar2.cpp:(.text+0x9c): undefined reference to `digitalRead'
/usr/bin/ld: /tmp/ccSgM8W1.o: in function `main':
sonarTest2.cpp:(.text+0x8): undefined reference to `wiringPiSetupGpio'
/usr/bin/ld: sonarTest2.cpp:(.text+0xe8): undefined reference to `delay'
collect2: error: ld returned 1 exit status

