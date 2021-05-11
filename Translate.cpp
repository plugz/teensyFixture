#include "Translate.hpp"
#include "Button.hpp"
#include "Encoder.hpp"
#include "Fader.hpp"
#include "Led.hpp"
#include "Midi.hpp"
#include "Utils.hpp"

////////////////////////////////////////////////////////////////////////////////
// midi

// second midi channel
#define MIDI_CHANNEL_IDX 1

static FeedbackHandler* s_midiIdxToFeedbackHandler[MIDI_TOTAL_COUNT] = {nullptr,};

// 3 notes for 1 encoder: {push, rot1, rot2}
static uint16_t s_encoderIdxToMidiIdx[ENCODER_COUNT][3] = {
};

////////////////////////////////////////////////////////////////////////////////
// pins usage

// SPI0:
// MOSI: 7, 11, 28
// MISO: 8, 12, 39
// SCK: 13(led), 14, 27
// CS: 2, 6, 9, 10, 15, 20, 21, 22, 23, 26, 45
//
// SPI1:
// MOSI: 0, 21, 59, 61
// MISO: 1, 5, 59, 61
// SCK: 20, 32, 60
// CS: 6, 31, 58, 62, 63
//
// SPI2:
// MOSI: 44, 52
// MISO: 45, 51
// SCK: 46, 53
// CS: 43, 54, 55

// {MOSI, MISO, SCK}
static uint8_t s_spiWireIdxToPins[SPI_WIRES_COUNT][3] = {
    {11, 12, 13}, // SPI0
    {26, 1, 27}, // SPI1
    // {44, 45, 46}, // SPI2
};

static uint8_t s_inputMCPSPIIdxToCSPin[INPUTMCPSPI_COUNT] = {
    10, 10, 10, 10,
};

static uint8_t s_MCP3008IdxToCSPin[MYMCP3008_COUNT] = {
    2, 3, 4, 5, 6, 7, 8, 9,
};

static const uint8_t s_faderIdxToPin[FADER_COUNT] = {
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
    uint8_t(0xff), uint8_t(0xff), uint8_t(0xff), uint8_t(0xff),
};

////////////////////////////////////////////////////////////////////////////////
// spi wires usage

static uint8_t s_inputMCPSPIIdxToSPIWireIdx[INPUTMCPSPI_COUNT] = {
    0, 0, 0, 0,
};

static uint8_t s_MCP3008IdxToSPIWireIdx[MYMCP3008_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0,
};

////////////////////////////////////////////////////////////////////////////////
// MCP23S17 usage

static const uint8_t s_buttonIdxToMCPIdx[BUTTON_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
};

static const uint8_t s_buttonIdxToMCPPin[BUTTON_COUNT] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
};

// {PUSH, ROT1, ROT2}
static const uint8_t s_encoderIdxToMCPIdx[ENCODER_COUNT][3] = {
};

static const uint8_t s_encoderIdxToMCPPin[ENCODER_COUNT][3] = {
};

////////////////////////////////////////////////////////////////////////////////
// MCP3008 usage

static const uint8_t s_faderIdxToMCPIdx[FADER_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7,
};

static const uint8_t s_faderIdxToMCPPin[FADER_COUNT] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
};

void Translate::setup()
{
    // button column 0 -> led column 0
    //               1 ->            2
    //               2 ->            1
    //               3 ->            3
    // loop per column
    for (unsigned int i = 0; i < BUTTON_COUNT; i += 4)
    {
        auto buttonCol = i / 4;
        auto ledCol = buttonCol;
        if (buttonCol % 4 == 1) {
            ledCol = buttonCol + 1;
        }
        else if (buttonCol % 4 == 2) {
            ledCol = buttonCol - 1;
        }

        // loop per button
        for (unsigned int j = 0; j < 4; ++j) {
            s_midiIdxToFeedbackHandler[buttonIdxToMidiIdx(buttonCol * 4 + j)] =
                &leds[ledCol * 4 + j];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// midi

FeedbackHandler* Translate::midiIdxToFeedbackHandler(uint16_t midiIdx)
{
    if (midiIdx >= ARRAY_COUNT(s_midiIdxToFeedbackHandler))
        return nullptr;
    return s_midiIdxToFeedbackHandler[midiIdx];
}

uint16_t Translate::buttonIdxToMidiIdx(uint16_t buttonIdx)
{
    if (buttonIdx >= BUTTON_COUNT)
        return -1;
    return MIDI_CHANNEL_IDX * MIDI_NOTE_COUNT + buttonIdx;
}

uint16_t Translate::faderIdxToMidiIdx(uint16_t faderIdx)
{
    if (faderIdx >= FADER_COUNT)
        return -1;
    return MIDI_CHANNEL_IDX * MIDI_NOTE_COUNT + BUTTON_COUNT + faderIdx;
}

uint16_t Translate::encoderIdxToPushMidiIdx(uint16_t encoderIdx)
{
    if (encoderIdx >= ENCODER_COUNT)
        return -1;
    return s_encoderIdxToMidiIdx[encoderIdx][0];
}

uint16_t Translate::encoderIdxToRot1MidiIdx(uint16_t encoderIdx)
{
    if (encoderIdx >= ENCODER_COUNT)
        return -1;
    return s_encoderIdxToMidiIdx[encoderIdx][1];
}

uint16_t Translate::encoderIdxToRot2MidiIdx(uint16_t encoderIdx)
{
    if (encoderIdx >= ENCODER_COUNT)
        return -1;
    return s_encoderIdxToMidiIdx[encoderIdx][2];
}

////////////////////////////////////////////////////////////////////////////////
// pins usage

uint8_t Translate::spiWireIdxToMOSIPin(uint16_t spiWireIdx)
{
    return s_spiWireIdxToPins[spiWireIdx][0];
}

uint8_t Translate::spiWireIdxToMISOPin(uint16_t spiWireIdx)
{
    return s_spiWireIdxToPins[spiWireIdx][1];
}

uint8_t Translate::spiWireIdxToSCKPin(uint16_t spiWireIdx)
{
    return s_spiWireIdxToPins[spiWireIdx][2];
}

uint8_t Translate::inputMCPSPIIdxToCSPin(uint16_t inputMCPSPIIdx)
{
    return s_inputMCPSPIIdxToCSPin[inputMCPSPIIdx];
}

uint8_t Translate::MCP3008IdxToCSPin(uint16_t MCP3008Idx)
{
    return s_MCP3008IdxToCSPin[MCP3008Idx];
}

uint8_t Translate::faderIdxToPin(uint16_t faderIdx)
{
    return s_faderIdxToPin[faderIdx];
}

////////////////////////////////////////////////////////////////////////////////
// spi wires usage

uint8_t Translate::inputMCPSPIIdxToSPIWireIdx(uint16_t inputMCPSPIIdx)
{
    return s_inputMCPSPIIdxToSPIWireIdx[inputMCPSPIIdx];
}

uint8_t Translate::MCP3008IdxToSPIWireIdx(uint16_t MCP3008Idx)
{
    return s_MCP3008IdxToSPIWireIdx[MCP3008Idx];
}

////////////////////////////////////////////////////////////////////////////////
// MCP23S17 usage

uint8_t Translate::buttonIdxToMCPIdx(uint16_t buttonIdx)
{
    return s_buttonIdxToMCPIdx[buttonIdx];
}

uint8_t Translate::buttonIdxToMCPPin(uint16_t buttonIdx)
{
    return s_buttonIdxToMCPPin[buttonIdx];
}

uint8_t Translate::encoderIdxToPushMCPIdx(uint16_t encoderIdx)
{
    return s_encoderIdxToMCPIdx[encoderIdx][0];
}

uint8_t Translate::encoderIdxToPushMCPPin(uint16_t encoderIdx)
{
    return s_encoderIdxToMCPPin[encoderIdx][0];
}

uint8_t Translate::encoderIdxToRot1MCPIdx(uint16_t encoderIdx)
{
    return s_encoderIdxToMCPIdx[encoderIdx][1];
}

uint8_t Translate::encoderIdxToRot1MCPPin(uint16_t encoderIdx)
{
    return s_encoderIdxToMCPPin[encoderIdx][1];
}

uint8_t Translate::encoderIdxToRot2MCPIdx(uint16_t encoderIdx)
{
    return s_encoderIdxToMCPIdx[encoderIdx][2];
}

uint8_t Translate::encoderIdxToRot2MCPPin(uint16_t encoderIdx)
{
    return s_encoderIdxToMCPPin[encoderIdx][2];
}

////////////////////////////////////////////////////////////////////////////////
// MCP3008 usage

uint8_t Translate::faderIdxToMCPIdx(uint16_t faderIdx)
{
    return s_faderIdxToMCPIdx[faderIdx];
}

uint8_t Translate::faderIdxToMCPPin(uint16_t faderIdx)
{
    return s_faderIdxToMCPPin[faderIdx];
}
