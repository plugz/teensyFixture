#ifndef __LOG_HPP__
#define __LOG_HPP__

#define LOGLEVEL_NONE 0
#define LOGLEVEL_ERROR 1
#define LOGLEVEL_WARNING 2
#define LOGLEVEL_INFO 3
#define LOGLEVEL_DEBUG 4
#define LOGLEVEL_VERBOSE 5

#ifndef LOGLEVEL
# define LOGLEVEL LOGLEVEL_NONE
#endif

#if (LOGLEVEL > 0)

//#include <HardwareSerial.h>
#include <Arduino.h>

// Serial7: RX on pin 28, TX on pin 29

#define LOG_SERIAL Serial

#define LOGLN(LVL, ...) do { \
    if (LOGLEVEL >= LVL) { \
        char buf[512]; \
        auto now = micros(); \
        auto position = snprintf(buf, sizeof(buf), "[%5lu.%06lu] ", now / 1000000, now % 1000000); \
        snprintf(buf + position, sizeof(buf) - position, __VA_ARGS__); \
        LOG_SERIAL.println(buf); \
    } \
} while (0);

//    LOG_SERIAL.setRX(28);
//    LOG_SERIAL.setTX(29);

#define LOGSETUP() do { \
    LOG_SERIAL.begin(115200); \
} while (0);


#else

#define LOG(...) do { \
} while (0);

#define LOGLN(...) do { \
} while (0);

#define LOGSETUP(...) do { \
} while (0);

#endif

#define LOGLN_ERROR(...) LOGLN(LOGLEVEL_ERROR, __VA_ARGS__)
#define LOGLN_WARNING(...) LOGLN(LOGLEVEL_WARNING, __VA_ARGS__)
#define LOGLN_INFO(...) LOGLN(LOGLEVEL_INFO, __VA_ARGS__)
#define LOGLN_DEBUG(...) LOGLN(LOGLEVEL_DEBUG, __VA_ARGS__)
#define LOGLN_VERBOSE(...) LOGLN(LOGLEVEL_VERBOSE, __VA_ARGS__)

#endif

