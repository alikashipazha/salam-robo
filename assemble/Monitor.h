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
        SC_SAVER,
        READY2PLAY,
        PLAY,
        STOP,
    };

    enum Mode {
        VIDEO,
        AUDIO,
        GIF
    };

private:
    string videosDirectory;
    string audiosDirectory;
    string currentStream;
    string scSaverGif;
    Task currentTask;
    Mode currentMode;
    bool streamFinished;

    pid_t playerPid;  // PID فرآیند پلیر
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
    void stopPlayback();
    unsigned long millis();
};

#endif // MONITOR_H
