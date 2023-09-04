#include "RGBEffectWrapper.hpp"

#include "Log.hpp"

#define MYMIN(x, y) ((x) < (y) ? (x) : (y))

#define NUM_LEDS_PER_STRIP ADJ_HEIGHT
#define NUM_STRIPS (8)
#define NUM_LEDS (NUM_LEDS_PER_STRIP * NUM_STRIPS) // can not go higher than this - Runs out of SRAM

int posArrayBufferArray[NUM_LEDS] = {0};
StaticVector<int> posArrayBuffer{posArrayBufferArray, ARRAY_COUNT(posArrayBufferArray)};

const RGBEffect::PosArray RGBEffectWrapper::posArray =
    RGBEffect::posArrayTower(posArrayBuffer);

///**/
//    RGBEffect::posArraySimple(NUM_LEDS_PER_STRIP, 1);//NUM_STRIPS);
//    RGBEffect::posArraySimple(60, 1);//NUM_STRIPS);
/**/
/** /
    RGBEffect::posArrayFromLedArray({// Fairy Wings V3
        -1, -1, 24, -1, 23, -1, -1,    -1, -1, 14, -1, 13, -1, -1,
        25, -1, -1, -1, -1, -1, -1,    -1, -1, -1, -1, -1, -1, 12,
        -1, -1, -1, -1, -1, 22, -1,    -1, 15, -1, -1, -1, -1, -1,
        -1, 26, -1, 27, -1, -1, -1,    -1, -1, -1, 10, -1, 11, -1,
        -1, -1, -1, -1, -1, 28, 21,    16,  9, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1,    -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, 29, -1, -1, 20,    17, -1, -1,  8, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1,    -1, -1, -1, -1, -1, -1, -1,
        -1, 30, -1, 31, -1, 32, 19,    18,  5, -1,  6, -1,  7, -1,
        -1, -1, -1, -1, -1, -1, -1,    -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, 33, -1, -1, -1,    -1, -1, -1,  4, -1, -1, -1,
        -1, 34, -1, -1, -1, -1, 37,     0, -1, -1, -1, -1,  3, -1,
        -1, -1, -1, -1, -1, -1, -1,    -1, -1, -1, -1, -1, -1, -1,
        -1, -1, 35, -1, 36, -1, -1,    -1, -1,  1, -1,  2, -1, -1,
        }, 14, 14);
/ **/
/** /
    RGBEffect::posArrayFromLedArray({// Eventail
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 17, -1, 18, -1, 19, -1, 20, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 16, -1, -1, -1, -1, -1, -1, -1, -1, -1, 21, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 22, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 14, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, 13, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 24, -1, -1, -1, 28, 29, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, 12, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 25, -1, -1, -1, 27, -1, -1, -1, -1, 30, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 26, -1, -1, -1, -1, -1, -1, -1, -1, 31, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 32, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 33, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         8, -1, -1, -1, -1, -1, -1, -1,  3, -1,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 34, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 35, -1, -1,
        -1,  7, -1, -1, -1, -1, -1,  4, -1, -1, -1,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 36, -1, -1, -1, -1,
        -1, -1, -1,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 37, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1,  5, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 38, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 39, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        }, 40, 18);
/ **/
/** /
    RGBEffect::posArrayFromLedArray({ // Jupe
        -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 39, -1, 38, -1, -1,
        -1, -1, -1, -1, -1, -1,  2, -1,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, -1, 37,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1,  9, -1, 11, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 36,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  6, -1,  7, -1,  8, -1, -1, -1, 12, -1,
        23, -1, 22, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 35, -1,
        -1, -1, -1, -1, 21, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 13, -1, -1, -1,
        24, -1, -1, -1, -1, -1, 20, -1, 19, -1, 18, -1, 17, -1, 16, -1, 15, -1, 14, -1, 34, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, 25, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 33, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, 26, -1, 27, -1, 28, -1, 29, -1, 30, -1, 31, -1, 32, -1, -1, -1, -1, -1, -1, -1,
        }, 24, 12);
/ **/

using EffectDescVector = StaticVector2<RGBEffect::Desc, 64>;

struct EffectComboDesc {
    EffectDescVector effects;
    EffectDescVector strobeEffects;
};

using EffectComboDescVector = StaticVector2<EffectComboDesc, 16>;

// clang-format off
EffectComboDescVector const sEffects{
    {
        {{{RGBEffectPattern::PLASMA}, 4000, RGBEffectMixingMode::REPLACE}},
        {{{RGBEffectPattern::STROBE}, 160, RGBEffectMixingMode::MAX}}
    },
    {
        {{{RGBEffectPattern::PLASMA}, 4000, RGBEffectMixingMode::REPLACE},
         {{RGBEffectPattern::STRIPE_SMOOTH}, 1700, RGBEffectMixingMode::SUB}},
        {{{RGBEffectPattern::SMOOTHER_ON_OFF}, 160, RGBEffectMixingMode::MAX}}
    },
    {
        {{{RGBEffectPattern::ROTATION}, 1700, RGBEffectMixingMode::REPLACE},
         {{RGBEffectPattern::ROTATION_THIN}, 400, RGBEffectMixingMode::SUB}},
        {{{RGBEffectPattern::ROTATION}, 400, RGBEffectMixingMode::GRAINMERGE}}
    },
    {
        {{{RGBEffectPattern::ROTATION, RGBEffectAxis::VERTICAL}, 1700, RGBEffectMixingMode::REPLACE},
         {{RGBEffectPattern::ROTATION_THIN, RGBEffectAxis::VERTICAL}, 400, RGBEffectMixingMode::SUB}},
        {{{RGBEffectPattern::ROTATION}, 400, RGBEffectMixingMode::GRAINMERGE}}
    },
    {
        {{{RGBEffectPattern::ROTATION, RGBEffectAxis::FRONTAL}, 1700, RGBEffectMixingMode::REPLACE},
         {{RGBEffectPattern::ROTATION_THIN, RGBEffectAxis::FRONTAL}, 400, RGBEffectMixingMode::SUB}},
        {{{RGBEffectPattern::ROTATION}, 400, RGBEffectMixingMode::GRAINMERGE}}
    },
    {
        {{{RGBEffectPattern::ROTATION_SMOOTH}, 1300, RGBEffectMixingMode::ADD},
         {{RGBEffectPattern::ROTATION_SMOOTH_THIN}, 4000, RGBEffectMixingMode::SUB}},
        {{{RGBEffectPattern::ROTATION}, 400, RGBEffectMixingMode::GRAINMERGE}}
    },
    {
        {{{RGBEffectPattern::STRIPE_SMOOTH, RGBEffectAxis::VERTICAL, RGBEffectDirection::FORWARD}, 1700, RGBEffectMixingMode::REPLACE},
         {{RGBEffectPattern::STRIPE, RGBEffectAxis::VERTICAL, RGBEffectDirection::FORWARD}, 1200, RGBEffectMixingMode::MAX}},
        {{{RGBEffectPattern::STRIPE_SMOOTH, RGBEffectAxis::VERTICAL, RGBEffectDirection::FORWARD}, 280, RGBEffectMixingMode::MAX}}
    },
    {
        {{{RGBEffectPattern::STRIPE_SMOOTH, RGBEffectAxis::HORIZONTAL, RGBEffectDirection::BACKWARD}, 1700, RGBEffectMixingMode::REPLACE},
         {{RGBEffectPattern::STRIPE, RGBEffectAxis::HORIZONTAL, RGBEffectDirection::BACKWARD}, 1200, RGBEffectMixingMode::MAX}},
        {{{RGBEffectPattern::STRIPE_SMOOTH, RGBEffectAxis::HORIZONTAL, RGBEffectDirection::BACKWARD}, 280, RGBEffectMixingMode::MAX}}
    },
    {
        {{{RGBEffectPattern::STRIPE_SMOOTH, RGBEffectAxis::FRONTAL, RGBEffectDirection::PING_PONG}, 1700, RGBEffectMixingMode::REPLACE},
         {{RGBEffectPattern::STRIPE, RGBEffectAxis::FRONTAL, RGBEffectDirection::PING_PONG}, 1200, RGBEffectMixingMode::MAX}},
        {{{RGBEffectPattern::STRIPE_SMOOTH, RGBEffectAxis::FRONTAL, RGBEffectDirection::PING_PONG}, 280, RGBEffectMixingMode::MAX}}
    },
};
// clang-format on

using ColorVector = StaticVector2<RGBEffectColor, 64>;

struct ColorComboDesc {
    ColorVector colors;
    ColorVector strobeColors;
};

using ColorComboVector = StaticVector2<ColorComboDesc, 16>;

// clang-format off

ColorComboVector sColors{
    {
        {RGBEffectColor::OCEAN, RGBEffectColor::WHITE},
        {RGBEffectColor::WHITE}
    },
    {
        {RGBEffectColor::FLAME, RGBEffectColor::WHITE},
        {RGBEffectColor::WHITE}
    },
    {
        {RGBEffectColor::GRASS, RGBEffectColor::WHITE},
        {RGBEffectColor::WHITE}
    },
    {
        {RGBEffectColor::RAINBOW, RGBEffectColor::WHITE},
        {RGBEffectColor::WHITE}
    },
    {
        {RGBEffectColor::PINK, RGBEffectColor::WHITE},
        {RGBEffectColor::WHITE}
    },
    {
        {RGBEffectColor::GOLD, RGBEffectColor::WHITE},
        {RGBEffectColor::WHITE}
    }
};

// clang-format on

void RGBEffectWrapper::begin(uint8_t* pixels, int pixelCount) {
    LOGLN_VERBOSE("begin 1");

    static RGBEffect currentEffectsBuffer[16];
    _currentEffects.data = currentEffectsBuffer;
    static RGBEffect currentStrobeEffectsBuffer[16];
    _currentStrobeEffects.data = currentStrobeEffectsBuffer;

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
    _currentEffects.size = 0;
    _currentStrobeEffects.size = 0;

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
