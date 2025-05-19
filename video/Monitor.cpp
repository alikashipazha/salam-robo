#include "Monitor.h"

using namespace std;

Monitor::Monitor(const string& videosDir)
    : videosDirectory(videosDir),
      currentVideo(""),
      currentTask(Task::IDLE),
      videoFinished(false),
      playerPid(-1),
      videoStartTime(0) {}

string Monitor::getVideosDirectory() const {
    return videosDirectory;
}

void Monitor::setVideo(const string& videoPath) {
    currentVideo = videoPath;
    videoFinished = false;
    // زمان شروع وقتی واقعا پخش کردیم تنظیم میشه
}

void Monitor::setTask(Task task) {
    if (task == Task::PLAY_VIDEO && currentTask != Task::PLAY_VIDEO) {
        startPlayback();
    } else if (task == Task::STOP_VIDEO && currentTask == Task::PLAY_VIDEO) {
        stopPlayback();
        currentTask = Task::IDLE; // همینجا حالت رو IDLE کن
        return;
    }
    currentTask = task;
}

bool Monitor::getVideoFinished() {
    if (currentTask == Task::PLAY_VIDEO) {
        if (playerPid > 0) {
            // بررسی اینکه آیا فرآیند هنوز وجود داره یا نه
            int ret = kill(playerPid, 0);
            if (ret == -1) {
                // پروسس وجود نداره، یعنی تموم شده
                videoFinished = true;
                currentTask = Task::IDLE;
                playerPid = -1;
            }
        } else {
            videoFinished = true;
            currentTask = Task::IDLE;
        }
    }
    return videoFinished;
}

void Monitor::update() {
    switch (currentTask) {
        case Task::IDLE:
            // کاری نمی‌کنیم
            break;
        case Task::PLAY_VIDEO:
            getVideoFinished(); // آپدیت وضعیت ویدیو
            break;
        case Task::STOP_VIDEO:
            stopPlayback();
            currentTask = Task::IDLE;
            break;
    }
}

void Monitor::startPlayback() {
    if (currentVideo.empty()) return;

    // دستور پخش با omxplayer، PID پروسس رو بگیر
    string command = "omxplayer -o local \"" + currentVideo + "\" & echo $!";

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        videoFinished = true;
        return;
    }

    char buffer[128];
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        try {
            playerPid = stoi(buffer);
        } catch (...) {
            playerPid = -1;
        }
    } else {
        playerPid = -1;
    }
    pclose(pipe);

    videoFinished = false;
    videoStartTime = millis();
}

void Monitor::stopPlayback() {
    if (playerPid > 0) {
        kill(playerPid, SIGTERM);
        playerPid = -1;
    }
    videoFinished = true;
}

unsigned long Monitor::millis() {
    using namespace chrono;
    static auto start = steady_clock::now();
    auto now = steady_clock::now();
    return duration_cast<milliseconds>(now - start).count();
}
