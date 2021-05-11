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

#ifndef __MYMCP3008_HPP__
#define __MYMCP3008_HPP__

#include <cstdint>

#define MYMCP3008_ANALOG_RESOLUTION 10
#define MYMCP3008_ANALOG_VALUECOUNT (1 << MYMCP3008_ANALOG_RESOLUTION)
#define MYMCP3008_ANALOG_VALUEMAX (MYMCP3008_ANALOG_VALUECOUNT - 1)

class MyMCP3008 {
  public:
    static void setup();

    void begin(uint8_t csPin);
    int readADC(uint8_t channel);

  private:
    uint8_t _cs;
};

extern MyMCP3008 myMCP3008s[];

#endif
