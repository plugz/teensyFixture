/*
 * Rotary encoder library for Arduino.
 */

#ifndef __EXTERNALROTARY_HPP__
#define __EXTERNALROTARY_HPP__

#include <stdint.h>

// Enable this to emit codes twice per step.
//#define HALF_STEP

// Values returned by 'process'
// No complete step yet.
#define DIR_NONE 0x0
// Clockwise step.
#define DIR_CW 0x10
// Anti-clockwise step.
#define DIR_CCW 0x20

class ExternalRotary
{
  public:
    ExternalRotary();
    // Process pin(s)
    uint8_t update(uint8_t val1, uint8_t val2);
    uint8_t read() const;
  private:
    uint8_t _state;
};

#endif
 
