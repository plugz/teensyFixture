#ifndef __TRANSLATE_HPP__
#define __TRANSLATE_HPP__

#include <cstdint>

#define INPUTMCPSPI_COUNT 2
#define MYMCP3008_COUNT 1

#define BUTTON_COUNT 10
#define ENCODER_COUNT 4
#define FADER_COUNT 6

class Translate
{
public:
    static void setup();

    // pins usage
    static uint8_t inputMCPSPIIdxToCSPin(uint8_t inputMCPSPIIdx);
    static uint8_t MCP3008IdxToCSPin(uint8_t MCP3008Idx);

    // MCP23S17 usage
    static uint8_t buttonIdxToMCPIdx(uint8_t buttonIdx);
    static uint8_t buttonIdxToMCPPin(uint8_t buttonIdx);
    static uint8_t encoderIdxToPushMCPIdx(uint8_t encoderIdx);
    static uint8_t encoderIdxToPushMCPPin(uint8_t encoderIdx);
    static uint8_t encoderIdxToRot1MCPIdx(uint8_t encoderIdx);
    static uint8_t encoderIdxToRot1MCPPin(uint8_t encoderIdx);
    static uint8_t encoderIdxToRot2MCPIdx(uint8_t encoderIdx);
    static uint8_t encoderIdxToRot2MCPPin(uint8_t encoderIdx);

    // MCP3008 usage
    static uint8_t faderIdxToMCPIdx(uint8_t faderIdx);
    static uint8_t faderIdxToMCPPin(uint8_t faderIdx);
};

#endif
