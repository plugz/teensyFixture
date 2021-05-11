#ifndef __FADER_HPP__
#define __FADER_HPP__

#include "ExternalResponsiveAnalogRead.hpp"
#include "Utils.hpp"

#include <cstdint>

class Fader
{
public:
    static void setup();
    static void loop();

    void setupSingle(uint16_t idx, uint16_t midiIdx, uint8_t mcpIdx, uint8_t pin);
    void loopSingle();

private:
    ExternalResponsiveAnalogRead _smoothener;
    uint16_t _idx;
    uint16_t _midiIdx;
    uint8_t _mcpIdx;
    uint8_t _pin;
    uint8_t _currentMidiValue = 0;
    bool _hasToSendMidiValue = false;
    TimerMillis _midiTimer = 10; // 100Hz midi update freq
};

extern Fader faders[];

#endif
