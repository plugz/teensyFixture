#include "Encoder.hpp"
#include "InputMCPI2C.hpp"
#include "Log.hpp"
#include "Midi.hpp"
#include "Translate.hpp"
#include "Utils.hpp"

Encoder encoders[ENCODER_COUNT];

void Encoder::setup()
{
    unsigned int i = 0;
    for (auto& encoder: encoders)
    {
        encoder.setupSingle(
                i,
                Translate::encoderIdxToPushMidiIdx(i),
                Translate::encoderIdxToRot1MidiIdx(i),
                Translate::encoderIdxToRot2MidiIdx(i),
                Translate::encoderIdxToPushMCPIdx(i),
                Translate::encoderIdxToRot1MCPIdx(i),
                Translate::encoderIdxToRot2MCPIdx(i),
                Translate::encoderIdxToPushMCPPin(i),
                Translate::encoderIdxToRot1MCPPin(i),
                Translate::encoderIdxToRot2MCPPin(i));
        ++i;
    }
}

void Encoder::loop()
{
    for (auto& encoder: encoders)
    {
        encoder.loopSingle();
    }
}

void Encoder::setupSingle(
        uint16_t idx,
        uint16_t pushMidiIdx,
        uint16_t rot1MidiIdx,
        uint16_t rot2MidiIdx,
        uint8_t pushMcp,
        uint8_t rot1Mcp,
        uint8_t rot2Mcp,
        uint8_t pushPin,
        uint8_t rot1Pin,
        uint8_t rot2Pin)
{
    _idx = idx;
    _pushMidiIdx = pushMidiIdx;
    _rot1MidiIdx = rot1MidiIdx;
    _rot2MidiIdx = rot2MidiIdx;
    _pushMcp = pushMcp;
    _rot1Mcp = rot1Mcp;
    _rot2Mcp = rot2Mcp;
    _pushPin = pushPin;
    _rot1Pin = rot1Pin;
    _rot2Pin = rot2Pin;

    _debouncer.setInterval(3);
    // Set current encoder position
    _externalRotary.update(
            inputMCPI2Cs[_rot1Mcp].getCurrentValue(_rot1Pin),
            inputMCPI2Cs[_rot2Mcp].getCurrentValue(_rot2Pin));
}

void Encoder::loopSingle()
{
    // update() returns true if the state changed
    if (_debouncer.update(inputMCPI2Cs[_pushMcp].getCurrentValue(_pushPin)))
    {
        // pullup -> 1 means button is not pushed -> invert
        bool value = !_debouncer.read();
        LOGLN_DEBUG("encoder button changed: enc=%u, val=%u",
                unsigned(_idx), unsigned(value));
        Midi::sendMessage(_pushMidiIdx, value ? 127 : 0);
    }

    if (_externalRotary.update(
                inputMCPI2Cs[_rot1Mcp].getCurrentValue(_rot1Pin),
                inputMCPI2Cs[_rot2Mcp].getCurrentValue(_rot2Pin)) != DIR_NONE)
    {
        if (_externalRotary.read() == DIR_CCW)
        {
            // backward-> rot1 midi button press/release
            Midi::sendMessage(_rot1MidiIdx, 127);
            Midi::sendMessage(_rot1MidiIdx, 0);
            --_rotaryPosition;
        }
        else // (_externalRotary.read() == DIR_CW)
        {
            // forward-> rot2 midi button press/release
            Midi::sendMessage(_rot2MidiIdx, 127);
            Midi::sendMessage(_rot2MidiIdx, 0);
            ++_rotaryPosition;
        }
        LOGLN_DEBUG("encoder position changed: enc=%u, pos=%i, dir=%s",
                unsigned(_idx), int(_rotaryPosition),
                (_externalRotary.read() == DIR_CW ? "CW" : "CCW"));
    }
}
