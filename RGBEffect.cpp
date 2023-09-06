#include "RGBEffect.hpp"
#include "simplexnoise1234.h"

#include "Log.hpp"

#include <cstring>
#include <algorithm>
#include <cmath>
#include <iterator>

//#include <iostream>

#ifdef abs
#undef abs
#endif
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

template<typename T>
static inline T loopedDistance(T const a, T const b, T const size)
{
    return std::min(
                std::abs(a - b),
                std::min(
                    std::abs((a + size) - b),
                    std::abs((a - size) - b))
                );
}

void RGBEffect::begin(Desc const& desc,
                      uint8_t *pixels,
                      unsigned int pixelCount,
                      PosArray const& posArray)
{
    _desc = desc;
    setMixingMode(_desc.mixingMode);
    _pixels = pixels;
    _pixelCount = pixelCount;
    _posArray = posArray;

    beginCurrentCombo();
}

RGBEffect::PosArray RGBEffect::posArraySimple(StaticVector<int>& targetBuffer, unsigned int width,
                                              unsigned int height, unsigned int depth) {
    PosArray posArray;
    posArray.width = width;
    posArray.height = height;
    posArray.depth = depth;
    posArray.array = targetBuffer;
    for (unsigned int i = 0; i < width * height * depth; ++i)
        posArray.array[i] = i;
    return posArray;
}

RGBEffect::PosArray RGBEffect::posArrayZigZag(StaticVector<int>& targetBuffer, unsigned int width,
                                              unsigned int height, unsigned int depth) {
    PosArray posArray;
    posArray.width = width;
    posArray.height = height;
    posArray.depth = depth;
    posArray.array = targetBuffer;
    for (unsigned int i = 0; i < width * height * depth; ++i)
    {
        if ((i / width) % 2)
            posArray.array[i] = (i / width) * width + ((width - 1) - (i % width));
        else
            posArray.array[i] = i;
    }
    return posArray;
}

RGBEffect::PosArray RGBEffect::posArrayFromLedArray(StaticVector<int>& targetBuffer,
                                                    unsigned int width, unsigned int height,
                                                    unsigned int depth,
                                                    StaticVector<int> const& ledArray) {
    PosArray posArray;
    posArray.width = width;
    posArray.height = height;
    posArray.depth = depth;
    posArray.array = targetBuffer;

    int ledMax = 0;
    for (auto led : ledArray)
    {
        if (led > ledMax)
            ledMax = led;
    }
    const int ledCount = ledMax + 1;

    for (int i = 0; i < ledCount; ++i)
    {
        auto ledArrayIt = std::find(ledArray.begin(), ledArray.end(), i);
        if (ledArrayIt != ledArray.end())
        {
            posArray.array[i] = ledArrayIt - ledArray.begin();
        }
        else
        {
            posArray.array[i] = -1;
        }
    }

    return posArray;
}


RGBEffect::PosArray RGBEffect::posArrayAirDJ(StaticVector<int>& targetBuffer) {
    PosArray posArray;
    posArray.width = ADJ_WIDTH;
    posArray.height = ADJ_HEIGHT;
    posArray.depth = ADJ_DEPTH;
    posArray.array = targetBuffer;
    // tower
    for (unsigned int i = ADJ_HEIGHT * 0; i < ADJ_HEIGHT * 1; ++i) {
        unsigned int j = i - ADJ_HEIGHT * 0;
        if (j < 19) {
            posArray.array[i] = (ADJ_WIDTH - 45 + j) + ADJ_WIDTH * 0 + ADJ_WIDTH * ADJ_HEIGHT * (10 + j / 3);
        }
        else if (j < 40) {
            posArray.array[i] = (ADJ_WIDTH - 45 + j) + ADJ_WIDTH * 8 + ADJ_WIDTH * ADJ_HEIGHT * (10 + j / 3);
        }
        else
            posArray.array[i] = -1;
    }
    // vtech centre
    for (unsigned int i = ADJ_HEIGHT * 1; i < ADJ_HEIGHT * 2; ++i) {
        unsigned int j = i - ADJ_HEIGHT * 1;
        if (j < 30)
            posArray.array[i] = (ADJ_WIDTH / 2 + 15 - j) + ADJ_WIDTH * 0 + ADJ_WIDTH * ADJ_HEIGHT * 10;
        else
            posArray.array[i] = -1;
    }
    // fisher price
    for (unsigned int i = ADJ_HEIGHT * 2; i < ADJ_HEIGHT * 3; ++i) {
        unsigned int j = i - ADJ_HEIGHT * 2;
        posArray.array[i] = i;
        if (j < 30)
            posArray.array[i] = (5 + j) + ADJ_WIDTH * 0 + ADJ_WIDTH * ADJ_HEIGHT * (15 - j / 3);
        else
            posArray.array[i] = -1;
    }
    // rien
    for (unsigned int i = ADJ_HEIGHT * 3; i < ADJ_HEIGHT * 4; ++i) {
        // unsigned int j = i - ADJ_HEIGHT * 3;
        posArray.array[i] = -1;
    }
    // poto jardin loin
    for (unsigned int i = ADJ_HEIGHT * 4; i < ADJ_HEIGHT * 5; ++i) {
        unsigned int j = i - ADJ_HEIGHT * 4;
        posArray.array[i] = 0 + ADJ_WIDTH * j + ADJ_WIDTH * ADJ_HEIGHT * (ADJ_DEPTH - 1);
    }
    // poto cour loin
    for (unsigned int i = ADJ_HEIGHT * 5; i < ADJ_HEIGHT * 6; ++i) {
        unsigned int j = i - ADJ_HEIGHT * 5;
        posArray.array[i] = (ADJ_WIDTH - 1) + ADJ_WIDTH * j + ADJ_WIDTH * ADJ_HEIGHT * (ADJ_DEPTH - 1);
    }
    // poto cour proche
    for (unsigned int i = ADJ_HEIGHT * 6; i < ADJ_HEIGHT * 7; ++i) {
        unsigned int j = i - ADJ_HEIGHT * 6;
        posArray.array[i] = (ADJ_WIDTH - 1) + ADJ_WIDTH * j + ADJ_WIDTH * ADJ_HEIGHT * 0;
    }
    // poto jardin proche
    for (unsigned int i = ADJ_HEIGHT * 7; i < ADJ_HEIGHT * 8; ++i) {
        unsigned int j = i - ADJ_HEIGHT * 7;
        posArray.array[i] = 0 + ADJ_WIDTH * j + ADJ_WIDTH * ADJ_HEIGHT * 0;
    }
    return posArray;
}

void RGBEffect::setPattern(RGBEffectPattern pattern)
{
    _desc.pattern.pattern = pattern;
    beginCurrentCombo();
}

void RGBEffect::setAxis(RGBEffectAxis axis)
{
    _desc.pattern.axis = axis;
    beginCurrentCombo();
}

void RGBEffect::setDirection(RGBEffectDirection direction)
{
    _desc.pattern.direction = direction;
    beginCurrentCombo();
}

void RGBEffect::setColor(RGBEffectColor color)
{
    _desc.color = color;
//    beginCurrentCombo();
}

void RGBEffect::setDimmer(Float dim)
{
    _desc.dimmer = dim;
}

void RGBEffect::setMixingMode(RGBEffectMixingMode mixingMode)
{
    _desc.mixingMode = mixingMode;
    switch (mixingMode)
    {
    default:
    case RGBEffectMixingMode::REPLACE:
        mixPixel = mixPixelReplace;
        break;
    case RGBEffectMixingMode::MAX:
        mixPixel = mixPixelMax;
        break;
    case RGBEffectMixingMode::MIN:
        mixPixel = mixPixelMin;
        break;
    case RGBEffectMixingMode::ADD:
        mixPixel = mixPixelAdd;
        break;
    case RGBEffectMixingMode::SUB:
        mixPixel = mixPixelSub;
        break;
    case RGBEffectMixingMode::MULTIPLY:
        mixPixel = mixPixelMultiply;
        break;
    case RGBEffectMixingMode::DIVIDE:
        mixPixel = mixPixelDivide;
        break;
    case RGBEffectMixingMode::SCREEN:
        mixPixel = mixPixelScreen;
        break;
    case RGBEffectMixingMode::OVERLAY:
        mixPixel = mixPixelOverlay;
        break;
    case RGBEffectMixingMode::DODGE:
        mixPixel = mixPixelDodge;
        break;
    case RGBEffectMixingMode::BURN:
        mixPixel = mixPixelBurn;
        break;
    case RGBEffectMixingMode::HARDLIGHT:
        mixPixel = mixPixelHardLight;
        break;
    case RGBEffectMixingMode::SOFTLIGHT:
        mixPixel = mixPixelSoftLight;
        break;
    case RGBEffectMixingMode::GRAINEXTRACT:
        mixPixel = mixPixelGrainExtract;
        break;
    case RGBEffectMixingMode::GRAINMERGE:
        mixPixel = mixPixelGrainMerge;
        break;
    case RGBEffectMixingMode::DIFFERENCE:
        mixPixel = mixPixelDifference;
        break;
    }
}

int RGBEffect::loopTime() const
{
    return _desc.loopTime;
}

void RGBEffect::setLoopTime(int loopTime)
{
    _desc.loopTime = std::min(std::max(20, loopTime), 20000000);
}

void RGBEffect::setPosArray(PosArray const& posArray)
{
    _posArray = posArray;
}

bool RGBEffect::refreshPixels(unsigned long currentMillis)
{
    currentMillis -= _startTime;
    _prevUpdateMillis = currentMillis;
//    if (currentMillis - _prevUpdateMillis >= 16) // ~60fps
    {
//        _prevUpdateMillis = (currentMillis / 16) * 16;
        switch (_desc.pattern.pattern)
        {
        default:
        case RGBEffectPattern::PLAIN:
            refreshPixelsPlain();
            break;
        case RGBEffectPattern::STROBE:
            refreshPixelsStrobe();
            break;
        case RGBEffectPattern::ON_OFF:
            refreshPixelsOnOff();
            break;
        case RGBEffectPattern::SMOOTH_ON_OFF:
            refreshPixelsSmoothOnOff();
            break;
        case RGBEffectPattern::SMOOTHER_ON_OFF:
            refreshPixelsSmootherOnOff();
            break;
        case RGBEffectPattern::STRIPE:
            refreshPixelsStripe();
            break;
        case RGBEffectPattern::STRIPE_SMOOTH:
            refreshPixelsStripeSmooth();
            break;
        case RGBEffectPattern::ROTATION:
            refreshPixelsRotation();
            break;
        case RGBEffectPattern::ROTATION_THIN:
            refreshPixelsRotationThin();
            break;
        case RGBEffectPattern::ROTATION_SMOOTH:
            refreshPixelsRotationSmooth();
            break;
        case RGBEffectPattern::ROTATION_SMOOTH_THIN:
            refreshPixelsRotationSmoothThin();
            break;
        case RGBEffectPattern::PLASMA:
            refreshPixelsPlasma();
            break;
        }
        return true;
    }
    //return false;
}

Float RGBEffect::advance() const {
    auto const advFull = Float::scaleUp(_prevUpdateMillis) / _desc.loopTime;
    auto const adv = advFull.floatPart();
    switch (_desc.pattern.direction) {
    case RGBEffectDirection::FORWARD:
    default:
        return adv;
    case RGBEffectDirection::BACKWARD:
        return Float::scaleUp(1.0) - adv;
    case RGBEffectDirection::PING_PONG:
        return std::abs(Float::scaleUp(1) - (advFull / 2).floatPart() * 2);
    }
}

unsigned int RGBEffect::axisSize() const {
    switch (_desc.pattern.axis) {
    case RGBEffectAxis::HORIZONTAL:
    default:
        return _posArray.width;
    case RGBEffectAxis::VERTICAL:
        return _posArray.height;
    case RGBEffectAxis::FRONTAL:
        return _posArray.depth;
    }
}

unsigned int RGBEffect::perpendicularW() const {
    switch (_desc.pattern.axis) {
    case RGBEffectAxis::HORIZONTAL:
    default:
        return _posArray.height;
    case RGBEffectAxis::VERTICAL:
        return _posArray.depth;
    case RGBEffectAxis::FRONTAL:
        return _posArray.width;
    }
}

unsigned int RGBEffect::perpendicularH() const {
    switch (_desc.pattern.axis) {
    case RGBEffectAxis::HORIZONTAL:
    default:
        return _posArray.depth;
    case RGBEffectAxis::VERTICAL:
        return _posArray.width;
    case RGBEffectAxis::FRONTAL:
        return _posArray.height;
    }
}

unsigned int RGBEffect::idxToAxisIdx(unsigned int idx) const {
    switch (_desc.pattern.axis) {
    case RGBEffectAxis::HORIZONTAL:
    default:
        return idx % _posArray.width;
    case RGBEffectAxis::VERTICAL:
        return (idx % (_posArray.width * _posArray.height)) / _posArray.width;
    case RGBEffectAxis::FRONTAL:
        return idx / (_posArray.width * _posArray.height);
    }
}

unsigned int RGBEffect::idxToPerpendicularX(unsigned int idx) const {
    switch (_desc.pattern.axis) {
    case RGBEffectAxis::HORIZONTAL:
    default:
        return (idx % (_posArray.width * _posArray.height)) / _posArray.width;
    case RGBEffectAxis::VERTICAL:
        return idx / (_posArray.width * _posArray.height);
    case RGBEffectAxis::FRONTAL:
        return idx % _posArray.width;
    }
}

unsigned int RGBEffect::idxToPerpendicularY(unsigned int idx) const {
    switch (_desc.pattern.axis) {
    case RGBEffectAxis::HORIZONTAL:
    default:
        return idx / (_posArray.width * _posArray.height);
    case RGBEffectAxis::VERTICAL:
        return idx % _posArray.width;
    case RGBEffectAxis::FRONTAL:
        return (idx % (_posArray.width * _posArray.height)) / _posArray.width;
    }
}

RGBEffect::Color RGBEffect::getTimeGradientColor(Float timeMultiplier) {
    Float const colorLoopTime = Float::scaleUp(_desc.loopTime) * getColor().size * timeMultiplier;
    return getGradientColor((Float::scaleUp(_prevUpdateMillis) % colorLoopTime) / colorLoopTime);
}

RGBEffect::Color RGBEffect::getGradientColor(Float advance)
{
    auto const& colors = getColor();

    unsigned int const sIdx = (unsigned int)(advance * colors.size).scaleDown() % colors.size;
    unsigned int const eIdx = (sIdx + 1) % colors.size;

    int const sr = colors[sIdx][0];
    int const sg = colors[sIdx][1];
    int const sb = colors[sIdx][2];
    int const er = colors[eIdx][0];
    int const eg = colors[eIdx][1];
    int const eb = colors[eIdx][2];

    int const stepR = (er - sr);
    int const stepG = (eg - sg);
    int const stepB = (eb - sb);

    Float const innerAdvance = (advance * colors.size).floatPart();

    int gradR = sr + (stepR * innerAdvance).scaleDown();
    int gradG = sg + (stepG * innerAdvance).scaleDown();
    int gradB = sb + (stepB * innerAdvance).scaleDown();

    return Color{uint8_t(gradR), uint8_t(gradG), uint8_t(gradB)};
}

RGBEffect::Color RGBEffect::dimColor(RGBEffect::Color color) {
    return dimColor(color, _desc.dimmer);
}

RGBEffect::Color RGBEffect::dimColor(RGBEffect::Color color, Float dim) {
    {
        unsigned int c = (color[0] * dim).scaleDown();
        color[0] = std::min(c, 0xffu);
    }
    {
        unsigned int c = (color[1] * dim).scaleDown();
        color[1] = std::min(c, 0xffu);
    }
    {
        unsigned int c = (color[2] * dim).scaleDown();
        color[2] = std::min(c, 0xffu);
    }
    return color;
}

void RGBEffect::beginCurrentCombo()
{
    _startTime = millis();
    switch (_desc.pattern.pattern)
    {
    default:
    case RGBEffectPattern::PLAIN:
        beginPlain();
        break;
    case RGBEffectPattern::STROBE:
        beginStrobe();
        break;
    case RGBEffectPattern::ON_OFF:
        beginOnOff();
        break;
    case RGBEffectPattern::SMOOTH_ON_OFF:
        beginSmoothOnOff();
        break;
    case RGBEffectPattern::SMOOTHER_ON_OFF:
        beginSmootherOnOff();
        break;
    case RGBEffectPattern::STRIPE:
        beginStripe();
        break;
    case RGBEffectPattern::STRIPE_SMOOTH:
        beginStripeSmooth();
        break;
    case RGBEffectPattern::ROTATION:
        beginRotation();
        break;
    case RGBEffectPattern::ROTATION_THIN:
        beginRotationThin();
        break;
    case RGBEffectPattern::ROTATION_SMOOTH:
        beginRotationSmooth();
        break;
    case RGBEffectPattern::ROTATION_SMOOTH_THIN:
        beginRotationSmoothThin();
        break;
    case RGBEffectPattern::PLASMA:
        beginPlasma();
        break;
    }
}

StaticVector<RGBEffect::Color> const& RGBEffect::getColor() const
{
    using ColorArray = StaticVector2<Color, 128>;

    // clang-format off
    static const ColorArray blackColors{
        {0x00, 0x00, 0x00}
    };

    static const ColorArray redColors{
        {0xff, 0x00, 0x00}
    };

    static const ColorArray yellowColors{
        {0xff, 0xff, 0x00}
    };

    static const ColorArray greenColors{
        {0x00, 0xff, 0x00}
    };

    static const ColorArray cyanColors{
        {0x00, 0xff, 0xff}
    };

    static const ColorArray blueColors{
        {0x00, 0x00, 0xff}
    };

    static const ColorArray magentaColors{
        {0xff, 0x00, 0xff}
    };

    static const ColorArray whiteColors{
        {0xff, 0xff, 0xff}
    };

    static const ColorArray flameColors{
        {0xff, 0x00, 0x00},
        {0xff, 0x00, 0x00},
        {0x40, 0x00, 0x80},
        {0xaf, 0x00, 0x00},
        {0xff, 0x00, 0x00},
        {0xdf, 0x50, 0x00},
        {0xfe, 0xee, 0x00},
    };

    static const ColorArray grassColors{
        {0x00, 0xff, 0x00},
        {0x00, 0xff, 0x00},
        {0xc7, 0xee, 0x00},
        {0x00, 0xff, 0x00},
        {0xd3, 0xa1, 0x00}
    };

    static const ColorArray rainbowColors{
        {0xff, 0x00, 0xff},
        {0xff, 0x00, 0xff},
        {0xff, 0xff, 0x00},
        {0x00, 0xff, 0xff},
    };

    static const ColorArray oceanColors{
        {0x00, 0x00, 0xff},
        {0x00, 0x00, 0xff},
        {0x00, 0x3a, 0xb9},
        {0x02, 0xea, 0xff},
        {0x00, 0x3a, 0xb9},
        {0xee, 0xee, 0xfe},
    };

    static const ColorArray goldColors{
        {0xff, 0xd7, 0x00},
        {0xff, 0xd7, 0x00},
        {0xda, 0xa5, 0x20},
        {0xb8, 0x86, 0x0b},
        {0xee, 0xe8, 0xaa},
    };

    static const ColorArray whiteMixedColors{
        {0xff, 0xff, 0xff},
        {0xff, 0xff, 0xff},
        {0xef, 0xeb, 0xdd},
        {0xf5, 0xf5, 0xf5},
        {0xf8, 0xf8, 0xff},
    };

    static const ColorArray pinkColors{
        {0xff, 0x14, 0x93},
        {0xff, 0x14, 0x93},
        {0xff, 0x00, 0xff},
        {0xc7, 0x15, 0x85},
        {0xff, 0x69, 0xb4},
        {0xda, 0x70, 0xd6},
    };

    static const ColorArray etherealColors{
        {0x00, 0x00, 0x00},
        {0xff, 0x55, 0xff},
        {0xaa, 0xbb, 0xbb},
        {0x55, 0x55, 0xff},
        {0x99, 0x99, 0xbb},
    };

    static const ColorArray warmSunsetColors{
        {0xfd, 0x7f, 0x20},
        {0xfc, 0x2e, 0x20},
        {0xfd, 0xb7, 0x50},
        {0x01, 0x01, 0x00},
    };

    static const ColorArray deepBlueColors{
        {0x05, 0x0a, 0x30},
        {0x00, 0x0c, 0x66},
        {0x00, 0x00, 0xff},
        {0x7e, 0xc8, 0xe3},
    };

    static const ColorArray margueritaColors{
        {0x0a, 0x70, 0x29},
        {0xfe, 0xde, 0x00},
        {0xc8, 0xdf, 0x52},
        {0xdb, 0xe8, 0xd8},
    };

    static const ColorArray peachColors{
        {0x19, 0x02, 0x04},
        {0xff, 0x89, 0x76},
        {0xcb, 0x4e, 0x47},
        {0xff, 0xcb, 0xa4},
    };

    static const ColorArray freshColors{
        {0x89, 0xbe, 0x63},
        {0xaf, 0xcc, 0xa9},
        {0xcd, 0xef, 0xee},
        {0xec, 0xf9, 0xf2},
    };

    static const ColorArray violetShadowsColors{
        {0x3d, 0x39, 0x3b},
        {0x7e, 0x1e, 0x80},
        {0x95, 0x05, 0xe3},
        {0xb6, 0x37, 0xfb},
    };

    static const ColorArray darkSideColors{
        {0x10, 0x10, 0x10},
        {0xfb, 0xc1, 0x00},
        {0x00, 0x00, 0x00},
        {0xf9, 0xfa, 0xfc},
        {0x20, 0x20, 0x20},
    };

    // clang-format on

    switch (_desc.color)
    {
    case RGBEffectColor::BLACK:
        return blackColors;
    case RGBEffectColor::RED:
        return redColors;
    case RGBEffectColor::YELLOW:
        return yellowColors;
    case RGBEffectColor::GREEN:
        return greenColors;
    case RGBEffectColor::CYAN:
        return cyanColors;
    case RGBEffectColor::BLUE:
        return blueColors;
    case RGBEffectColor::MAGENTA:
        return magentaColors;
    case RGBEffectColor::WHITE:
        return whiteColors;
    case RGBEffectColor::FLAME:
    default:
        return flameColors;
    case RGBEffectColor::GRASS:
        return grassColors;
    case RGBEffectColor::RAINBOW:
        return rainbowColors;
    case RGBEffectColor::OCEAN:
        return oceanColors;
    case RGBEffectColor::GOLD:
        return goldColors;
    case RGBEffectColor::WHITEMIXED:
        return whiteMixedColors;
    case RGBEffectColor::PINK:
        return pinkColors;
    case RGBEffectColor::ETHEREAL:
        return etherealColors;
    case RGBEffectColor::WARM_SUNSET:
        return warmSunsetColors;
    case RGBEffectColor::DEEP_BLUE:
        return deepBlueColors;
    case RGBEffectColor::MARGUERITA:
        return margueritaColors;
    case RGBEffectColor::PEACH:
        return peachColors;
    case RGBEffectColor::FRESH:
        return freshColors;
    case RGBEffectColor::VIOLET_SHADOWS:
        return violetShadowsColors;
    case RGBEffectColor::DARK_SIDE:
        return darkSideColors;
    }
}

void RGBEffect::beginPlain()
{
}

void RGBEffect::refreshPixelsPlain()
{
    Color rgb = getTimeGradientColor(Float::scaleUp(0.5f));
    for (unsigned int i = 0; i < _pixelCount; ++i) {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        mixPixel(_pixels + i * 3, dimColor(rgb).data());
    }
}

void RGBEffect::beginStrobe()
{
}

void RGBEffect::refreshPixelsStrobe()
{
    Color rgb{0,0,0};
    // 20ms flashes
    if (_prevUpdateMillis % _desc.loopTime < 20) {
        rgb = getTimeGradientColor();
    }
    for (unsigned int i = 0; i < _pixelCount; ++i) {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        mixPixel(_pixels + i * 3, dimColor(rgb).data());
    }
}

void RGBEffect::beginOnOff()
{
}

void RGBEffect::refreshPixelsOnOff()
{
    Color rgb{0,0,0};
    if ((_prevUpdateMillis / (_desc.loopTime / 2)) % 2) {
        rgb = getTimeGradientColor(Float::scaleUp(10));
    }
    for (unsigned int i = 0; i < _pixelCount; ++i) {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        mixPixel(_pixels + i * 3, dimColor(rgb).data());
    }
}

void RGBEffect::beginSmoothOnOff()
{
}

void RGBEffect::refreshPixelsSmoothOnOff()
{
    int colorAdvance = _prevUpdateMillis % _desc.loopTime;

    auto& colors = getColor();
    int colorIdx = (_prevUpdateMillis / _desc.loopTime) % colors.size;
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    red = (red * ((_desc.loopTime / 2) - std::abs((_desc.loopTime / 2) - colorAdvance))) / (_desc.loopTime / 2);
    green = (green * ((_desc.loopTime / 2) - std::abs((_desc.loopTime / 2) - colorAdvance))) / (_desc.loopTime / 2);
    blue = (blue * ((_desc.loopTime / 2) - std::abs((_desc.loopTime / 2) - colorAdvance))) / (_desc.loopTime / 2);
    Color rgb{uint8_t(red), uint8_t(green), uint8_t(blue)};
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        mixPixel(_pixels + i * 3, dimColor(rgb).data());
    }
}

void RGBEffect::beginSmootherOnOff()
{
}

void RGBEffect::refreshPixelsSmootherOnOff()
{
    Float const advance = Utils::sin1(Float::scaleUp(_prevUpdateMillis) / _desc.loopTime);

    auto const color = getTimeGradientColor();
    int red = color[0];
    int green = color[1];
    int blue = color[2];
    red = (red * advance).scaleDown();
    green = (green * advance).scaleDown();
    blue = (blue * advance).scaleDown();
    Color rgb{uint8_t(red), uint8_t(green), uint8_t(blue)};
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        mixPixel(_pixels + i * 3, dimColor(rgb).data());
    }
}

void RGBEffect::beginStripe()
{
}

void RGBEffect::refreshPixelsStripe()
{
    auto const color = getTimeGradientColor();
    int const red = color[0];
    int const green = color[1];
    int const blue = color[2];
    int const position = (advance() * axisSize()).scaleDown();
    int const length = 2;
    Color rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int axisIdx = idxToAxisIdx(_posArray.array[i]);
        int distance = loopedDistance<int>(position, axisIdx, axisSize());
        if (distance < length)
        {
            rgb[0] = (red * (length - distance)) / length;
            rgb[1] = (green * (length - distance)) / length;
            rgb[2] = (blue * (length - distance)) / length;
        }
        else
        {
            std::fill(std::begin(rgb), std::end(rgb), 0);
        }
        mixPixel(_pixels + i * 3, dimColor(rgb).data());
    }
}

void RGBEffect::beginStripeSmooth()
{
}

void RGBEffect::refreshPixelsStripeSmooth()
{
    Float const position = advance() * axisSize();
    auto const color = getTimeGradientColor();
    int const r = color[0];
    int const g = color[1];
    int const b = color[2];

    Float const lengthMin = Float::scaleUp(axisSize()) / 10;
    Float const lengthMax = lengthMin * 2;
    Color rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int const axisIdx = idxToAxisIdx(i);
        Float const distance = loopedDistance<Float>(position, Float::scaleUp(axisIdx),
                                                     Float::scaleUp(axisSize()));
        if (distance < lengthMin)
        {
            rgb = color;
        }
        else if (distance < lengthMax)
        {
            Float const distance10 = (distance - lengthMin) / (lengthMax - lengthMin);
            Float const distance01 = Float::scaleUp(1) - distance10;
            rgb[0] = (r * distance01).scaleDown();
            rgb[1] = (g * distance01).scaleDown();
            rgb[2] = (b * distance01).scaleDown();
        }
        else
        {
            std::fill(std::begin(rgb), std::end(rgb), 0);
        }
        mixPixel(_pixels + i * 3, dimColor(rgb).data());
    }
}

void RGBEffect::beginRotation()
{
}

void RGBEffect::refreshPixelsRotation()
{
    auto const color = getTimeGradientColor();

    Color black{0,0,0};
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int const x = idxToPerpendicularX(_posArray.array[i]);
        int const y = idxToPerpendicularY(_posArray.array[i]);
        Float nx = (Float::scaleUp(x) / perpendicularW()) - Float::scaleUp(0.5);
        Float ny = -(Float::scaleUp(y) / perpendicularH()) + Float::scaleUp(0.5);
        Float sq = std::abs(nx.value) > std::abs(ny.value) ? nx : ny;
        if (sq.value < 0)
            sq.value = -sq.value;
        if (sq.value) {
            nx /= sq;
            ny /= sq;
        }
        Float const nh = Utils::sqrt2((nx * nx) + (ny * ny));
        Float sinus{0};
        if (nh.value)
            sinus = ny / nh;
        Float angle = Utils::asin1((sinus + Float::scaleUp(1)) / 2);
        if (nx.value < 0)
            angle = Float::scaleUp(1) - angle;

        Float totalSinus = Utils::sin1((angle * 2 + Float::scaleUp(_prevUpdateMillis) / _desc.loopTime));

        Color c = (totalSinus > Float::scaleUp(0.5) ? color : black);
        mixPixel(_pixels + i * 3, dimColor(c).data());
    }
}

void RGBEffect::beginRotationThin()
{
}

void RGBEffect::refreshPixelsRotationThin()
{
    auto const color = getTimeGradientColor();

    Color black{0,0,0};
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int const x = idxToPerpendicularX(_posArray.array[i]);
        int const y = idxToPerpendicularY(_posArray.array[i]);
        Float nx = (Float::scaleUp(x) / perpendicularW()) - Float::scaleUp(0.5);
        Float ny = -(Float::scaleUp(y) / perpendicularH()) + Float::scaleUp(0.5);
        Float sq = std::abs(nx.value) > std::abs(ny.value) ? nx : ny;
        if (sq.value < 0)
            sq.value = -sq.value;
        if (sq.value) {
            nx /= sq;
            ny /= sq;
        }
        Float const nh = Utils::sqrt2((nx * nx) + (ny * ny));
        Float sinus{0};
        if (nh.value)
            sinus = ny / nh;
        Float angle = Utils::asin1((sinus + Float::scaleUp(1)) / 2);
        if (nx.value < 0)
            angle = Float::scaleUp(1) - angle;

        Float totalSinus = Utils::sin1((angle * 2 + Float::scaleUp(_prevUpdateMillis) / _desc.loopTime));

        Color c = totalSinus > Float::scaleUp(0.95) ? color : black;
        mixPixel(_pixels + i * 3, dimColor(c).data());
    }
}

void RGBEffect::beginRotationSmooth()
{
}

void RGBEffect::refreshPixelsRotationSmooth()
{
    auto color = getTimeGradientColor();
    int const r = color[0];
    int const g = color[1];
    int const b = color[2];

    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int const x = idxToPerpendicularX(_posArray.array[i]);
        int const y = idxToPerpendicularY(_posArray.array[i]);
        Float nx = (Float::scaleUp(x) / perpendicularW()) - Float::scaleUp(0.5);
        Float ny = -(Float::scaleUp(y) / perpendicularH()) + Float::scaleUp(0.5);
        Float sq = std::abs(nx.value) > std::abs(ny.value) ? nx : ny;
        if (sq.value < 0)
            sq.value = -sq.value;
        if (sq.value) {
            nx /= sq;
            ny /= sq;
        }
        Float const nh = Utils::sqrt2((nx * nx) + (ny * ny));
        Float sinus{0};
        if (nh.value)
            sinus = ny / nh;
        Float angle = Utils::asin1((sinus + Float::scaleUp(1)) / 2);
        if (nx.value < 0)
            angle = Float::scaleUp(1) - angle;

        Float totalSinus = Utils::sin1((angle * 2 + Float::scaleUp(_prevUpdateMillis) / _desc.loopTime));

        color[0] = (r * totalSinus).scaleDown();
        color[1] = (g * totalSinus).scaleDown();
        color[2] = (b * totalSinus).scaleDown();
        mixPixel(_pixels + i * 3, dimColor(color).data());
    }
}

void RGBEffect::beginRotationSmoothThin()
{
}

void RGBEffect::refreshPixelsRotationSmoothThin()
{
    auto color = getTimeGradientColor();
    int const r = color[0];
    int const g = color[1];
    int const b = color[2];

    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int const x = idxToPerpendicularX(_posArray.array[i]);
        int const y = idxToPerpendicularY(_posArray.array[i]);
        Float nx = (Float::scaleUp(x) / perpendicularW()) - Float::scaleUp(0.5);
        Float ny = -(Float::scaleUp(y) / perpendicularH()) + Float::scaleUp(0.5);
        Float sq = std::abs(nx.value) > std::abs(ny.value) ? nx : ny;
        if (sq.value < 0)
            sq.value = -sq.value;
        if (sq.value) {
            nx /= sq;
            ny /= sq;
        }
        Float const nh = Utils::sqrt2((nx * nx) + (ny * ny));
        Float sinus{0};
        if (nh.value)
            sinus = ny / nh;
        Float angle = Utils::asin1((sinus + Float::scaleUp(1)) / 2);
        if (nx.value < 0)
            angle = Float::scaleUp(1) - angle;

        Float totalSinus = Utils::sin1((angle * 2 + Float::scaleUp(_prevUpdateMillis) / _desc.loopTime));
        Float totalSinusSq = totalSinus * totalSinus;

        color[0] = (r * totalSinusSq).scaleDown();
        color[1] = (g * totalSinusSq).scaleDown();
        color[2] = (b * totalSinusSq).scaleDown();
        mixPixel(_pixels + i * 3, dimColor(color).data());
    }
}

void RGBEffect::beginPlasma()
{
}

void RGBEffect::refreshPixelsPlasma()
{
    const int presetSize = 7;

    int size = presetSize / 2;

    Float const plasmaStep = Float::scaleUp(_prevUpdateMillis) / _desc.loopTime;

    std::array<unsigned int, 3> sizes{_posArray.width, _posArray.height, _posArray.depth};
    Float square = Float::scaleUp(*std::max_element(sizes.begin(), sizes.end()));
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int const x = _posArray.array[i] % _posArray.width;
        int const y = (_posArray.array[i] % (_posArray.width * _posArray.height)) / _posArray.width;
        int const z = _posArray.array[i] / (_posArray.width * _posArray.height);
        Float nx = Float::scaleUp(x) / square;
        Float ny = Float::scaleUp(y) / square;
        Float nz = Float::scaleUp(z) / square;
        Float n = SimplexNoise1234::noise(nx * size, ny * size, nz * size, plasmaStep);
        Float n01 = (n + Float::scaleUp(1)) / 2;
        mixPixel(_pixels + i * 3, dimColor(getGradientColor(n01 + plasmaStep / 4)).data());
    }
}

void RGBEffect::mixPixelReplace(uint8_t* pixel, uint8_t const* values)
{
    for (int i = 0; i < 3; ++i)
    {
        pixel[i] = values[i];
    }
}

void RGBEffect::mixPixelMax(uint8_t* pixel, uint8_t const* values)
{
    for (int i = 0; i < 3; ++i)
    {
        if (pixel[i] < values[i])
            pixel[i] = values[i];
    }
}

void RGBEffect::mixPixelMin(uint8_t* pixel, uint8_t const* values)
{
    for (int i = 0; i < 3; ++i)
    {
        if (pixel[i] > values[i])
            pixel[i] = values[i];
    }
}

void RGBEffect::mixPixelAdd(uint8_t* pixel, uint8_t const* values)
{
    for (int i = 0; i < 3; ++i)
    {
        int value = int(pixel[i]) + int(values[i]);
        pixel[i] = std::min(value, 0xff);
    }
}

void RGBEffect::mixPixelSub(uint8_t* pixel, uint8_t const* values)
{
    for (int i = 0; i < 3; ++i)
    {
        int value = int(pixel[i]) - int(values[i]);
        pixel[i] = std::max(value, 0x00);
    }
}

void RGBEffect::mixPixelMultiply(uint8_t* pixel, uint8_t const* values)
{
    for (int i = 0; i < 3; ++i)
    {
        pixel[i] = (int(pixel[i]) * int(values[i])) / int(0xff);
    }
}

void RGBEffect::mixPixelDivide(uint8_t* pixel, uint8_t const* values)
{
    for (int i = 0; i < 3; ++i)
    {
        int I = pixel[i];
        int M = values[i];
        pixel[i] = std::min(std::max(0, (I * 256) / (M + 1)), 255);
    }
}

void RGBEffect::mixPixelScreen(uint8_t* pixel, uint8_t const* values)
{
    for (int i = 0; i < 3; ++i)
    {
        pixel[i] = int(0xff)
            - (
                    (int(0xff) - int(pixel[i]))
                    *
                    (int(0xff) - int(values[i]))
              ) / int(0xff);
    }
}

void RGBEffect::mixPixelOverlay(uint8_t* pixel, uint8_t const* values)
{
    for (int i = 0; i < 3; ++i)
    {
        int I = pixel[i];
        int M = values[i];
        pixel[i] = (I * (I + (2 * M * (255 - I)) / 255)) / 255;
    }
}

void RGBEffect::mixPixelDodge(uint8_t* pixel, uint8_t const* values)
{
    for (int i = 0; i < 3; ++i)
    {
        int I = pixel[i];
        int M = values[i];
        int res = (I * 256) / (256 - M);
        pixel[i] = std::min(std::max(0, res), 255);
    }
}

void RGBEffect::mixPixelBurn(uint8_t* pixel, uint8_t const* values)
{
    for (int i = 0; i < 3; ++i)
    {
        int I = pixel[i];
        int M = values[i];
        int res = 255 - (256 * (255 - I)) / (M + 1);
        pixel[i] = std::min(std::max(0, res), 255);
    }
}

void RGBEffect::mixPixelHardLight(uint8_t* pixel, uint8_t const* values)
{
    for (int i = 0; i < 3; ++i)
    {
        int I = pixel[i];
        int M = values[i];
        if (M > 128)
            pixel[i] = 255 - ((255 - 2 * (M - 128)) * (255 - I)) / 256;
        else
            pixel[i] = (2 * M * I) / 256;
    }
}

void RGBEffect::mixPixelSoftLight(uint8_t* pixel, uint8_t const* values)
{
    uint8_t screen[3] = {pixel[0], pixel[1], pixel[2]};
    mixPixelScreen(screen, values);
    for (int i = 0; i < 3; ++i)
    {
        int I = pixel[i];
        int M = values[i];
        int R = screen[i];
        int res = (((255 - I) * M + R * 255) * I) / (255 * 255);
        pixel[i] = std::min(std::max(0, res), 255);
    }
}

void RGBEffect::mixPixelGrainExtract(uint8_t* pixel, uint8_t const* values)
{
    for (int i = 0; i < 3; ++i)
    {
        int I = pixel[i];
        int M = values[i];
        int res = I - M + 128;
        pixel[i] = std::min(std::max(0, res), 255);
    }
}

void RGBEffect::mixPixelGrainMerge(uint8_t* pixel, uint8_t const* values)
{
    for (int i = 0; i < 3; ++i)
    {
        int I = pixel[i];
        int M = values[i];
        int res = I + M - 128;
        pixel[i] = std::min(std::max(0, res), 255);
    }
}

void RGBEffect::mixPixelDifference(uint8_t* pixel, uint8_t const* values)
{
    for (int i = 0; i < 3; ++i)
    {
        int I = pixel[i];
        int M = values[i];
        pixel[i] = std::abs(I - M);
    }
}
