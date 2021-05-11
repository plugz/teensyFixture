/***************************************************
  This is an Arduino Library for the MCP3008 - 8-Channel 10-Bit ADC
  With SPI Interface.
  ----> http://www.adafruit.com/products/856
  
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Copyright (c) 2017, Adafruit Industries (adafruit.com)
  MIT License (https://opensource.org/licenses/MIT)
****************************************************/

#include "MyMCP3008.hpp"
#include "MySPI.hpp"
#include "Translate.hpp"

#include <Arduino.h>

#define MCP3008_SPI_MAX_5V  3600000
#define MCP3008_SPI_MAX_3V  1350000
#define MCP3008_SPI_MAX     MCP3008_SPI_MAX_3V
#define MCP3008_SPI_ORDER   MSBFIRST
#define MCP3008_SPI_MODE    SPI_MODE0

MyMCP3008 myMCP3008s[MYMCP3008_COUNT];

void MyMCP3008::setup()
{
    uint8_t idx = 0;
    for (auto& myMCP3008: myMCP3008s)
    {
        myMCP3008.begin(
                Translate::MCP3008IdxToSPIWireIdx(idx),
                Translate::MCP3008IdxToCSPin(idx));
        ++idx;
    }
}

// Initialize for hardware SPI
void MyMCP3008::begin(uint8_t spiWire, uint8_t csPin) {
  _spiWire = spiWire;
  _cs = csPin;

  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
}

// Read single ended ADC channel.
int MyMCP3008::readADC(uint8_t channel) {
  if ((channel < 0) || (channel > 7)) return -1;
  
  uint8_t command0 = 1; // start bit
  uint16_t command1 =
      (1 << 15) // single mode
      | ((channel & 0x7) << 12); // channel
  
    spi_wires[_spiWire]->beginTransaction(SPISettings(MCP3008_SPI_MAX, MCP3008_SPI_ORDER, MCP3008_SPI_MODE));
    digitalWrite(_cs, LOW);
  
    spi_wires[_spiWire]->transfer(command0);
    uint16_t raw = spi_wires[_spiWire]->transfer16(command1);
  
    digitalWrite(_cs, HIGH);
    spi_wires[_spiWire]->endTransaction();
  
    return raw & 0x3FF;
}
