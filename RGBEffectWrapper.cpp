#include "RGBEffectWrapper.hpp"

#include "Log.hpp"

#define MYMIN(x, y) ((x) < (y) ? (x) : (y))

#define NUM_LEDS_PER_STRIP TWR_STRIPLEN
#define NUM_STRIPS (8)
#define NUM_LEDS (NUM_LEDS_PER_STRIP * NUM_STRIPS) // can not go higher than this - Runs out of SRAM

int posArrayBufferArray[NUM_LEDS] = {0};
StaticVector<int> posArrayBuffer{posArrayBufferArray, ARRAY_COUNT(posArrayBufferArray)};

const RGBEffect::PosArray RGBEffectWrapper::posArray =
    RGBEffect::posArrayTower(posArrayBuffer);

using EffectDescVector = StaticVector2<RGBEffect::Desc, 64>;

// clang-format off
EffectDescVector const sEffects{
    {
        {RGBEffectPattern::PLASMA}, 4000, RGBEffectMixingMode::REPLACE
    },
    {
        {RGBEffectPattern::STRIPE_SMOOTH}, 1700, RGBEffectMixingMode::SUB
    },
    {
        {RGBEffectPattern::ROTATION}, 1700, RGBEffectMixingMode::REPLACE
    },
    {
        {RGBEffectPattern::ROTATION_THIN}, 400, RGBEffectMixingMode::SUB
    },
    {
        {RGBEffectPattern::ROTATION, RGBEffectAxis::VERTICAL}, 1700, RGBEffectMixingMode::REPLACE
    },
    {
        {RGBEffectPattern::ROTATION_THIN, RGBEffectAxis::VERTICAL}, 400, RGBEffectMixingMode::SUB
    },
    {
        {RGBEffectPattern::ROTATION_SMOOTH}, 1300, RGBEffectMixingMode::ADD
    },
    {
        {RGBEffectPattern::ROTATION_SMOOTH_THIN}, 4000, RGBEffectMixingMode::SUB
    },
    {
        {RGBEffectPattern::STRIPE_SMOOTH, RGBEffectAxis::VERTICAL, RGBEffectDirection::FORWARD}, 1700, RGBEffectMixingMode::REPLACE
    },
    {
        {RGBEffectPattern::STRIPE, RGBEffectAxis::VERTICAL, RGBEffectDirection::FORWARD}, 1200, RGBEffectMixingMode::MAX
    },
    {
        {RGBEffectPattern::STRIPE_SMOOTH, RGBEffectAxis::HORIZONTAL, RGBEffectDirection::BACKWARD}, 1700, RGBEffectMixingMode::REPLACE
    },
    {
        {RGBEffectPattern::STRIPE, RGBEffectAxis::HORIZONTAL, RGBEffectDirection::BACKWARD}, 1200, RGBEffectMixingMode::MAX
    },
};
// clang-format on

RGBEffect::Desc sSmoothOffDesc{{RGBEffectPattern::SMOOTHER_ON_OFF}, 1000, RGBEffectMixingMode::SUB, RGBEffectColor::WHITE};
RGBEffect::Desc sFullOffDesc{{RGBEffectPattern::STROBE}, 1000, RGBEffectMixingMode::REPLACE, RGBEffectColor::WHITE, Float::scaleUp(0)};
RGBEffect::Desc sSmoothStrobeDesc{{RGBEffectPattern::SMOOTHER_ON_OFF}, 800, RGBEffectMixingMode::MAX, RGBEffectColor::WHITE};
RGBEffect::Desc sVnrStrobeDesc{{RGBEffectPattern::STROBE}, 120, RGBEffectMixingMode::MAX, RGBEffectColor::WHITE};

using ColorVector = StaticVector2<RGBEffectColor, 64>;

// clang-format off
ColorVector sColors{
    RGBEffectColor::OCEAN,
    RGBEffectColor::FLAME,
    RGBEffectColor::GRASS,
    RGBEffectColor::RAINBOW,
    RGBEffectColor::PINK,
    RGBEffectColor::GOLD
};
// clang-format on

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
    _pat0Speed = multiplier;
}

void RGBEffectWrapper::pat1ChangeSpeed(Float multiplier) {
    _pat1Speed = multiplier;
}

void RGBEffectWrapper::pat0Enable(bool enable) {
    _pat0Enable = enable;
}

void RGBEffectWrapper::pat1Enable(bool enable) {
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
    _pat1Effect.setDimmer(_pat0Dim);
}

bool RGBEffectWrapper::refreshPixels(unsigned long currentMillis) {
    static unsigned long prevMillis = 0;
    static unsigned long prevEffectMillis = 0;

    // TODO
//    auto dMillis = Float::scaleUp(currentMillis - prevMillis) * _speed;
//    unsigned long effectMillis = prevEffectMillis + dMillis.scaleDown();
    auto dMillis = currentMillis - prevMillis;
    unsigned long effectMillis = prevEffectMillis + dMillis;

    bool ret = false;

    // pat0
    if (_pat0Enable)
    {
        ret |= _pat0Effect.refreshPixels(effectMillis);
    }

    // pat1
    if (_pat1Enable)
    {
        ret |= _pat1Effect.refreshPixels(effectMillis);
    }

    if (_smoothOffFlashing)
        ret |= _smoothOffEffect.refreshPixels(effectMillis);
    if (_fullOffFlashing)
        ret |= _fullOffEffect.refreshPixels(effectMillis);
    if (_smoothStrobeFlashing)
        ret |= _smoothStrobeEffect.refreshPixels(effectMillis);
    if (_vnrStrobeFlashing)
        ret |= _vnrStrobeEffect.refreshPixels(effectMillis);

    prevMillis = currentMillis;
    prevEffectMillis = effectMillis;

    return ret;
}

void RGBEffectWrapper::updateColor() {
    _pat0Effect.setColor(sColors[_pat0ColorIdx]);
    _pat1Effect.setColor(sColors[_pat1ColorIdx]);
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
    _pat0Effect.begin(effectDesc, _pixels, _pixelCount, posArray);
}

void RGBEffectWrapper::pat1Begin() {
    auto effectDesc = sEffects[_pat1EffectIdx];
    effectDesc.color = sColors[_pat1ColorIdx];
    effectDesc.dimmer = _pat1Dim;
    _pat1Effect.begin(effectDesc, _pixels, _pixelCount, posArray);
}
