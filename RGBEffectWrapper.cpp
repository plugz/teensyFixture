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

RGBEffect::Desc smoothOffDesc{{RGBEffectPattern::SMOOTHER_ON_OFF}, 1000, RGBEffectMixingMode::SUB, RGBEffectColor::WHITE};
RGBEffect::Desc fullOffDesc{{RGBEffectPattern::STROBE}, 1000, RGBEffectMixingMode::REPLACE, RGBEffectColor::WHITE, Float::scaleUp(0)};
RGBEffect::Desc smoothStrobeDesc{{RGBEffectPattern::SMOOTHER_ON_OFF}, 800, RGBEffectMixingMode::MAX, RGBEffectColor::WHITE};
RGBEffect::Desc vnrStrobeDesc{{RGBEffectPattern::STROBE}, 120, RGBEffectMixingMode::MAX, RGBEffectColor::WHITE};

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

void RGBEffectWrapper::startFlash() { _flashing = true; }

void RGBEffectWrapper::stopFlash() { _flashing = false; }

void RGBEffectWrapper::prevMode() {
    _currentEffectsIdx = (_currentEffectsIdx + (sEffects.size - 1)) % (sEffects.size);

    begin();
}

void RGBEffectWrapper::nextMode() {
    _currentEffectsIdx = (_currentEffectsIdx + 1) % (sEffects.size);

    begin();
}

void RGBEffectWrapper::begin() {
    LOGLN_VERBOSE("begin 2");

    LOGLN_VERBOSE("start effects");
    unsigned int idx = 0;
    for (auto effectDesc : sEffects[_currentEffectsIdx].effects) {
        LOGLN_VERBOSE("idx=%u", idx);
        unsigned int colorIdx = std::min(idx, sColors[_currentColorsIdx].colors.size);

        LOGLN_VERBOSE("colorIdx=%u, _currentColorsIdx=%u", colorIdx, _currentColorsIdx);
        ++_currentEffects.size;

        effectDesc.color = sColors[_currentColorsIdx].colors[colorIdx];
        LOGLN_VERBOSE("begincurrentEffect");
        _currentEffects[idx].begin(effectDesc, _pixels, _pixelCount, posArray);
        ++idx;
    }

    LOGLN_VERBOSE("start strobe effects");
    idx = 0;
    for (auto strobeEffectDesc : sEffects[_currentEffectsIdx].strobeEffects) {
        unsigned int colorIdx = std::min(idx, sColors[_currentColorsIdx].strobeColors.size);

        ++_currentStrobeEffects.size;

        strobeEffectDesc.color = sColors[_currentColorsIdx].colors[colorIdx];
        _currentStrobeEffects[idx].begin(strobeEffectDesc, _pixels, _pixelCount, posArray);
        ++idx;
    }
}

void RGBEffectWrapper::prevColor() {
    _currentColorsIdx = (_currentColorsIdx + (sColors.size - 1)) % (sColors.size);

    updateColor();
}

void RGBEffectWrapper::nextColor() {
    _currentColorsIdx = (_currentColorsIdx + 1) % (sColors.size);

    updateColor();
}

void RGBEffectWrapper::updateColor() {
    unsigned int idx = 0;
    for (auto& effect : _currentEffects) {
        unsigned int colorIdx = MYMIN(idx, sColors[_currentColorsIdx].colors.size);
        effect.setColor(sColors[_currentColorsIdx].colors[colorIdx]);
        ++idx;
    }
    idx = 0;
    for (auto& strobeEffect : _currentStrobeEffects) {
        unsigned int colorIdx = MYMIN(idx, sColors[_currentColorsIdx].strobeColors.size);
        strobeEffect.setColor(sColors[_currentColorsIdx].strobeColors[colorIdx]);
        ++idx;
    }
}

void RGBEffectWrapper::changeSpeed(Float speed) {
    _speed = speed;
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

    for (auto& effect : _currentEffects)
        ret |= effect.refreshPixels(effectMillis);

    if (_flashing) {
        for (auto& strobeEffect : _currentStrobeEffects)
            ret |= strobeEffect.refreshPixels(effectMillis);
    }

    prevMillis = currentMillis;
    prevEffectMillis = effectMillis;

    return ret;
}
