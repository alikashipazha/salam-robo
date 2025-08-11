#include "Monitor.h"
#include <iostream>

using namespace std;

Monitor::Monitor(const string& videosDir, const string& audiosDir, const string& sc_saver_gif)
    : videosDirectory(videosDir),
	  audiosDirectory(audiosDir),
      scSaverGif(sc_saver_gif),
      currentStream(""),
      currentMode(Mode::GIF), //HERE GIF
      currentTask(Task::SC_SAVER), //HERE-JUNE : SC_SAVER bood . PLAY shod
      streamFinished(false),
      playerPid(-1),
      streamStartTime(0) {
			cout << "Monitor constructed" << endl;
            cout << "Monitor: Mode = " << currentMode << endl;
            cout << "Monitor: Task = " << currentTask << endl;
            // std::string command = "mpv --loop --fs " + this->scSaverGif + " &"; // استفاده از --fs برای نمایش فول‌اسکرین و --loop برای تکرار ویدیو
            

            //this one commented HERE 13 mordad
            std::string command = "mpv --loop --fs --mute " + this->scSaverGif + " &"; 




            // std::string command = "vlc --fullscreen --loop --no-audio " + this->scSaverGif + " &"; // استفاده از --fs برای نمایش فول‌اسکرین، --loop برای تکرار ویدیو و --mute برای پخش بی‌صدا
            // system(command.c_str()); //HERE june8
            startPlayback(); //HERE-JUNE

            
            // std::string command = "mpv --loop " + this->scSaverGif + " &"; // استفاده از & برای اجرا در پس‌زمینه
            // std::string command = "nohup mpv --loop " + this->scSaverGif + " &";
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
    cout << "Monitor: new task is " << task << " | current task is " << currentTask << endl;
    if (task == Task::PLAY) {//&& currentTask != Task::PLAY) { //HERE-JUNE
        if(this->currentMode == VIDEO) {
            cout << "MONITOR: task is play: video gif" << endl;
            stopPlayback(Mode::GIF);
            // startPlayback(); //HERE-JUNE
        }
        std::this_thread::sleep_for(std::chrono::seconds(INTERSTREAM_SLEEP_TIME)); //HERE tabestoon no such line here
        startPlayback();
        // if(this->currentMode == AUDIO) { //HERE-JUNE if only :  && this->currentTask != Task::PLAY
        //     startPlayback();
        // }
    } else if (task == Task::STOP) {// && currentTask == Task::PLAY) { //HERE-JUNE
        stopPlayback(this->currentMode);
        
        if(this->currentMode == VIDEO) {
            cout << "MONITOR: task is stop: video" << endl;
            this->currentMode = GIF;
            std::this_thread::sleep_for(std::chrono::seconds(INTERSTREAM_SLEEP_TIME)); //HERE tabestoon was 1
            startPlayback();
        }

        // currentTask = Task::SC_SAVER; // همینجا حالت رو SC_SAVER کن
        return;
    } 
    else if (task == Task::SC_SAVER && currentTask != Task::SC_SAVER) { //HERE-JUNE
        this->currentMode = Mode::GIF;
        std::this_thread::sleep_for(std::chrono::seconds(INTERSTREAM_SLEEP_TIME)); //HERE tabestoon no such line
        startPlayback();
    }
    else if(task == Task::READY2PLAY && currentTask == Task::SC_SAVER) {
        stopPlayback(Mode::GIF);
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
    if(this->currentMode != mode) { //HERE june8
        this->previousMode = currentMode;
        this->currentMode = mode;
    }
    cout << "Monitor: new mode is " << currentMode << " | previous mode is " << previousMode << endl;
}

void Monitor::update() {
    switch (currentTask) {
        case Task::SC_SAVER:
            // if (playerPids[Mode::GIF] == -1) { // اگر هنوز در حال پخش نیست
            //     startPlayback();
            // }
            break;
        case Task::READY2PLAY:
            if (playerPids[Mode::GIF] != -1) { // اگر هنوز GIF یا هر چیزی در حال پخش است، توقف بده
                stopPlayback(Mode::GIF);
            }
            break;
        case Task::PLAY:
            if(getStreamFinished()) { // آپدیت وضعیت ویدیو
                currentTask = Task::STOP;
            }
            break;
        case Task::STOP:
            stopPlayback(this->currentMode);
            currentTask = Task::SC_SAVER;
            // currentMode = Mode::GIF; //HERE
            break;
    }
}

void Monitor::startPlayback() {
    // if (currentStream.empty()) return; //HERE-JUNE

    string command;
    if(this->currentMode == VIDEO) {
        // دستور پخش با omxplayer، PID پروسس رو بگیر
        //HERE
        
        // stopPlayback(Mode::GIF); //HERE-JUNE


        command = "mpv --fullscreen --geometry=100%:100% \"$(find " + this->currentStream + " -name \"*.mp4\" | head -n 1)\" & echo $! > mpv_pid.txt";
        cout << command << endl;
        system(command.c_str());

        FILE* pidFile = fopen("mpv_pid.txt", "r");
        if (pidFile) {
            char buffer[128];
            if (fgets(buffer, sizeof(buffer), pidFile) != nullptr) {
                try {
                    playerPid = stoi(buffer);  // استخراج PID از فایل
                } catch (...) {
                    playerPid = -1;
                }
            } else {
                playerPid = -1;
            }
            fclose(pidFile);
        } else {
            playerPid = -1;
        }

        playerPids[Mode::VIDEO] = playerPid; // ذخیره PID
        streamFinished = false;
        streamStartTime = millis();

    } else if(this->currentMode == AUDIO) {
        // command = "mpv --no-video --really-quiet \"" + this->currentStream + "\" & echo $!";
        command = "ffplay -nodisp -loglevel quiet \"" + this->currentStream + "\" & echo $!";

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
        playerPids[this->currentMode] = playerPid; //HERE
        pclose(pipe);

        streamFinished = false;
        streamStartTime = millis();
    } else if(this->currentMode == GIF){ //HERE
        // stopPlayback(Mode::VIDEO); //HERE-JUNE
        cout << "MONITOR: in GIF" << endl; //HERE-JUNE
        //HERE-JUNE --mute HAZF SHOD. --volume=50 EZAFE SHOD
        std::string command = "mpv --loop --fs --volume=70 " + this->scSaverGif + " & echo $! > gif_pid.txt"; // استفاده از --fs برای نمایش فول‌اسکرین، --loop برای تکرار ویدیو و --mute برای پخش بی‌صدا
        system(command.c_str());

        FILE* pidFile = fopen("gif_pid.txt", "r");
        if (pidFile) {
            char buffer[128];
            if (fgets(buffer, sizeof(buffer), pidFile) != nullptr) {
                try {
                    playerPid = stoi(buffer);  // استخراج PID از فایل
                } catch (...) {
                    playerPid = -1;
                }
            } else {
                playerPid = -1;
            }
            fclose(pidFile);
        } else {
            playerPid = -1;
        }

        playerPids[Mode::GIF] = playerPid; // ذخیره PID
        streamFinished = false;
        streamStartTime = millis();


        // command = "mpv --loop " + this->scSaverGif + " &";
        
        
        // command = "mpv --no-terminal --audio-device=alsa/default --really-quiet --fullscreen \"" + this->scSaverGif + "\" & echo $!";
        // command = "mpv --loop --no-audio --really-quiet --fullscreen \"" + this->scSaverGif + "\" & echo $!";
        // command = "mpv --loop " + this->scSaverGif + " & echo $!";
    }

    // system(command.c_str());
    // FILE* pipe = popen(command.c_str(), "r");
    // if (!pipe) {
    //     streamFinished = true;
    //     return;
    // }

    // char buffer[128];
    // if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    //     try {
    //         playerPid = stoi(buffer);
    //     } catch (...) {
    //         playerPid = -1;
    //     }
    // } else {
    //     playerPid = -1;
    // }
    // playerPids[this->currentMode] = playerPid; //HERE
    // pclose(pipe);

    // streamFinished = false;
    // streamStartTime = millis();
}

void Monitor::stopPlayback(Mode mode) {
    pid_t pp = playerPids[mode]; //HERE
    if (pp > 0) {
        kill(pp, SIGTERM);
        pp = -1;
    }
    streamFinished = true;
}

unsigned long Monitor::millis() {
    using namespace chrono;
    static auto start = steady_clock::now();
    auto now = steady_clock::now();
    return duration_cast<milliseconds>(now - start).count();
}
