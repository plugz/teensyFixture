#ifndef __ENCODER_HPP__
#define __ENCODER_HPP__

#include "ExternalBounce.hpp"
#include "ExternalRotary.hpp"

#include <cstdint>

class Encoder
{
public:
    // idx, status (false for CCW, true for CW)
    using Callback = void (*)(uint8_t, bool);

public:
    static void setup(Callback cb);
    static void loop();

    void setupSingle(
            uint16_t idx,
            uint8_t rot1Mcp,
            uint8_t rot2Mcp,
            uint8_t rot1Pin,
            uint8_t rot2Pin);
    void loopSingle();

private:
    static Callback _callback;
    ExternalRotary _externalRotary;
    ExternalBounce _rot1Debouncer;
    ExternalBounce _rot2Debouncer;
    int32_t _rotaryPosition = 0;
    uint16_t _idx;
    uint8_t _rot1Mcp;
    uint8_t _rot2Mcp;
    uint8_t _rot1Pin;
    uint8_t _rot2Pin;
};

extern Encoder encoders[];

#endif
