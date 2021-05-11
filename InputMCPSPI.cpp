#include "InputMCPSPI.hpp"
#include "Translate.hpp"

#include <core_pins.h>

// TODO move this to Translate.[ch]pp
static uint8_t INPUTMCPSPI_ADDRESSES[INPUTMCPSPI_COUNT] = {
    0
};

InputMCPSPI inputMCPSPIs[INPUTMCPSPI_COUNT];

void InputMCPSPI::setup()
{
    uint8_t idx = 0;
    for (auto& inputMCPSPI: inputMCPSPIs)
    {
        inputMCPSPI.setupSingle(
                idx,
                Translate::inputMCPSPIIdxToCSPin(idx),
                INPUTMCPSPI_ADDRESSES[idx]);
        ++idx;
    }
}

void InputMCPSPI::loop()
{
    for (auto& inputMCPSPI: inputMCPSPIs)
    {
        inputMCPSPI.loopSingle();
    }
}

void InputMCPSPI::setupSingle(
        uint8_t idx,
        uint8_t spiCsPin,
        uint8_t spiAddress)
{
    _idx = idx;

    MyMCP23S17::setup(spiCsPin, spiAddress);
    begin();
    portPullup(0xFFFF);

    // let pullups do their jobs
    delay(1);
    // get current values
    update();
}

void InputMCPSPI::loopSingle()
{
    update();
}

void InputMCPSPI::update()
{
    _currentValue = readGpioPort();

    // Invert, because A and B are reversed compared to MCP23017
    _currentValue =
        ((_currentValue >> 8) & 0x00FF)
        | ((_currentValue << 8) & 0xFF00);
}

uint16_t InputMCPSPI::getCurrentValue() const
{
    return _currentValue;
}

bool InputMCPSPI::getCurrentValue(uint8_t pin) const
{
    return (_currentValue >> pin) & 1;
}
