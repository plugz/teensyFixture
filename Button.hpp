#ifndef __BUTTON_HPP__
#define __BUTTON_HPP__

#include "ExternalBounce.hpp"

#include <cstdint>

class Button
{
public:
    // idx, status
    using Callback = void (*)(uint8_t, bool);

public:
    static void setup(Callback cb);
    static void loop();

    void setupSingle(
            uint8_t idx,
            uint8_t mcp,
            uint8_t pin);
    void loopSingle();

private:
    static Callback _callback;
    ExternalBounce _debouncer;
    uint8_t _idx;
    uint8_t _mcp;
    uint8_t _pin;
};

extern Button buttons[];

#endif
