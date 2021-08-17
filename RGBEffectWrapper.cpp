#include "RGBEffectWrapper.hpp"

#include "Log.hpp"

#define MYMIN(x, y) ((x) < (y) ? (x) : (y))

#define NUM_LEDS_PER_STRIP 120
#define NUM_STRIPS (8)
#define NUM_LEDS (NUM_LEDS_PER_STRIP * NUM_STRIPS) // can not go higher than this - Runs out of SRAM

int posArrayBufferArray[NUM_LEDS] = {0};
StaticVector<int> posArrayBuffer{posArrayBufferArray, ARRAY_COUNT(posArrayBufferArray)};

const RGBEffect::PosArray RGBEffectWrapper::posArray =
    RGBEffect::posArrayAirDJ(posArrayBuffer, NUM_LEDS_PER_STRIP, NUM_STRIPS);

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

struct EffectDesc {
    RGBEffectPattern pattern;
    RGBEffectMixingMode mixingMode;
    int loopTime;
};

using EffectDescVector = StaticVector2<EffectDesc, 64>;

struct EffectComboDesc {
    EffectDescVector effects;
    EffectDescVector strobeEffects;
};

using EffectComboDescVector = StaticVector2<EffectComboDesc, 16>;

// clang-format off
EffectComboDescVector const sEffects{
    {
        {{RGBEffectPattern::PLASMA, RGBEffectMixingMode::REPLACE, 4000}},
        {{RGBEffectPattern::STROBE, RGBEffectMixingMode::MAX, 160}}
    },
    {
        {{RGBEffectPattern::PLASMA, RGBEffectMixingMode::REPLACE, 4000},
         {RGBEffectPattern::STRIPE_SMOOTH_H_LEFT_RIGHT, RGBEffectMixingMode::SUB, 1700}},
        {{RGBEffectPattern::SMOOTHER_ON_OFF, RGBEffectMixingMode::MAX, 160}}
    },
    {
        {{RGBEffectPattern::ROTATION, RGBEffectMixingMode::REPLACE, 1700},
         //{RGBEffectPattern::ROTATION_SMOOTH_THIN, RGBEffectMixingMode::MULTIPLY, 1400}
         },
        {{RGBEffectPattern::ROTATION, RGBEffectMixingMode::GRAINMERGE, 400}}
    },
    {
        {{RGBEffectPattern::ROTATION_SMOOTH, RGBEffectMixingMode::ADD, 1300},
         {RGBEffectPattern::ROTATION_SMOOTH_THIN, RGBEffectMixingMode::SUB, 4000}},
        {{RGBEffectPattern::ROTATION, RGBEffectMixingMode::GRAINMERGE, 400}}
    },
    {
        {{RGBEffectPattern::STRIPE_SMOOTH_V_DOWN_UP, RGBEffectMixingMode::REPLACE, 1700},
         {RGBEffectPattern::STRIPE_V_DOWN_UP, RGBEffectMixingMode::MAX, 1200}},
        {{RGBEffectPattern::PING_PONG_SMOOTH_V, RGBEffectMixingMode::MAX, 280}}
    },
    {
        {{RGBEffectPattern::STRIPE_SMOOTH_H_LEFT_RIGHT, RGBEffectMixingMode::REPLACE, 1700},
         {RGBEffectPattern::STRIPE_H_LEFT_RIGHT, RGBEffectMixingMode::MAX, 4800}},
        {{RGBEffectPattern::PING_PONG_SMOOTH_V, RGBEffectMixingMode::MAX, 280}}
    }
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
    for (auto& effectDesc : sEffects[_currentEffectsIdx].effects) {
        LOGLN_VERBOSE("idx=%u", idx);
        unsigned int colorIdx = MYMIN(idx, sColors[_currentColorsIdx].colors.size);

        LOGLN_VERBOSE("colorIdx=%u, _currentColorsIdx=%u", colorIdx, _currentColorsIdx);
        ++_currentEffects.size;

        LOGLN_VERBOSE("begincurrentEffect");
        _currentEffects[idx].begin(effectDesc.pattern, sColors[_currentColorsIdx].colors[colorIdx],
                                   effectDesc.mixingMode, _pixels, _pixelCount, posArray);
        LOGLN_VERBOSE("setLoopTim");
        _currentEffects[idx].setLoopTime(effectDesc.loopTime);
        ++idx;
    }

    LOGLN_VERBOSE("start strobe effects");
    idx = 0;
    for (auto& strobeEffectDesc : sEffects[_currentEffectsIdx].strobeEffects) {
        unsigned int colorIdx = MYMIN(idx, sColors[_currentColorsIdx].strobeColors.size);

        ++_currentStrobeEffects.size;

        _currentStrobeEffects[idx].begin(
            strobeEffectDesc.pattern, sColors[_currentColorsIdx].strobeColors[colorIdx],
            strobeEffectDesc.mixingMode, _pixels, _pixelCount, posArray);
        _currentStrobeEffects[idx].setLoopTime(strobeEffectDesc.loopTime);
        ++idx;
    }
}

void RGBEffectWrapper::nextColor() {
    _currentColorsIdx = (_currentColorsIdx + 1) % (sColors.size);

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

bool RGBEffectWrapper::refreshPixels(unsigned long currentMillis) {
    bool ret = false;

    for (auto& effect : _currentEffects)
        ret |= effect.refreshPixels(currentMillis);

    if (_flashing) {
        for (auto& strobeEffect : _currentStrobeEffects)
            ret |= strobeEffect.refreshPixels(currentMillis);
    }

    return ret;
}
