#ifndef MPVOBbservedProps_H_
#define MPVOBbservedProps_H_
#include <cstdint>

enum reply_prop : uint64_t {
    NONE = 0, // used by mpv to signal there is no reply_userdata
    PAUSE, 
    VOLUME,
    DEVICE,
    TIME, 
    PERCENT,
    METADATA,
    DURATION
};


#endif // MPVOBbservedProps_H_