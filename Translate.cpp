#include "Translate.hpp"
#include "Button.hpp"
#include "Fader.hpp"
#include "Utils.hpp"

static uint8_t s_inputMCPSPIIdxToCSPin[INPUTMCPSPI_COUNT] = {
    9, 9
};

static uint8_t s_MCP3008IdxToCSPin[MYMCP3008_COUNT] = {
    10
};

////////////////////////////////////////////////////////////////////////////////
// MCP23S17 usage

static const uint8_t s_buttonIdxToMCPIdx[BUTTON_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1,
};

static const uint8_t s_buttonIdxToMCPPin[BUTTON_COUNT] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 13,
    0, 3,
};

// {ROT1, ROT2}
static const uint8_t s_encoderIdxToMCPIdx[ENCODER_COUNT][2] = {
    {0, 0},
    {0, 0},
    {1, 1},
    {1, 1},
};

// {ROT1, ROT2}
// OOPS encoders are soldered reversed
static const uint8_t s_encoderIdxToMCPPin[ENCODER_COUNT][2] = {
    {12, 11},
    {15, 14},
    {2, 1},
    {5, 4},
};

////////////////////////////////////////////////////////////////////////////////
// MCP3008 usage

static const uint8_t s_faderIdxToMCPIdx[FADER_COUNT] = {
    0, 0, 0, 0, 0, 0,
};

static const uint8_t s_faderIdxToMCPPin[FADER_COUNT] = {
    0, 1, 2, 3, 4, 5,
};

void Translate::setup()
{
}

////////////////////////////////////////////////////////////////////////////////
// pins usage

uint8_t Translate::inputMCPSPIIdxToCSPin(uint8_t inputMCPSPIIdx)
{
    return s_inputMCPSPIIdxToCSPin[inputMCPSPIIdx];
}

uint8_t Translate::MCP3008IdxToCSPin(uint8_t MCP3008Idx)
{
    return s_MCP3008IdxToCSPin[MCP3008Idx];
}

////////////////////////////////////////////////////////////////////////////////
// MCP23S17 usage

uint8_t Translate::buttonIdxToMCPIdx(uint8_t buttonIdx)
{
    return s_buttonIdxToMCPIdx[buttonIdx];
}

uint8_t Translate::buttonIdxToMCPPin(uint8_t buttonIdx)
{
    return s_buttonIdxToMCPPin[buttonIdx];
}

uint8_t Translate::encoderIdxToRot1MCPIdx(uint8_t encoderIdx)
{
    return s_encoderIdxToMCPIdx[encoderIdx][0];
}

uint8_t Translate::encoderIdxToRot1MCPPin(uint8_t encoderIdx)
{
    return s_encoderIdxToMCPPin[encoderIdx][0];
}

uint8_t Translate::encoderIdxToRot2MCPIdx(uint8_t encoderIdx)
{
    return s_encoderIdxToMCPIdx[encoderIdx][1];
}

uint8_t Translate::encoderIdxToRot2MCPPin(uint8_t encoderIdx)
{
    return s_encoderIdxToMCPPin[encoderIdx][1];
}

////////////////////////////////////////////////////////////////////////////////
// MCP3008 usage

uint8_t Translate::faderIdxToMCPIdx(uint8_t faderIdx)
{
    return s_faderIdxToMCPIdx[faderIdx];
}

uint8_t Translate::faderIdxToMCPPin(uint8_t faderIdx)
{
    return s_faderIdxToMCPPin[faderIdx];
}
