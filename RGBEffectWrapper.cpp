#include "RGBEffectWrapper.hpp"

#include "Log.hpp"

int posArrayBufferArray[NUM_LEDS] = {0};
StaticVector<int> posArrayBuffer{posArrayBufferArray, ARRAY_COUNT(posArrayBufferArray)};

RGBEffect::PosArray RGBEffectWrapper::posArray =
    RGBEffectWrapper::posArrayTower(posArrayBuffer,
            true, Float::scaleUp(1), Float::scaleUp(0.5f),
            true, Float::scaleUp(1), Float::scaleUp(0.5f));;

using EffectDescVector = StaticVector2<RGBEffect::Desc, 64>;

// clang-format off
EffectDescVector const sEffects{
    {{RGBEffectPattern::PLASMA}, 4000, RGBEffectMixingMode::REPLACE},
    {{RGBEffectPattern::STRIPE}, 2400, RGBEffectMixingMode::REPLACE},
    {{RGBEffectPattern::STRIPE}, 2430, RGBEffectMixingMode::SUB},
    {{RGBEffectPattern::STRIPE}, 2460, RGBEffectMixingMode::DIFFERENCE},
    {{RGBEffectPattern::STRIPE, RGBEffectAxis::VERTICAL}, 2500, RGBEffectMixingMode::REPLACE},
    {{RGBEffectPattern::STRIPE, RGBEffectAxis::VERTICAL}, 2530, RGBEffectMixingMode::SUB},
    {{RGBEffectPattern::STRIPE, RGBEffectAxis::VERTICAL}, 2560, RGBEffectMixingMode::DIFFERENCE},
    {{RGBEffectPattern::STRIPE_SMOOTH}, 2600, RGBEffectMixingMode::REPLACE},
    {{RGBEffectPattern::STRIPE_SMOOTH}, 2630, RGBEffectMixingMode::SUB},
    {{RGBEffectPattern::STRIPE_SMOOTH}, 2660, RGBEffectMixingMode::DIFFERENCE},
    {{RGBEffectPattern::STRIPE_SMOOTH, RGBEffectAxis::VERTICAL}, 2700, RGBEffectMixingMode::REPLACE},
    {{RGBEffectPattern::STRIPE_SMOOTH, RGBEffectAxis::VERTICAL}, 2730, RGBEffectMixingMode::SUB},
    {{RGBEffectPattern::STRIPE_SMOOTH, RGBEffectAxis::VERTICAL}, 2760, RGBEffectMixingMode::DIFFERENCE},
    {{RGBEffectPattern::ROTATION}, 2800, RGBEffectMixingMode::REPLACE},
    {{RGBEffectPattern::ROTATION}, 2830, RGBEffectMixingMode::SUB},
    {{RGBEffectPattern::ROTATION}, 2860, RGBEffectMixingMode::DIFFERENCE},
    {{RGBEffectPattern::ROTATION_THIN}, 2900, RGBEffectMixingMode::REPLACE},
    {{RGBEffectPattern::ROTATION_THIN}, 2930, RGBEffectMixingMode::SUB},
    {{RGBEffectPattern::ROTATION_THIN}, 2960, RGBEffectMixingMode::DIFFERENCE},
    {{RGBEffectPattern::ROTATION_SMOOTH}, 3000, RGBEffectMixingMode::REPLACE},
    {{RGBEffectPattern::ROTATION_SMOOTH}, 3030, RGBEffectMixingMode::SUB},
    {{RGBEffectPattern::ROTATION_SMOOTH}, 3060, RGBEffectMixingMode::DIFFERENCE},
    {{RGBEffectPattern::ROTATION_SMOOTH_THIN}, 3100, RGBEffectMixingMode::REPLACE},
    {{RGBEffectPattern::ROTATION_SMOOTH_THIN}, 3130, RGBEffectMixingMode::SUB},
    {{RGBEffectPattern::ROTATION_SMOOTH_THIN}, 3160, RGBEffectMixingMode::DIFFERENCE},
};
// clang-format on

RGBEffect::Desc sSmoothOffDesc{{RGBEffectPattern::SMOOTHER_ON_OFF}, 1000, RGBEffectMixingMode::SUB, RGBEffectColor::WHITE};
RGBEffect::Desc sFullOffDesc{{RGBEffectPattern::PLAIN}, 1000, RGBEffectMixingMode::REPLACE, RGBEffectColor::BLACK, Float::scaleUp(0)};
RGBEffect::Desc sSmoothStrobeDesc{{RGBEffectPattern::SMOOTHER_ON_OFF}, 800, RGBEffectMixingMode::MAX, RGBEffectColor::WHITE};
RGBEffect::Desc sVnrStrobeDesc{{RGBEffectPattern::STROBE}, 110, RGBEffectMixingMode::MAX, RGBEffectColor::WHITE};

using ColorVector = StaticVector2<RGBEffectColor, 64>;

// clang-format off
ColorVector sColors{
    RGBEffectColor::FLAME,
    RGBEffectColor::OCEAN,
    RGBEffectColor::GRASS,
    RGBEffectColor::RAINBOW,
    RGBEffectColor::PINK,
    RGBEffectColor::GOLD,
    RGBEffectColor::WHITEMIXED,
    RGBEffectColor::RED,
    RGBEffectColor::YELLOW,
    RGBEffectColor::GREEN,
    RGBEffectColor::CYAN,
    RGBEffectColor::BLUE,
    RGBEffectColor::MAGENTA,
    RGBEffectColor::ETHEREAL,
    RGBEffectColor::WARM_SUNSET,
    RGBEffectColor::DEEP_BLUE,
    RGBEffectColor::MARGUERITA,
    RGBEffectColor::PEACH,
    RGBEffectColor::FRESH,
    RGBEffectColor::VIOLET_SHADOWS,
    RGBEffectColor::DARK_SIDE,
};
// clang-format on

RGBEffect::PosArray RGBEffectWrapper::posArrayTower(StaticVector<int>& targetBuffer,
        bool hEn, Float hSize, Float hPos,
        bool vEn, Float vSize, Float vPos) {
    RGBEffect::PosArray posArray;
    posArray.width = TWR_WIDTH;
    posArray.height = TWR_HEIGHT;
    posArray.depth = 1;
    posArray.array = targetBuffer;
    int i = 0;
    // horiz
    for (; i < TWR_WIDTH; ++i) {
        int width = (TWR_WIDTH * hSize).scaleDown();
        int center = (TWR_WIDTH * hPos).scaleDown();
        int begin = center - width / 2;
        int end = center + width / 2;
        if (!hEn)
            posArray.array[i] = -1;
        else if (i < begin || i >= end)
            posArray.array[i] = -1;
        else
        {
            const int adv = i - begin;
            posArray.array[i] = (adv * TWR_WIDTH) / width;
        }
    }
    // vert
    for (; i < TWR_WIDTH + TWR_HEIGHT; ++i) {
        int j = i - TWR_WIDTH;

        int height = (TWR_HEIGHT * vSize).scaleDown();
        int center = (TWR_HEIGHT * vPos).scaleDown();
        int begin = center - height / 2;
        int end = center + height / 2;
        if (!vEn)
            posArray.array[i] = -1;
        else if (j < begin || j >= end)
            posArray.array[i] = -1;
        else
        {
            int adv = j - begin;

            //            x              y
            posArray.array[i] = TWR_STRIPLEN + ((adv * TWR_WIDTH) / height) * posArray.width;
        }
    }
    return posArray;
}

void RGBEffectWrapper::begin(uint8_t* pixels, int pixelCount) {
    LOGLN_VERBOSE("begin 1");

    _pixels = pixels;
    _pixelCount = pixelCount;
    begin();
}

void RGBEffectWrapper::flashSmoothOff(bool enable) {
    _smoothOffFlashing = enable;
}

void RGBEffectWrapper::flashFullOff(bool enable) {
    _fullOffFlashing = enable;
}

void RGBEffectWrapper::flashSmoothStrobe(bool enable) {
    _smoothStrobeFlashing = enable;
}

void RGBEffectWrapper::flashVnrStrobe(bool enable) {
    _vnrStrobeFlashing = enable;
}

void RGBEffectWrapper::pat0PrevMode() {
    _pat0EffectIdx = (_pat0EffectIdx + (sEffects.size - 1)) % (sEffects.size);

    pat0Begin();
}

void RGBEffectWrapper::pat0NextMode() {
    _pat0EffectIdx = (_pat0EffectIdx + 1) % (sEffects.size);

    pat0Begin();
}

void RGBEffectWrapper::pat1PrevMode() {
    _pat1EffectIdx = (_pat1EffectIdx + (sEffects.size - 1)) % (sEffects.size);

    pat1Begin();
}

void RGBEffectWrapper::pat1NextMode() {
    _pat1EffectIdx = (_pat1EffectIdx + 1) % (sEffects.size);

    pat1Begin();
}

void RGBEffectWrapper::pat0PrevColor() {
    _pat0ColorIdx = (_pat0ColorIdx + (sColors.size - 1)) % (sColors.size);

    updateColor();
}

void RGBEffectWrapper::pat0NextColor() {
    _pat0ColorIdx = (_pat0ColorIdx + 1) % (sColors.size);

    updateColor();
}

void RGBEffectWrapper::pat1PrevColor() {
    _pat1ColorIdx = (_pat1ColorIdx + (sColors.size - 1)) % (sColors.size);

    updateColor();
}

void RGBEffectWrapper::pat1NextColor() {
    _pat1ColorIdx = (_pat1ColorIdx + 1) % (sColors.size);

    updateColor();
}

void RGBEffectWrapper::pat0ChangeSpeed(Float multiplier) {
    LOGLN_DEBUG("pat0 speed %08x", (unsigned)multiplier.value);
    _pat0Speed = multiplier;
}

void RGBEffectWrapper::pat1ChangeSpeed(Float multiplier) {
    LOGLN_DEBUG("pat1 speed %08x", (unsigned)multiplier.value);
    _pat1Speed = multiplier;
}

void RGBEffectWrapper::pat0Enable(bool enable) {
    LOGLN_DEBUG("pat0 en %u", (unsigned)enable);
    _pat0Enable = enable;
}

void RGBEffectWrapper::pat1Enable(bool enable) {
    LOGLN_DEBUG("pat1 en %u", (unsigned)enable);
    _pat1Enable = enable;
}

void RGBEffectWrapper::pat0ChangeDim(Float dim) {
    LOGLN_DEBUG("pat0 dim %08x", (unsigned)dim.value);
    _pat0Dim = dim;
    _pat0Effect.setDimmer(_pat0Dim);
}

void RGBEffectWrapper::pat1ChangeDim(Float dim) {
    LOGLN_DEBUG("pat1 dim %08x", (unsigned)dim.value);
    _pat1Dim = dim;
    _pat1Effect.setDimmer(_pat1Dim);
}

void RGBEffectWrapper::hPartEnable(bool enable) {
    _hPartEnable = enable;
    updatePosArray();
}

void RGBEffectWrapper::vPartEnable(bool enable) {
    _vPartEnable = enable;
    updatePosArray();
}

void RGBEffectWrapper::hPartSize(Float size) {
    _hPartSize = size;
    updatePosArray();
}

void RGBEffectWrapper::vPartSize(Float size) {
    _vPartSize = size;
    updatePosArray();
}

void RGBEffectWrapper::hPartPos(Float pos) {
    _hPartPos = pos;
    updatePosArray();
}

void RGBEffectWrapper::vPartPos(Float pos) {
    _vPartPos = pos;
    updatePosArray();
}

bool RGBEffectWrapper::refreshPixels(unsigned long currentMillis) {
    static unsigned long prevMillis = 0;
    static unsigned long pat0PrevEffectMillis = 0;
    static unsigned long pat1PrevEffectMillis = 0;

    auto dMillis = currentMillis - prevMillis;

    if (dMillis < 16) // ~60fps
        return false;

    bool ret = false;

    // pat0
    {
        unsigned long effectMillis = pat0PrevEffectMillis + (dMillis * _pat0Speed).scaleDown();
        if (_pat0Enable)
            ret |= _pat0Effect.refreshPixels(effectMillis);
        pat0PrevEffectMillis = effectMillis;
    }

    // pat1
    {
        unsigned long effectMillis = pat1PrevEffectMillis + (dMillis * _pat1Speed).scaleDown();
        if (_pat1Enable)
            ret |= _pat1Effect.refreshPixels(effectMillis);
        pat1PrevEffectMillis = effectMillis;
    }

    {
        unsigned long effectMillis = prevMillis + dMillis;
        if (_smoothOffFlashing)
            ret |= _smoothOffEffect.refreshPixels(effectMillis);
        if (_fullOffFlashing)
            ret |= _fullOffEffect.refreshPixels(effectMillis);
        if (_smoothStrobeFlashing)
            ret |= _smoothStrobeEffect.refreshPixels(effectMillis);
        if (_vnrStrobeFlashing)
            ret |= _vnrStrobeEffect.refreshPixels(effectMillis);
    }

    prevMillis = currentMillis;

    return ret;
}

void RGBEffectWrapper::updateColor() {
    _pat0Effect.setColor(sColors[_pat0ColorIdx]);
    _pat1Effect.setColor(sColors[_pat1ColorIdx]);
}

void RGBEffectWrapper::updatePosArray() {
    posArray = posArrayTower(posArrayBuffer,
            _hPartEnable, _hPartSize, _hPartPos,
            _vPartEnable, _vPartSize, _vPartPos);;
}

void RGBEffectWrapper::begin() {

    LOGLN_VERBOSE("start effects");

    pat0Begin();
    pat1Begin();

    LOGLN_VERBOSE("start strobe effects");

    _smoothOffEffect.begin(sSmoothOffDesc, _pixels, _pixelCount, posArray);
    _fullOffEffect.begin(sFullOffDesc, _pixels, _pixelCount, posArray);
    _smoothStrobeEffect.begin(sSmoothStrobeDesc, _pixels, _pixelCount, posArray);
    _vnrStrobeEffect.begin(sVnrStrobeDesc, _pixels, _pixelCount, posArray);
}

void RGBEffectWrapper::pat0Begin() {
    auto effectDesc = sEffects[_pat0EffectIdx];
    effectDesc.color = sColors[_pat0ColorIdx];
    effectDesc.dimmer = _pat0Dim;
    effectDesc.mixingMode = RGBEffectMixingMode::REPLACE;
    _pat0Effect.begin(effectDesc, _pixels, _pixelCount, posArray);
}

void RGBEffectWrapper::pat1Begin() {
    auto effectDesc = sEffects[_pat1EffectIdx];
    effectDesc.color = sColors[_pat1ColorIdx];
    effectDesc.dimmer = _pat1Dim;
    if (effectDesc.mixingMode == RGBEffectMixingMode::REPLACE)
        effectDesc.mixingMode = RGBEffectMixingMode::ADD;
    _pat1Effect.begin(effectDesc, _pixels, _pixelCount, posArray);
}
