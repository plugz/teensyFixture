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
    void startFlash();
    void stopFlash();
    void nextMode();
    void nextColor();
    bool refreshPixels(unsigned long currentMillis);

private:
    void begin();
    uint8_t* _pixels;
    unsigned int _pixelCount;
    int _currentEffectsIdx = 0;
    int _currentColorsIdx = 0;
    StaticVector<RGBEffect> _currentEffects;
    StaticVector<RGBEffect> _currentStrobeEffects;
    bool _flashing = false;
};

#endif
