#ifndef STRING_COMPARE_INSENSITIVE_H_
#define STRING_COMPARE_INSENSITIVE_H_

#include <string.h>
#ifdef _WIN32
    static inline int strcmp_no_case(const char *s1, const char *s2) {
        return _stricmp(s1, s2);
    }
#else // assume POSIX 
    static inline int strcmp_no_case(const char *s1, const char *s2) {
        return strcasecmp(s1, s2);
    }
#endif


#endif // STRING_COMPARE_INSENSITIVE_H_