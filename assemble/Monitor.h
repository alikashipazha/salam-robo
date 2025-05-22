#ifndef MONITOR_H
#define MONITOR_H

#include <string>
#include <cstdlib>   // system
#include <cstdio>    // popen, pclose
#include <unistd.h>  // pid_t, kill
#include <signal.h>  // SIGTERM
#include <chrono>

using namespace std;

class Monitor {
public:
    enum Task {
        IDLE,
        PLAY,
        STOP,
    };

    enum Mode {
        VIDEO,
        AUDIO
    };

private:
    string videosDirectory;
    string audiosDirectory;
    string currentStream;
    Task currentTask;
    Mode currentMode;
    bool streamFinished;

    pid_t playerPid;  // PID فرآیند پلیر
    unsigned long streamStartTime; // زمان شروع پخش

public:
    explicit Monitor(const string& videosDir, const string& audiosDir);

    string getVideosDirectory() const;
    string getAudiosDirectory() const;

    void setStream(const string& streamPath);
    void setTask(Task task);
    bool getStreamFinished();
    void setMode(Mode mode);

    void update();

private:
    void startPlayback();
    void stopPlayback();
    unsigned long millis();
};

#endif // MONITOR_H
