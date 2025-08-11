#ifndef MONITOR_H
#define MONITOR_H

#include <string>
#include <cstdlib>   // system
#include <cstdio>    // popen, pclose
#include <unistd.h>  // pid_t, kill
#include <signal.h>  // SIGTERM
#include <chrono>
#include <thread>

#include <iostream>
#include <termios.h>

using namespace std;

#define INTERSTREAM_SLEEP_TIME 0

class Monitor {
public:
    enum Task {
        SC_SAVER,
        READY2PLAY,
        PLAY,
        STOP,
    };

    enum Mode {
        VIDEO,
        AUDIO,
        GIF,
    };

private:
    string videosDirectory;
    string audiosDirectory;
    string currentStream;
    string scSaverGif;
    Task currentTask;
    Mode currentMode;
    Mode previousMode;
    bool streamFinished;

    pid_t playerPid;  // PID فرآیند پلیر
    pid_t playerPids[3];
    unsigned long streamStartTime; // زمان شروع پخش

public:
    explicit Monitor(const string& videosDir, const string& audiosDir, const string& sc_saver_gif);

    string getVideosDirectory() const;
    string getAudiosDirectory() const;

    void setStream(const string& streamPath);
    void setTask(Task task);
    bool getStreamFinished();
    void setMode(Mode mode);

    void update();
    void startPlayback();
    
private:
    void stopPlayback(Mode mode);
    unsigned long millis();
};

#endif // MONITOR_H
