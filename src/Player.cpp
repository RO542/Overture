#include "Player.hpp"
#include <assert.h>

MPVPlayer::MPVPlayer(QObject *parent):
    QObject(parent)
{
    mpv_handle *ctx = mpv_create();
    assert(ctx);
    mpv_initialize(ctx);

    //TODO: error check these property
    int not_needed = 0;
    mpv_set_property(ctx, "terminal",       MPV_FORMAT_FLAG, &not_needed);
    mpv_set_property(ctx, "audio-display",  MPV_FORMAT_FLAG, &not_needed);
    mpv_set_property(ctx, "video",          MPV_FORMAT_FLAG, &not_needed);
    mpv_set_property(ctx, "osc",            MPV_FORMAT_FLAG, &not_needed);


    // mpv_set_option_string(ctx, "audio-out-callback", audio_callback);

    //TODO: error checking property observations
    mpv_observe_property(ctx, reply_prop::VOLUME,   "volume",        MPV_FORMAT_DOUBLE);
    mpv_observe_property(ctx, reply_prop::TIME,     "playback-time", MPV_FORMAT_DOUBLE);
    mpv_observe_property(ctx, reply_prop::PERCENT,  "percent-pos",   MPV_FORMAT_DOUBLE);
    mpv_observe_property(ctx, reply_prop::DEVICE,   "audio-device",  MPV_FORMAT_NODE);
    mpv_observe_property(ctx, reply_prop::PAUSE,    "pause",         MPV_FORMAT_FLAG);
    mpv_observe_property(ctx, reply_prop::METADATA, "metadata",      MPV_FORMAT_NODE);
    mpv_observe_property(ctx, reply_prop::DURATION, "duration",      MPV_FORMAT_DOUBLE);


    // mpv_observe_property(ctx, reply_prop::SEEK,     "seeking",       MPV_FORMAT_FLAG);
    // mpv_observe_property(ctx, reply_prop::LOOPFILE, "loop-file", MPV_FORMAT_FLAG);
    // mpv_observe_property(ctx, , "playlist-current-pos", MPV_FORMAT_INT64);
    // mpv_observe_property(ctx, , "playlist-count", MPV_FORMAT_INT64);

    this->mpv_ctx = ctx;
    this->setVolume(20);
    this->enumerateDevices();
    worker->moveToThread(pollingThread);
    connect(pollingThread, &QThread::started, [&]() {
        worker->pollEvents(mpv_ctx);
    });
    pollingThread->start();
}

MPVPlayer::~MPVPlayer() {
    pollingThread->quit();
    std::cout << "player and mpv_ctx destroyed\n"; 
    
    mpv_destroy(mpv_ctx);
    mpv_ctx = nullptr;
}


//TODO: better name
//FIXME: error check all the mpv_set_property and mpv_command calls
void MPVPlayer::loadSong(std::string filepath) {
    std::cout << "Loading filepath " << filepath << "\n";
    const char* loadfile_cmd[] = {"loadfile", filepath.c_str(), NULL};
    mpv_command(mpv_ctx, loadfile_cmd);
    m_paused = false;
}

//FIXME: 
void MPVPlayer::queueSong(std::string filepath) {
    std::cout << "Queueing filepath " << filepath << "\n";
    const char* loadfile_cmd[] = {"loadfile", filepath.c_str(), "append-play", NULL};
    mpv_command(mpv_ctx, loadfile_cmd);
    m_paused = false;
}

void MPVPlayer::pausePlayback() {
    std::cout << "pausing player ...\n";
    int pause = 1;
    mpv_set_property(mpv_ctx, "pause", MPV_FORMAT_FLAG, &pause);
    m_paused = true;
}

void MPVPlayer::unpausePlayback() {
    std::cout << "unpausing player ...\n";
    int pause = 0;
    mpv_set_property(mpv_ctx, "pause", MPV_FORMAT_FLAG, &pause);
    m_paused = false;
}

void MPVPlayer::cyclePause() {
    const char *cmd[] = {"cycle", "pause", NULL};
    mpv_command(this->mpv_ctx, cmd);
}

void MPVPlayer::setVolume(double volume) {
    m_volume = volume;
    mpv_set_property(mpv_ctx, "volume", MPV_FORMAT_DOUBLE, &volume);
}

void MPVPlayer::setAudioDevice(std::string device_name) {
    this->enumerateDevices();
    if (this->m_devices.find(device_name) != this->m_devices.end()) {
        std::cout << "Desired device not found\n";
        return;
    }
    int res = mpv_set_property(mpv_ctx, "device", MPV_FORMAT_STRING, (void *)device_name.c_str());
    if (res < 0) {
        std::cout << "Error in set_devices changing device to: " << device_name << "\n";
    }
}

void MPVPlayer::seekRelative(double amount) {
    char temp[16];
    snprintf(temp, 15, "%.2f", amount);
    const char *cmd_arr[] = {"seek", temp, "relative", NULL};
    mpv_command(mpv_ctx, cmd_arr);
}

void MPVPlayer::seekAbsolutePercent(double percent) {
    std::cout << "seeking by percent to " << percent << "\n";
    char temp_buffer[16];
    memset(temp_buffer, 0, 16);
    snprintf(temp_buffer, 15, "%.2f", percent);
    std::cout << "Called seekAbsolutePercent to % " << temp_buffer << "\n"; 
    const char *seek_cmd[] = {"seek", temp_buffer, "absolute-percent", NULL};
    mpv_command(mpv_ctx, seek_cmd);
}

// enumerate a list of audio devices mpv obtains from the OS
// the device list is an array of nodes, each node a key value pair
//FIXME: only one foor lop is needed
void MPVPlayer::enumerateDevices() {
    mpv_node device_node_list;
    int query_result = mpv_get_property(mpv_ctx, "audio-device-list", MPV_FORMAT_NODE, &device_node_list);
    if (query_result < 0 || device_node_list.format != MPV_FORMAT_NODE_ARRAY) {
        std::cout << "Found no audio devices or wrong query format in enumerate_devices FORMAT:" <<  device_node_list.format << "ERROR: " << query_result << "\n";
        return;
    }
    //FIXME: make the mutexed section smaller, also research if Qt's Mutex is more compatible here
    // maybe Qt mutex and QStrings as well
    std::map<std::string, std::string> new_dev_list;

    std::lock_guard<std::mutex> lock(m_devices_mutex);
    mpv_node_list *dev_list = device_node_list.u.list;
    // std::cout << "# of devices: " <<  dev_list->num << "\n";
    for (int i = 0; i < dev_list->num; i++) {
        mpv_node *device = &dev_list->values[i];
        if (device->format != MPV_FORMAT_NODE_MAP) {
            continue; 
        }         
        mpv_node_list *device_props = device->u.list;
        const char *name = NULL;
        const char *description = NULL;
        for (int j = 0; j < device_props->num; j++) {
            const char *key = device_props->keys[j];
            mpv_node *value = &device_props->values[j];
            if (strncmp(key, "name", sizeof("name")) == 0 && value->format == MPV_FORMAT_STRING) {
                name = value->u.string;
            } else if (strncmp(key, "description", sizeof("description")) == 0 && value->format == MPV_FORMAT_STRING) {
                description = value->u.string;
            }
        }

        if (name && description) {

            // std::cout << description << " -> " << name << "\n";
            new_dev_list[description] = name;
            m_devices[description] = name;
        }
    }

    // std::lock_guard<std::mutex> lock(m_devices_mutex);
    // m_devices.clear();

    
    mpv_free_node_contents(&device_node_list);
}

void MPVPlayer::playlistClear() {
    mpv_command_string(mpv_ctx, "playlist-clear");
}


void MPVPlayer::playlistNext() {
    mpv_command_string(mpv_ctx, "playlist-next");
}
