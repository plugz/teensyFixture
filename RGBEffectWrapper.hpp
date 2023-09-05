#ifndef __RGBEFFECTWRAPPER_HPP__
#define __RGBEFFECTWRAPPER_HPP__

#include "RGBEffect.hpp"
#include "Utils.hpp"

#include <cstdint>

class RGBEffectWrapper
{
public:
    static const RGBEffect::PosArray posArray;

public:
    void begin(uint8_t* pixels, int pixelCount);
    void startFlash
    void startFlash();
    void stopFlash();
    void prevMode();
    void nextMode();
    void prevColor();
    void nextColor();
    void changeSpeed(Float multiplier);
    bool refreshPixels(unsigned long currentMillis);

private:
    void updateColor();
    void begin();
    uint8_t* _pixels;
    unsigned int _pixelCount;
    int _currentEffect0Idx = 0;
    int _currentEffect1Idx = 0;
    int _currentColor0Idx = 0;
    int _currentColor1Idx = 0;
    RGBEffect _currentEffect0;
    RGBEffect _currentEffect1;
    RGBEffect _smoothOff;
    RGBEffect _fullOff;
    RGBEffect _smoothStrobe;
    RGBEffect _vnrStrobe;

    Float _speed = Float::scaleUp(1);
};

#endif
