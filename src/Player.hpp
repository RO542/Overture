#pragma once
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <QObject>
#include <QThread>

#include "../deps/mpv/client.h"
#include "enum/MPVObservedProps.hpp"
#include "MPVCaptureWorker.hpp"



class MPVPlayer : public QObject {
    Q_OBJECT
public:
    MPVPlayer(QObject *parent);
    ~MPVPlayer();
    mpv_handle *mpv_ctx;
    double m_volume;
    bool m_paused = true; // FIXME: the player currently doesn't need to track this bt it might be useful later
    std::mutex m_devices_mutex;
    std::unordered_map<std::string, std::string> m_devices; //TODO: mutex so background thread can update this 

    void cyclePause();
    void pausePlayback();
    void unpausePlayback();
    void loadSong(
        std::string filepath = 
        "C:/Users/richa/Code_Projects/prototype-music-player/test_music/Altare & Bafu - Lost.opus"
        // "https://soundcloud.com/linkinpark/figure-09-1"
        // "C:/Users/richa/Code_Projects/prototype-music-player/test_music/Altare - Surge.opus"
    );

    // "https://soundcloud.com/linkinpark/figure-09-1"
    // "https://soundcloud.com/cosmicsand/9h00-preview"
    // "https://www.youtube.com/watch?v=ra9UgspPmXE"
    // "C:/Users/richa/Code_Projects/prototype-music-player/test_music/Bluebird.mp3"
    // "https://soundcloud.com/forgetbeats/waves"
    void queueSong(std::string filepath);
    void enumerateDevices(); // TODO: only void for now later will set internal devicesMap
    void setVolume(double volume);
    void setAudioDevice(std::string device);
    void seekRelative(double amount);
    void seekAbsolutePercent(double percent);
    void playlistClear();
    void playlistNext();

    // typedef enum PLAYBACK_MODE {
    //     SINGLE_SONG, // immediately clear the playlist and append-play this one
    //     PLAYLIST, // append-play the full playlist upfront and play until context switch or reached end
    //     ALBUM, // treat just like PLAYLIST
    //     GENERAL_SONG_LIST, // use the two at a time approach
    // } PLAYBACK_MODE;

    // enum LOOP_MODE : int {
    //     NONE, 
    //     CURRENT_FILE,
    //     CURRENT_PLAYLIST
    // };

    MPVEventGenerator *worker = new MPVEventGenerator(nullptr);

private:
    QThread *pollingThread = new QThread();
};
