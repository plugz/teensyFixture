#include "Encoder.hpp"
#include "InputMCPSPI.hpp"
#include "Log.hpp"
#include "Translate.hpp"
#include "Utils.hpp"

Encoder encoders[ENCODER_COUNT];
Encoder::Callback Encoder::_callback = nullptr;

void Encoder::setup(Callback cb)
{
    _callback = cb;
    unsigned int i = 0;
    for (auto& encoder: encoders)
    {
        encoder.setupSingle(
                i,
                Translate::encoderIdxToRot1MCPIdx(i),
                Translate::encoderIdxToRot2MCPIdx(i),
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
        uint8_t rot1Mcp,
        uint8_t rot2Mcp,
        uint8_t rot1Pin,
        uint8_t rot2Pin)
{
    _idx = idx;
    _rot1Mcp = rot1Mcp;
    _rot2Mcp = rot2Mcp;
    _rot1Pin = rot1Pin;
    _rot2Pin = rot2Pin;

    // Set current encoder position
    _externalRotary.update(
            inputMCPSPIs[_rot1Mcp].getCurrentValue(_rot1Pin),
            inputMCPSPIs[_rot2Mcp].getCurrentValue(_rot2Pin));
}

void Encoder::loopSingle()
{
    // update() returns true if the state changed
    if (_externalRotary.update(
            inputMCPSPIs[_rot1Mcp].getCurrentValue(_rot1Pin),
            inputMCPSPIs[_rot2Mcp].getCurrentValue(_rot2Pin)))
    {
        if (_externalRotary.read() == DIR_CCW)
        {
            _callback(_idx, false);
            --_rotaryPosition;
        }
        else // (_externalRotary.read() == DIR_CW)
        {
            _callback(_idx, true);
            ++_rotaryPosition;
        }
        LOGLN_DEBUG("encoder position changed: enc=%u, pos=%i, dir=%s",
                unsigned(_idx), int(_rotaryPosition),
                (_externalRotary.read() == DIR_CW ? "CW" : "CCW"));
    }
}
