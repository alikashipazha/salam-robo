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
    enum class Task {
        IDLE,
        PLAY_VIDEO,
        STOP_VIDEO,
    };

private:
    string videosDirectory;
    string currentVideo;
    Task currentTask;
    bool videoFinished;

    pid_t playerPid;  // PID فرآیند پلیر
    unsigned long videoStartTime; // زمان شروع پخش

public:
    explicit Monitor(const string& videosDir);

    string getVideosDirectory() const;

    void setVideo(const string& videoPath);
    void setTask(Task task);
    bool getVideoFinished();

    void update();

private:
    void startPlayback();
    void stopPlayback();
    unsigned long millis();
};

#endif // MONITOR_H
