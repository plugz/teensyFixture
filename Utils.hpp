#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <Arduino.h>

#define ARRAY_COUNT(array) (sizeof(array)/sizeof(*array))

// Same as MCP3008
#define ANALOG_RESOLUTION 10
#define ANALOG_VALUECOUNT (1 << ANALOG_RESOLUTION)
#define ANALOG_VALUEMAX (ANALOG_VALUECOUNT - 1)

using Micros = decltype(micros());
using Millis = decltype(millis());

template<class TTimeType, class TImpl>
class Timer {
public:
    Timer(TTimeType period) {
        _prevTime = TImpl::now();
        _period = period;
    }
    void reset() {
        _prevTime = TImpl::now();
    }
    bool advance() {
        TTimeType now = TImpl::now();
        TTimeType nextTime = _prevTime + _period;
        if (now < _prevTime) {
            // "now" has overflowed
            if (nextTime > _prevTime) {
                // "nextTime" has not overflowed
                _prevTime = nextTime;
                return true;
            }
            if (now >= nextTime) {
                _prevTime = nextTime;
                return true;
            }
            return false;
        }
        if (nextTime < _prevTime) {
            // "nextTime" has overflowed
            return false;
        }
        if (now >= nextTime) {
            _prevTime = nextTime;
            return true;
        }
        return false;
    }

private:
    TTimeType _prevTime;
    TTimeType _period;
};

class TimerMicros : public Timer<Micros, TimerMicros> {
public:
    using Timer<Micros, TimerMicros>::Timer;
    static Micros now() {
        return micros();
    }
};

class TimerMillis : public Timer<Millis, TimerMillis> {
public:
    using Timer<Millis, TimerMillis>::Timer;
    static Millis now() {
        return millis();
    }
};

#endif
