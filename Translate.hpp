#ifndef __TRANSLATE_HPP__
#define __TRANSLATE_HPP__

#include <cstdint>

#define SPI_WIRES_COUNT 2

#define INPUTMCPSPI_COUNT 4
#define MYMCP3008_COUNT 8

#define BUTTON_COUNT 64
#define ENCODER_COUNT 0
#define FADER_COUNT 64
#define LED_COUNT 64

class FeedbackHandler;

class Translate
{
public:
    static void setup();

    // midi
    static FeedbackHandler* midiIdxToFeedbackHandler(uint16_t midiIdx);
    static uint16_t buttonIdxToMidiIdx(uint16_t buttonIdx);
    static uint16_t faderIdxToMidiIdx(uint16_t faderIdx);
    static uint16_t encoderIdxToPushMidiIdx(uint16_t encoderIdx);
    static uint16_t encoderIdxToRot1MidiIdx(uint16_t encoderIdx);
    static uint16_t encoderIdxToRot2MidiIdx(uint16_t encoderIdx);

    // pins usage
    static uint8_t spiWireIdxToMOSIPin(uint16_t spiWireIdx);
    static uint8_t spiWireIdxToMISOPin(uint16_t spiWireIdx);
    static uint8_t spiWireIdxToSCKPin(uint16_t spiWireIdx);
    static uint8_t inputMCPSPIIdxToCSPin(uint16_t inputMCPSPIIdx);
    static uint8_t MCP3008IdxToCSPin(uint16_t MCP3008Idx);
    static uint8_t faderIdxToPin(uint16_t faderIdx);

    // spi wires usage
    static uint8_t inputMCPSPIIdxToSPIWireIdx(uint16_t inputMCPSPIIdx);
    static uint8_t MCP3008IdxToSPIWireIdx(uint16_t MCP3008Idx);

    // MCP23S17 usage
    static uint8_t buttonIdxToMCPIdx(uint16_t buttonIdx);
    static uint8_t buttonIdxToMCPPin(uint16_t buttonIdx);
    static uint8_t encoderIdxToPushMCPIdx(uint16_t encoderIdx);
    static uint8_t encoderIdxToPushMCPPin(uint16_t encoderIdx);
    static uint8_t encoderIdxToRot1MCPIdx(uint16_t encoderIdx);
    static uint8_t encoderIdxToRot1MCPPin(uint16_t encoderIdx);
    static uint8_t encoderIdxToRot2MCPIdx(uint16_t encoderIdx);
    static uint8_t encoderIdxToRot2MCPPin(uint16_t encoderIdx);

    // MCP3008 usage
    static uint8_t faderIdxToMCPIdx(uint16_t faderIdx);
    static uint8_t faderIdxToMCPPin(uint16_t faderIdx);
};

#endif
