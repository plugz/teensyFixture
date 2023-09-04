#include "Fader.hpp"
#include "Log.hpp"
#include "MyMCP3008.hpp"
#include "Translate.hpp"
#include "Utils.hpp"

#include <core_pins.h>

Fader faders[FADER_COUNT];
Fader::Callback Fader::_callback = nullptr;

void Fader::setup(Callback cb)
{
    _callback = cb;
    unsigned int i = 0;
    for (auto& fader: faders)
    {
        uint8_t mcpIdx = Translate::faderIdxToMCPIdx(i);
        uint8_t pin;
        pin = Translate::faderIdxToMCPPin(i);

        fader.setupSingle(
                i,
                mcpIdx,
                pin);
        ++i;
    }
}

void Fader::loop()
{
    for (auto& fader: faders)
    {
        fader.loopSingle();
    }
}

void Fader::setupSingle(uint8_t idx, uint8_t mcpIdx, uint8_t pin)
{
    _idx = idx;
    _mcpIdx = mcpIdx;
    _pin = pin;

    _smoothener.setAnalogResolution(MYMCP3008_ANALOG_VALUECOUNT);
    _timer.reset();
}

void Fader::loopSingle()
{
    int readValue;
    if (_mcpIdx == uint8_t(0xff))
        readValue = analogRead(_pin);
    else
        readValue = myMCP3008s[_mcpIdx].readADC(_pin);

    if (_smoothener.update(readValue))
    {
        LOGLN_VERBOSE("fader changed: fader=%u, val=%u",
                unsigned(_idx), unsigned(_smoothener.getValue()));
        _currentValue = _smoothener.getValue();
        _hasToSendValue = true;
    }

    if (_hasToSendValue) {
        if (_timer.advance()) {
            LOGLN_DEBUG("fader send update: fader=%u, val=%u",
                    unsigned(_idx), unsigned(_currentValue));
            _callback(_idx, _currentValue);
            _timer.reset();
            _hasToSendValue = false;
        }
    }
}
