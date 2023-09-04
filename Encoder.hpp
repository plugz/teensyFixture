#ifndef __ENCODER_HPP__
#define __ENCODER_HPP__

#include "ExternalBounce.hpp"
#include "ExternalRotary.hpp"

#include <cstdint>

class Encoder
{
public:
    static void setup();
    static void loop();

    void setupSingle(
            uint16_t idx,
            uint16_t pushMidiIdx,
            uint16_t rot1MidiIdx,
            uint16_t rot2MidiIdx,
            uint8_t pushMcp,
            uint8_t rot1Mcp,
            uint8_t rot2Mcp,
            uint8_t pushPin,
            uint8_t rot1Pin,
            uint8_t rot2Pin);
    void loopSingle();

private:
    ExternalRotary _externalRotary;
    int32_t _rotaryPosition = 0;
    ExternalBounce _debouncer;
    uint16_t _idx;
    uint16_t _pushMidiIdx;
    uint16_t _rot1MidiIdx;
    uint16_t _rot2MidiIdx;
    uint8_t _pushMcp;
    uint8_t _rot1Mcp;
    uint8_t _rot2Mcp;
    uint8_t _pushPin;
    uint8_t _rot1Pin;
    uint8_t _rot2Pin;
};

extern Encoder encoders[];

#endif
