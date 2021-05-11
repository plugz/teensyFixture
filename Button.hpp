#ifndef __BUTTON_HPP__
#define __BUTTON_HPP__

#include "ExternalBounce.hpp"

#include <cstdint>

class Button
{
public:
    static void setup();
    static void loop();

    void setupSingle(
            uint16_t idx,
            uint16_t midiIdx,
            uint8_t mcp,
            uint8_t pin);
    void loopSingle();

    uint16_t midiIdx() const
    {
        return _midiIdx;
    }

private:
    ExternalBounce _debouncer;
    uint16_t _idx;
    uint16_t _midiIdx;
    uint8_t _mcp;
    uint8_t _pin;
};

extern Button buttons[];

#endif
