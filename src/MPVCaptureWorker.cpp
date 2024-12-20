#include "MPVCaptureWorker.hpp"

MPVEventGenerator::MPVEventGenerator(QObject *parent) : QObject(parent) {

}

MPVEventGenerator::~MPVEventGenerator() {
    std::cout << "Destroying MPVCapture Worker\n";
}

void MPVEventGenerator::forwardPlaybackUpdates(mpv_event *event) {
    if (!event || !event->data) {
        std::cout << "Recieved event or event data that is null";
        return;
    }

    mpv_event_property *prop = (mpv_event_property *)event->data;
    if (prop->format == MPV_FORMAT_NONE) {
        // std::cout << "Got a FORMAT_NONE property observation\n";
        return;
    }


    switch (event->reply_userdata) {
    case reply_prop::NONE: {
        return;
    }

    case reply_prop::PERCENT: {
        assert(prop->data != NULL);
        double out_percent = *(double *)prop->data;
        emit percentChanged(out_percent);
        break;
    }

    case reply_prop::DURATION: {
        assert(prop->data != NULL);
        assert(prop->format == MPV_FORMAT_DOUBLE);
        double out_d = *(double *)prop->data;
        emit durationChanged(out_d);
    }

    case reply_prop::PAUSE: {
        int *pause_state = (int *)prop->data;
        assert(pause_state);
        int out_pause_state = *pause_state;
        emit pauseChanged(out_pause_state);
        break;
    } 

    case reply_prop::VOLUME: {
        double out_d = *(double *)prop->data;
        std::cout << "VOLUME: " <<  out_d <<"\n";
        break;
    }

    case reply_prop::DEVICE: {
        // std::cout << "DEVICE\n";
        break;
    }

    case reply_prop::TIME: {
        // FIXME: this if is necessary possibly becauase when mpv_observe_property is called on a property I think it is changed
        // dereferencing the property data at that point is a NULL deref. so to fix this consider adding to the Player class a boolean m_initialized or something 
        if (prop->format == MPV_FORMAT_DOUBLE) {
            double *time = (double *)prop->data;
            if (time != NULL) {
                double out_time = *time;
                emit timeChanged(out_time);
            }
        }
        break;
    }

    case reply_prop::METADATA: {
        std::cout << "reply property metadata \n" ;
        if (prop->format != MPV_FORMAT_NODE) {
            // std::cout << "format is not MPV_FORMAT_NODE it is " << prop->format << "\n";
            return;
        }
        mpv_node *node = (mpv_node *)prop->data;
        assert(node);
        // std::cout << "Format of this mpv_event_property data is " << prop->format << "\n";
        // std::cout << "Format of the node itself is " << node->format << "\n";
        mpv_node_list* node_list = (mpv_node_list *)node->u.list;
        assert(node_list);
        // std::cout << "Got node_list with " << node_list->num   << " entries\n";
        // std::cout << "Correctly converted property data to node with format " << node->format << "\n";

        int len_metadata = node_list->num;
        if (len_metadata <= 0) {
            std::cout << "got mpv_event_property data member as FORMAT_NODE but 0 length\n"; 
            return;
        } 
        const char *out_title = "", *out_artist = "";
        for (int i = 0; i < len_metadata; i++) {
            const char *key = node_list->keys[i];
            mpv_node *value = &node_list->values[i];
            // std::cout << key << "  " << value->u.string << "\n";
            // std::cout << "Inner node with format " << value->format <<  "\n";
            assert(value->format == MPV_FORMAT_STRING);
            if (strcmp_no_case(key, "artist") == 0) {
                // std::cout << key << "  " << value->u.string << "\n"; 
                out_artist = value->u.string;
            } 
            if (strcmp_no_case(key, "title") == 0) {
                // std::cout << key << "  " << value->u.string << "\n"; 
                out_title = value->u.string;
            }
        }
        emit metadataChanged(out_artist, out_title);
        break;
    } 

    default:
        std::cout << "UNKNOWN reply_prop: " << event->reply_userdata << "\n";
        assert(false);
    }
}



void MPVEventGenerator::pollEvents(mpv_handle *mpv_ctx) {
    mpv_event *event;
    bool player_quit = false;
    while (!player_quit) {
        event = mpv_wait_event(mpv_ctx, 250); //TODO: test out different values for wait times
        if (event->error) {
            std::cout << "In mpv loop mpv_error: " << event->error << "\n";
        }
        switch (event->event_id) {
        case MPV_EVENT_NONE:
            break;

        case MPV_EVENT_PROPERTY_CHANGE: {
            forwardPlaybackUpdates(event);
            break;
        }

        case MPV_EVENT_FILE_LOADED: {
            emit fileLoaded();
            break;
        }

        case MPV_EVENT_START_FILE: {
            mpv_event_start_file *start_file_event = (mpv_event_start_file *)event->data;
            assert(start_file_event);
            int64_t new_file_id = start_file_event->playlist_entry_id;
            //TODO: emit to player
            std::cout << "mpv playlist new_file_id " << new_file_id << "\n";
        }
    
        case MPV_EVENT_END_FILE: {
            std::cout << "END FILE EVENT\n";
            mpv_event_end_file *end_event  = (mpv_event_end_file *)event->data;
            if (end_event->error < 0) {
                std::cout << "Got MPV_EVENT_END_FILE error: " << mpv_error_string(end_event->error) << "\n";
            }

            // TODO: resolve file ending 
            // switch (end_event->reason) {
            //    case MPV_END_FILE_REASON_EOF:
            //    case MPV_END_FILE_REASON_STOP:
            //    case MPV_END_FILE_REASON_QUIT:
            //    case MPV_END_FILE_REASON_REDIRECT:

            //    case MPV_END_FILE_REASON_ERROR:
            // }

            //TODO: emit to Player class
            // int64_t playlist_count;
            // mpv_get_property(mpv_ctx, "playlist-count", MPV_FORMAT_INT64, &playlist_count);
            // std::cout << "end_file_event  playlist-count: " << playlist_count << "\n";

            //int64_t last_pl_id = end_event->playlist_entry_id;
            //int64_t next_pl_id = end_event->playlist_insert_id;
            //int pl_num_entries = end_event->playlist_insert_num_entries;
            // std::cout << "last id: " << last_pl_id << "\n";
            // std::cout << "next id: " << next_pl_id << "\n";
            // std::cout << "entries: " << pl_num_entries << "\n";
            // FIXME: the player should set a slot for this signal
            // TODO: or different signals shold be emitted based ont eh vale of pl_num_entries
            // and the overall state of the player, looping ? etc.
            emit fileEnded();
            break;
        }

        case MPV_EVENT_SEEK: {
            std::cout << "seek event\n";
            //TODO: maybe this is useful if the seek is performed in a large network playback scenario
            // to change the buffering or caching behaviors
            break;
        }

        case MPV_EVENT_PLAYBACK_RESTART: {
            //possibly useful in some streaming scenarios
            std::cout << "Resuming playback\n";
            break;
        }

        case MPV_EVENT_SHUTDOWN: {
            std::cout << "Recieved event shutdown, exiting mpv_ctx\n";
            player_quit = true;
            break;
            //FIXME: this could just be a return, the thread must terminate eithe way
        }

        case MPV_EVENT_SET_PROPERTY_REPLY: {
            // TODO: use for device cchange 
            std::cout << "Reply for changing property\n";
            break;
        }

        case MPV_EVENT_GET_PROPERTY_REPLY: {
            // std::cout << "Get property async reply\n";
            // mpv_event_property *ret_property = (mpv_event_property *)event->data;
            // ret_property->
            break;
        }

        case MPV_EVENT_COMMAND_REPLY: {
            // std::cout << "Async mpv command reply, dispatching data to queue\n"; 
            // mpv_event_command *cmd = (mpv_event_command *)event->data;
            // mpv_node node = cmd->result;
            // std::cout << "Got mpv_event_command reply with node format of " << node.format << "\n";
            break;
        }

        case MPV_EVENT_QUEUE_OVERFLOW: {
            std::cout << "\n----------------------------------------------------------------------------\n";
            std::cout << "Fatal Error MPV QUEUE OVERFLOW: " << mpv_error_string(MPV_EVENT_QUEUE_OVERFLOW) << "\n";
            std::cout << "----------------------------------------------------------------------------\n";
            exit(MPV_EVENT_QUEUE_OVERFLOW);
        }
        // All other events are not needed for this application.
        // case MPV_EVENT_IDLE:
        // case MPV_EVENT_TICK:
        // case MPV_EVENT_HOOK:
        // case MPV_EVENT_LOG_MESSAGE:
        // case MPV_EVENT_CLIENT_MESSAGE:
        // case MPV_EVENT_VIDEO_RECONFIG:
        // case MPV_EVENT_AUDIO_RECONFIG:
        default: continue;
        }
    }
}
