#include "Button.hpp"
#include "InputMCPSPI.hpp"
#include "Log.hpp"
#include "Translate.hpp"
#include "Utils.hpp"

#include <core_pins.h>

Button buttons[BUTTON_COUNT];
Button::Callback Button::_callback = nullptr;

void Button::setup(Callback cb)
{
    _callback = cb;
    unsigned int i = 0;
    for (auto& button: buttons)
    {
        button.setupSingle(
                i,
                Translate::buttonIdxToMCPIdx(i),
                Translate::buttonIdxToMCPPin(i));
        ++i;
    }
}

void Button::loop()
{
    for (auto& button: buttons)
    {
        button.loopSingle();
    }
}

void Button::setupSingle(
        uint8_t idx,
        uint8_t mcp,
        uint8_t pin)
{
    _idx = idx;
    _mcp = mcp;
    _pin = pin;

    // interval in ms
    _debouncer.setInterval(5);
}

void Button::loopSingle()
{
    // update() returns true if the state changed
    if (_debouncer.update(inputMCPSPIs[_mcp].getCurrentValue(_pin)))
    {
        // pullup -> 1 means button is not pushed -> invert
        bool value = !_debouncer.read();
        LOGLN_VERBOSE("button changed: btn=%u, val=%u",
                unsigned(_idx), unsigned(value));
        _callback(_idx, value);
    }
}
