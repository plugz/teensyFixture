#ifndef __RGBEFFECTWRAPPER_HPP__
#define __RGBEFFECTWRAPPER_HPP__

#include "RGBEffect.hpp"
#include "Utils.hpp"

#include <cstdint>

class RGBEffectWrapper
{
public:
    static RGBEffect::PosArray posArray;

    static RGBEffect::PosArray posArrayTower(StaticVector<int>& targetBuffer,
            bool hEn, Float hSize, Float hPos,
            bool vEn, Float vSize, Float vPos);

public:
    void begin(uint8_t* pixels, int pixelCount);
    void flashSmoothOff(bool enable);
    void flashFullOff(bool enable);
    void flashSmoothStrobe(bool enable);
    void flashVnrStrobe(bool enable);
    void pat0PrevMode();
    void pat0NextMode();
    void pat1PrevMode();
    void pat1NextMode();
    void pat0PrevColor();
    void pat0NextColor();
    void pat1PrevColor();
    void pat1NextColor();
    void pat0ChangeSpeed(Float multiplier);
    void pat1ChangeSpeed(Float multiplier);
    void pat0ChangeDim(Float dim);
    void pat1ChangeDim(Float dim);
    void pat0Enable(bool enable);
    void pat1Enable(bool enable);

    void hPartEnable(bool enable);
    void vPartEnable(bool enable);
    void hPartSize(Float size);
    void vPartSize(Float size);
    void hPartPos(Float pos);
    void vPartPos(Float pos);

    bool refreshPixels(unsigned long currentMillis);

private:
    void updateColor();
    void updatePosArray();
    void begin();
    void pat0Begin();
    void pat1Begin();
    uint8_t* _pixels;
    unsigned int _pixelCount;
    int _pat0EffectIdx = 0;
    int _pat1EffectIdx = 0;
    int _pat0ColorIdx = 0;
    int _pat1ColorIdx = 0;
    Float _pat0Speed = Float::scaleUp(1);
    Float _pat1Speed = Float::scaleUp(1);
    Float _pat0Dim = Float::scaleUp(1);
    Float _pat1Dim = Float::scaleUp(1);
    bool _pat0Enable = true;
    bool _pat1Enable = true;
    RGBEffect _pat0Effect;
    RGBEffect _pat1Effect;
    RGBEffect _smoothOffEffect;
    RGBEffect _fullOffEffect;
    RGBEffect _smoothStrobeEffect;
    RGBEffect _vnrStrobeEffect;
    bool _smoothOffFlashing = false;
    bool _fullOffFlashing = false;
    bool _smoothStrobeFlashing = false;
    bool _vnrStrobeFlashing = false;

    bool _hPartEnable = true;
    bool _vPartEnable = true;
    Float _hPartSize = Float::scaleUp(1);
    Float _hPartPos = Float::scaleUp(1);
    Float _vPartSize = Float::scaleUp(0.5f);
    Float _vPartPos = Float::scaleUp(0.5f);

    Float _speed = Float::scaleUp(1);
};

#endif
