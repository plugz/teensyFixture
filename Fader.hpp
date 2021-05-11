#ifndef __FADER_HPP__
#define __FADER_HPP__

#include "ExternalResponsiveAnalogRead.hpp"
#include "Utils.hpp"

#include <cstdint>

class Fader
{
public:
    // idx, value
    using Callback = void (*)(uint8_t, uint16_t);

public:
    static void setup(Callback cb);
    static void loop();

    void setupSingle(uint8_t idx, uint8_t mcpIdx, uint8_t pin);
    void loopSingle();

private:
    static Callback _callback;
    ExternalResponsiveAnalogRead _smoothener;
    uint8_t _idx;
    uint8_t _mcpIdx;
    uint8_t _pin;
    uint16_t _currentValue = 0;
    bool _hasToSendValue = false;
    TimerMillis _timer = 10; // 100Hz fader update freq
};

extern Fader faders[];

#endif
