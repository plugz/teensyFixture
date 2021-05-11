#ifndef __INPUTMCPSPI_HPP__
#define __INPUTMCPSPI_HPP__

#include "MyMCP23S17.hpp"

#include <cstdint>

class InputMCPSPI : public MyMCP23S17
{
public:
    static void setup();
    static void loop();

    void setupSingle(
            uint8_t idx,
            uint8_t spiWire,
            uint8_t spiCsPin,
            uint8_t spiAddress);
    void loopSingle();
    void update();

    uint16_t getCurrentValue() const;
    bool getCurrentValue(uint8_t pin) const;

private:
    uint8_t _idx;
    uint16_t _currentValue = 0xFFFF;
};

extern InputMCPSPI inputMCPSPIs[];

#endif
