#include "Fader.hpp"
#include "Log.hpp"
#include "Midi.hpp"
#include "MyMCP3008.hpp"
#include "Translate.hpp"
#include "Utils.hpp"

#include <core_pins.h>

Fader faders[FADER_COUNT];

void Fader::setup()
{
    unsigned int i = 0;
    for (auto& fader: faders)
    {
        uint8_t mcpIdx = Translate::faderIdxToMCPIdx(i);
        uint8_t pin;
        if (mcpIdx == uint8_t(0xff))
            pin = Translate::faderIdxToPin(i);
        else
            pin = Translate::faderIdxToMCPPin(i);

        fader.setupSingle(
                i,
                Translate::faderIdxToMidiIdx(i),
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

void Fader::setupSingle(uint16_t idx, uint16_t midiIdx, uint8_t mcpIdx, uint8_t pin)
{
    _idx = idx;
    _mcpIdx = mcpIdx;
    _pin = pin;
    _midiIdx = midiIdx;

    if (_mcpIdx == uint8_t(0xff))
    {
        pinMode(_pin, INPUT);
        _smoothener.setAnalogResolution(ANALOG_VALUECOUNT);
    }
    else
    {
        _smoothener.setAnalogResolution(MYMCP3008_ANALOG_VALUECOUNT);
    }
    _midiTimer.reset();
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
        int value = _smoothener.getValue();
        LOGLN_VERBOSE("fader changed: fader=%u, val=%u",
                unsigned(_idx), unsigned(value));
        uint8_t midiValue = Midi::analogToMidi(value);
        if (midiValue != _currentMidiValue)
        {
            LOGLN_VERBOSE("fader changed: fader=%u, val=%u, midiVal=%u",
                    unsigned(_idx), unsigned(value), unsigned(midiValue));
            _currentMidiValue = midiValue;
            _hasToSendMidiValue = true;
        }
    }

    if (_hasToSendMidiValue) {
        if (_midiTimer.advance()) {
            Midi::sendMessage(_midiIdx, _currentMidiValue);
            _midiTimer.reset();
            _hasToSendMidiValue = false;
        }
    }
}
