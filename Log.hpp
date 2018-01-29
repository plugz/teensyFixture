#ifndef __LOG_HPP__
#define __LOG_HPP__

#define LOGLEVEL_NONE 0
#define LOGLEVEL_ERROR 1
#define LOGLEVEL_WARNING 2
#define LOGLEVEL_INFO 3
#define LOGLEVEL_DEBUG 4
#define LOGLEVEL_VERBOSE 5

#ifndef LOGLEVEL
#define LOGLEVEL LOGLEVEL_NONE
#endif

#if (LOGLEVEL > 0)

#include <HardwareSerial.h>

#define LOG(LVL, ...)                                                                              \
    do {                                                                                           \
        if (LOGLEVEL >= LVL) {                                                                     \
            Serial.print(__VA_ARGS__);                                                             \
        }                                                                                          \
    } while (0);

#define LOGLN(LVL, ...)                                                                            \
    do {                                                                                           \
        if (LOGLEVEL >= LVL) {                                                                     \
            Serial.println(__VA_ARGS__);                                                           \
        }                                                                                          \
    } while (0);

#define LOGSETUP()                                                                                 \
    do {                                                                                           \
        Serial.begin(115200);                                                                      \
    } while (0);

#else

#define LOG(...)                                                                                   \
    do {                                                                                           \
    } while (0);

#define LOGLN(...)                                                                                 \
    do {                                                                                           \
    } while (0);

#define LOGSETUP(...)                                                                              \
    do {                                                                                           \
    } while (0);

#endif

#define LOG_ERROR(...) LOG(LOGLEVEL_ERROR, __VA_ARGS__)
#define LOG_WARNING(...) LOG(LOGLEVEL_WARNING, __VA_ARGS__)
#define LOG_INFO(...) LOG(LOGLEVEL_INFO, __VA_ARGS__)
#define LOG_DEBUG(...) LOG(LOGLEVEL_DEBUG, __VA_ARGS__)
#define LOG_VERBOSE(...) LOG(LOGLEVEL_VERBOSE, __VA_ARGS__)

#define LOGLN_ERROR(...) LOGLN(LOGLEVEL_ERROR, __VA_ARGS__)
#define LOGLN_WARNING(...) LOGLN(LOGLEVEL_WARNING, __VA_ARGS__)
#define LOGLN_INFO(...) LOGLN(LOGLEVEL_INFO, __VA_ARGS__)
#define LOGLN_DEBUG(...) LOGLN(LOGLEVEL_DEBUG, __VA_ARGS__)
#define LOGLN_VERBOSE(...) LOGLN(LOGLEVEL_VERBOSE, __VA_ARGS__)

#endif
