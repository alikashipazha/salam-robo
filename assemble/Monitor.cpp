#include "Monitor.h"

using namespace std;

Monitor::Monitor(const string& videosDir)
    : videosDirectory(videosDir),
      currentStream(""),
      currentTask(Task::IDLE),
      streamFinished(false),
      playerPid(-1),
      streamStartTime(0) {}

string Monitor::getVideosDirectory() const {
    return videosDirectory;
}

string Monitor::getAudiosDirectory() const {
    return audiosDirectory;
}

void Monitor::setStream(const string& streamPath) {
    currentStream = streamPath;
    streamFinished = false;
    // زمان شروع وقتی واقعا پخش کردیم تنظیم میشه
}

void Monitor::setTask(Task task) {
    if (task == Task::PLAY && currentTask != Task::PLAY) {
        startPlayback();
    } else if (task == Task::STOP && currentTask == Task::PLAY) {
        stopPlayback();
        currentTask = Task::IDLE; // همینجا حالت رو IDLE کن
        return;
    }
    currentTask = task;
}

bool Monitor::getStreamFinished() {
    if (currentTask == Task::PLAY) {
        if (playerPid > 0) {
            // بررسی اینکه آیا فرآیند هنوز وجود داره یا نه
            int ret = kill(playerPid, 0);
            if (ret == -1) {
                // پروسس وجود نداره، یعنی تموم شده
                streamFinished = true;
                currentTask = Task::IDLE;
                playerPid = -1;
            }
        } else {
            streamFinished = true;
            currentTask = Task::IDLE;
        }
    }
    return streamFinished;
}

void Monitor::setMode(Mode mode) {
    this->currentMode = mode;
}

void Monitor::update() {
    switch (currentTask) {
        case Task::IDLE:
            // کاری نمی‌کنیم
            break;
        case Task::PLAY:
            getStreamFinished(); // آپدیت وضعیت ویدیو
            break;
        case Task::STOP:
            stopPlayback();
            currentTask = Task::IDLE;
            break;
    }
}

void Monitor::startPlayback() {
    if (currentStream.empty()) return;

    string command;
    if(this->currentMode == VIDEO) {
        // دستور پخش با omxplayer، PID پروسس رو بگیر
        command = "mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen \"" + this->currentStream + "\" & echo $!";
    } else {
        command = "mpv --no-video --really-quiet \"" + this->currentStream + "\" & echo $!";
    }


    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        streamFinished = true;
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

    streamFinished = false;
    streamStartTime = millis();
}

void Monitor::stopPlayback() {
    if (playerPid > 0) {
        kill(playerPid, SIGTERM);
        playerPid = -1;
    }
    streamFinished = true;
}

unsigned long Monitor::millis() {
    using namespace chrono;
    static auto start = steady_clock::now();
    auto now = steady_clock::now();
    return duration_cast<milliseconds>(now - start).count();
}
