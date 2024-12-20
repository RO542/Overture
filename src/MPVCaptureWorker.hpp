#include <QObject>
#include <QThread>
#include <QString>
#include <iostream>

#pragma once
#include "../deps/mpv/client.h"
#include "platform/string_compare_insensitive.h"

#include "enum/MPVObservedProps.hpp"


class MPVEventGenerator: public QObject {
    Q_OBJECT
public:
    explicit MPVEventGenerator(QObject *parent);
    ~MPVEventGenerator();

    //FIXME: this is a duplicate enum of the same name in player
    // it should actually be put into a shared name space
    
public slots:
    void pollEvents(mpv_handle *mpv_ctx);
    void forwardPlaybackUpdates(mpv_event *event);

    // void fileEnded(int64_t ended_file_id, int64_t new_file_id, int mpv_playlist_count);

    // thes functions forward changes from the mpv event loop to qt's event loop for UI renderin
signals:
    // int onPauseChange();
    void pauseChanged(int pauseState);
    void percentChanged(double percent);
    void fileLoaded();
    void fileEnded();
    void timeChanged(double time);
    void durationChanged(double duration);
    void metadataChanged(const char *artist, const char *title);

private:

};


