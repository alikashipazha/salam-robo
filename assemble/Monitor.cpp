#include "Monitor.h"
#include <iostream>

using namespace std;

Monitor::Monitor(const string& videosDir, const string& audiosDir, const string& sc_saver_gif)
    : videosDirectory(videosDir),
	  audiosDirectory(audiosDir),
      scSaverGif(sc_saver_gif),
      currentStream(""),
      currentMode(Mode::VIDEO), //HERE GIF
      currentTask(Task::SC_SAVER),
      streamFinished(false),
      playerPid(-1),
      streamStartTime(0) {
			cout << "Monitor constructed" << endl;
            cout << "Monitor: Mode = " << currentMode << endl;
            cout << "Monitor: Task = " << currentTask << endl;
		  }

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
    cout << "Monitor: new task is " << currentTask << endl;
    if (task == Task::PLAY && currentTask != Task::PLAY) {
        startPlayback();
    } else if (task == Task::STOP && currentTask == Task::PLAY) {
        stopPlayback();
        // currentTask = Task::SC_SAVER; // همینجا حالت رو SC_SAVER کن
        return;
    } 
    // else if (task == Task::SC_SAVER && currentTask != Task::SC_SAVER) { //HERE
    //     startPlayback();
    // } else if(task == Task::READY2PLAY && currentTask == Task::SC_SAVER) {
    //     stopPlayback();
    // }
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
                //currentTask = Task::SC_SAVER; //HERE
                playerPid = -1;
            }
        } else {
            streamFinished = true;
            //currentTask = Task::SC_SAVER; //HERE
        }
    }
    return streamFinished;
}

void Monitor::setMode(Mode mode) {
    this->currentMode = mode;
    cout << "Monitor: new mode is " << currentMode << endl;
}

void Monitor::update() {
    switch (currentTask) {
        // case Task::SC_SAVER: //HERE
        //     // کاری نمی‌کنیم
        //     break;
        // case Task::READY2PLAY:
        //     stopPlayback();
        //     break;
        case Task::PLAY:
            if(getStreamFinished()) { // آپدیت وضعیت ویدیو
                currentTask = Task::STOP;
            }
            break;
        case Task::STOP:
            stopPlayback();
            currentTask = Task::SC_SAVER;
            // currentMode = Mode::GIF; //HERE
            break;
    }
}

void Monitor::startPlayback() {
    if (currentStream.empty()) return;

    string command;
    if(this->currentMode == VIDEO) {
        // دستور پخش با omxplayer، PID پروسس رو بگیر
        //HERE
        command = "sh -c 'video=$(ls \"" + this->currentStream + "/\" | grep -E \"\\.(mp4|mkv|avi)$\" | head -1); mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen \"" + this->currentStream + "/$video\" & echo $!'";
        // command = "mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen \"" + this->currentStream + "\" & echo $!";
    } else if(this->currentMode == AUDIO) {
        command = "mpv --no-video --really-quiet \"" + this->currentStream + "\" & echo $!";
    } 
    // else { //HERE
    //     command = "mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen \"" + this->scSaverGif + "\" & echo $!";
    // }


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
