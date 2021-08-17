#include "RGBEffect.hpp"
#include "simplexnoise1234.h"

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

void RGBEffect::begin(RGBEffectPattern pattern,
                      RGBEffectColor color,
                      RGBEffectMixingMode mixingMode,
                      uint8_t *pixels,
                      unsigned int pixelCount,
                      PosArray const& posArray)
{
    _pattern = pattern;
    _color = color;
    setMixingMode(mixingMode);
    _pixels = pixels;
    _pixelCount = pixelCount;
    _posArray = posArray;

    beginCurrentCombo();
}

RGBEffect::PosArray RGBEffect::posArraySimple(StaticVector<int>& targetBuffer, unsigned int width, unsigned int height)
{
    PosArray posArray;
    posArray.width = width;
    posArray.height = height;
    posArray.array = targetBuffer;
    for (unsigned int i = 0; i < width * height; ++i)
        posArray.array[i] = i;
    return posArray;
}

RGBEffect::PosArray RGBEffect::posArrayZigZag(StaticVector<int>& targetBuffer, unsigned int width, unsigned int height)
{
    PosArray posArray;
    posArray.width = width;
    posArray.height = height;
    posArray.array = targetBuffer;
    for (unsigned int i = 0; i < width * height; ++i)
    {
        if ((i / width) % 2)
            posArray.array[i] = (i / width) * width + ((width - 1) - (i % width));
        else
            posArray.array[i] = i;
    }
    return posArray;
}

RGBEffect::PosArray RGBEffect::posArrayFromLedArray(StaticVector<int>& targetBuffer, unsigned int width, unsigned int height, StaticVector<int> const& ledArray)
{
    PosArray posArray;
    posArray.width = width;
    posArray.height = height;
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

RGBEffect::PosArray RGBEffect::posArrayAirDJ(StaticVector<int>& targetBuffer, unsigned int width, unsigned int height)
{
    PosArray posArray;
    posArray.width = width;
    posArray.height = height;
    posArray.array = targetBuffer;
    for (unsigned int i = 0; i < width * height; ++i)
    {
        if ((i / width) % 2)
            posArray.array[i] = (i / width) * width + ((width - 1) - (i % width));
        else
            posArray.array[i] = i;
    }
    return posArray;
}

void RGBEffect::setPattern(RGBEffectPattern pattern)
{
    _pattern = pattern;
    beginCurrentCombo();
}

void RGBEffect::setColor(RGBEffectColor color)
{
    _color = color;
    beginCurrentCombo();
}

void RGBEffect::setMixingMode(RGBEffectMixingMode mixingMode)
{
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

void RGBEffect::setPosArray(PosArray const& posArray)
{
    _posArray = posArray;
}

int RGBEffect::loopTime() const
{
    return _loopTime;
}

void RGBEffect::setLoopTime(int loopTime)
{
    _loopTime = std::min(std::max(20, loopTime), 20000000);
}

bool RGBEffect::refreshPixels(unsigned long currentMillis)
{
    currentMillis -= _startTime;
    if (currentMillis - _prevUpdateMillis >= 16) // ~60fps
    {
        _prevUpdateMillis = (currentMillis / 16) * 16;
        switch (_pattern)
        {
        default:
        case RGBEffectPattern::SMOOTH_ON_OFF:
            refreshPixelsSmoothOnOff();
            break;
        case RGBEffectPattern::SMOOTHER_ON_OFF:
            refreshPixelsSmootherOnOff();
            break;
        case RGBEffectPattern::STROBE:
            refreshPixelsStrobe();
            break;
        case RGBEffectPattern::STRIPE_H_LEFT_RIGHT:
            refreshPixelsStripeHLeftRight();
            break;
        case RGBEffectPattern::STRIPE_V_UP_DOWN:
            refreshPixelsStripeVUpDown();
            break;
        case RGBEffectPattern::STRIPE_H_RIGHT_LEFT:
            refreshPixelsStripeHRightLeft();
            break;
        case RGBEffectPattern::STRIPE_V_DOWN_UP:
            refreshPixelsStripeVDownUp();
            break;
        case RGBEffectPattern::STRIPE_SMOOTH_H_LEFT_RIGHT:
            refreshPixelsStripeSmoothHLeftRight();
            break;
        case RGBEffectPattern::STRIPE_SMOOTH_V_UP_DOWN:
            refreshPixelsStripeSmoothVUpDown();
            break;
        case RGBEffectPattern::STRIPE_SMOOTH_H_RIGHT_LEFT:
            refreshPixelsStripeSmoothHRightLeft();
            break;
        case RGBEffectPattern::STRIPE_SMOOTH_V_DOWN_UP:
            refreshPixelsStripeSmoothVDownUp();
            break;
        case RGBEffectPattern::PING_PONG_H:
            refreshPixelsPingPongH();
            break;
        case RGBEffectPattern::PING_PONG_V:
            refreshPixelsPingPongV();
            break;
        case RGBEffectPattern::PING_PONG_SMOOTH_H:
            refreshPixelsPingPongSmoothH();
            break;
        case RGBEffectPattern::PING_PONG_SMOOTH_V:
            refreshPixelsPingPongSmoothV();
            break;
        case RGBEffectPattern::ROTATION:
            refreshPixelsRotation();
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
    return false;
}

Float RGBEffect::advance() const {
    return (Float::scaleUp(_prevUpdateMillis) / _loopTime).floatPart();
//    return (Float::scaleUp(_prevUpdateMillis) % Float::scaleUp(_loopTime)) /
//           Float::scaleUp(_loopTime);
}

RGBEffect::Color RGBEffect::getTimeGradientColor(Float timeMultiplier) {
    Float const colorLoopTime = Float::scaleUp(_loopTime) * getColor().size * timeMultiplier;
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

void RGBEffect::beginCurrentCombo()
{
    _startTime = millis();
    switch (_pattern)
    {
    default:
    case RGBEffectPattern::SMOOTH_ON_OFF:
        beginSmoothOnOff();
        break;
    case RGBEffectPattern::SMOOTHER_ON_OFF:
        beginSmootherOnOff();
        break;
    case RGBEffectPattern::STROBE:
        beginStrobe();
        break;
    case RGBEffectPattern::STRIPE_H_LEFT_RIGHT:
        beginStripeHLeftRight();
        break;
    case RGBEffectPattern::STRIPE_V_UP_DOWN:
        beginStripeVUpDown();
        break;
    case RGBEffectPattern::STRIPE_H_RIGHT_LEFT:
        beginStripeHRightLeft();
        break;
    case RGBEffectPattern::STRIPE_V_DOWN_UP:
        beginStripeVDownUp();
        break;
    case RGBEffectPattern::STRIPE_SMOOTH_H_LEFT_RIGHT:
        beginStripeSmoothHLeftRight();
        break;
    case RGBEffectPattern::STRIPE_SMOOTH_V_UP_DOWN:
        beginStripeSmoothVUpDown();
        break;
    case RGBEffectPattern::STRIPE_SMOOTH_H_RIGHT_LEFT:
        beginStripeSmoothHRightLeft();
        break;
    case RGBEffectPattern::STRIPE_SMOOTH_V_DOWN_UP:
        beginStripeSmoothVDownUp();
        break;
    case RGBEffectPattern::PING_PONG_H:
        beginPingPongH();
        break;
    case RGBEffectPattern::PING_PONG_V:
        beginPingPongV();
        break;
    case RGBEffectPattern::PING_PONG_SMOOTH_H:
        beginPingPongSmoothH();
        break;
    case RGBEffectPattern::PING_PONG_SMOOTH_V:
        beginPingPongSmoothV();
        break;
    case RGBEffectPattern::ROTATION:
        beginRotation();
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

    static const ColorArray whiteColors{
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
    // clang-format on

    switch (_color)
    {
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
    case RGBEffectColor::WHITE:
        return whiteColors;
    case RGBEffectColor::PINK:
        return pinkColors;
    }
}

void RGBEffect::beginSmoothOnOff()
{
}

void RGBEffect::refreshPixelsSmoothOnOff()
{
    int colorAdvance = _prevUpdateMillis % _loopTime;

    auto& colors = getColor();
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size;
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    red = (red * ((_loopTime / 2) - std::abs((_loopTime / 2) - colorAdvance))) / (_loopTime / 2);
    green = (green * ((_loopTime / 2) - std::abs((_loopTime / 2) - colorAdvance))) / (_loopTime / 2);
    blue = (blue * ((_loopTime / 2) - std::abs((_loopTime / 2) - colorAdvance))) / (_loopTime / 2);
    Color rgb{uint8_t(red), uint8_t(green), uint8_t(blue)};
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginSmootherOnOff()
{
}

void RGBEffect::refreshPixelsSmootherOnOff()
{
    Float const advance = Utils::sin1(Float::scaleUp(_prevUpdateMillis) / _loopTime);

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
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginStrobe()
{
}

void RGBEffect::refreshPixelsStrobe()
{
    Color rgb{0,0,0};
    if ((_prevUpdateMillis / (_loopTime / 2)) % 2) {
        rgb = getTimeGradientColor(Float::scaleUp(10));
    }
    for (unsigned int i = 0; i < _pixelCount; ++i) {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginStripeHLeftRight()
{
}

void RGBEffect::refreshPixelsStripeHLeftRight()
{
    int colorAdvance = _prevUpdateMillis % _loopTime;

    auto& colors = getColor();
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size;
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    int position = (_posArray.width * colorAdvance) / _loopTime;
    int length = 2;
    Color rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int x = _posArray.array[i] % _posArray.width;
        int distance = loopedDistance<int>(position, x, _posArray.width);
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
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginStripeVUpDown()
{
}

void RGBEffect::refreshPixelsStripeVUpDown()
{
    int colorAdvance = _prevUpdateMillis % _loopTime;

    auto& colors = getColor();
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size;
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    int position = (_posArray.height * colorAdvance) / _loopTime;
    int length = 2;
    Color rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int y = _posArray.array[i] / _posArray.width;
        int distance = loopedDistance<int>(position, y, _posArray.height);
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
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginStripeHRightLeft()
{
}

void RGBEffect::refreshPixelsStripeHRightLeft()
{
    int colorAdvance = _prevUpdateMillis % _loopTime;

    auto& colors = getColor();
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size;
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    int position = (_posArray.width - 1) - ((_posArray.width * colorAdvance) / _loopTime);
    int length = 2;
    Color rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int x = _posArray.array[i] % _posArray.width;
        int distance = std::abs(position - (int)x);
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
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginStripeVDownUp()
{
}

void RGBEffect::refreshPixelsStripeVDownUp()
{
    int colorAdvance = _prevUpdateMillis % _loopTime;

    auto& colors = getColor();
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size;
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    int position = (_posArray.height - 1) - ((_posArray.height * colorAdvance) / _loopTime);
    int length = 2;
    Color rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int y = _posArray.array[i] / _posArray.width;
        int distance = std::abs(position - (int)y);
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
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginStripeSmoothHLeftRight()
{
}

void RGBEffect::refreshPixelsStripeSmoothHLeftRight()
{
    Float const position = advance() * _posArray.width;
    auto const color = getTimeGradientColor();
    int const r = color[0];
    int const g = color[1];
    int const b = color[2];

    Float const lengthMin = Float::scaleUp(_posArray.width) / 10;
    Float const lengthMax = lengthMin * 2;
    Color rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int const x = _posArray.array[i] % _posArray.width;
        Float const distance = loopedDistance<Float>(position, Float::scaleUp(x),
                                                     Float::scaleUp(_posArray.width));
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
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginStripeSmoothVUpDown()
{
}

void RGBEffect::refreshPixelsStripeSmoothVUpDown()
{
    Float const position = advance() * _posArray.height;
    auto const color = getTimeGradientColor();
    int const r = color[0];
    int const g = color[1];
    int const b = color[2];

    Float const lengthMin = Float::scaleUp(_posArray.height) / 10;
    Float const lengthMax = lengthMin * 2;
    Color rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int const y = _posArray.array[i] / _posArray.width;
        Float const distance = loopedDistance<Float>(position, Float::scaleUp(y),
                                                     Float::scaleUp(_posArray.height));
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
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginStripeSmoothHRightLeft()
{
}

void RGBEffect::refreshPixelsStripeSmoothHRightLeft()
{
    Float const position = (Float::scaleUp(1.0) - advance()) * _posArray.width;
    auto const color = getTimeGradientColor();
    int const r = color[0];
    int const g = color[1];
    int const b = color[2];

    Float const lengthMin = Float::scaleUp(_posArray.width) / 10;
    Float const lengthMax = lengthMin * 2;
    Color rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int const x = _posArray.array[i] % _posArray.width;
        Float const distance = loopedDistance<Float>(position, Float::scaleUp(x),
                                                     Float::scaleUp(_posArray.width));
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
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginStripeSmoothVDownUp()
{
}

void RGBEffect::refreshPixelsStripeSmoothVDownUp()
{
    Float const position = (Float::scaleUp(1.0) - advance()) * _posArray.height;
    auto const color = getTimeGradientColor();
    int const r = color[0];
    int const g = color[1];
    int const b = color[2];

    Float const lengthMin = Float::scaleUp(_posArray.height) / 10;
    Float const lengthMax = lengthMin * 2;
    Color rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int const y = _posArray.array[i] / _posArray.width;
        Float const distance = loopedDistance<Float>(position, Float::scaleUp(y),
                                                     Float::scaleUp(_posArray.height));
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
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginPingPongH()
{
}

void RGBEffect::refreshPixelsPingPongH()
{
    int colorAdvance = _prevUpdateMillis % _loopTime;

    auto& colors = getColor();
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size;
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    int position = (_posArray.width * ((_loopTime / 2) - std::abs((_loopTime / 2) - colorAdvance))) / (_loopTime / 2);
    int length = 2;
    Color rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int x = _posArray.array[i] % _posArray.width;
        int distance = std::abs(position - (int)x);
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
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginPingPongV()
{
}

void RGBEffect::refreshPixelsPingPongV()
{
    int colorAdvance = _prevUpdateMillis % _loopTime;

    auto& colors = getColor();
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size;
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    int position = (_posArray.height * ((_loopTime / 2) - std::abs((_loopTime / 2) - colorAdvance))) / (_loopTime / 2);
    int length = 2;
    Color rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int y = _posArray.array[i] / _posArray.width;
        int distance = std::abs(position - (int)y);
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
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginPingPongSmoothH()
{
}

void RGBEffect::refreshPixelsPingPongSmoothH()
{
    Float const position = std::abs(advance() * 2 - Float::scaleUp(1)) * _posArray.width;
    auto const color = getTimeGradientColor();
    int const r = color[0];
    int const g = color[1];
    int const b = color[2];

    Float const lengthMin = Float::scaleUp(_posArray.width) / 10;
    Float const lengthMax = lengthMin * 2;
    Color rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int const x = _posArray.array[i] % _posArray.width;
        Float const distance = std::abs(position - Float::scaleUp(x));
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
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginPingPongSmoothV()
{
}

void RGBEffect::refreshPixelsPingPongSmoothV()
{
    Float const position = (advance() * 2 - Float::scaleUp(1)) * _posArray.height;
    auto const color = getTimeGradientColor();
    int const r = color[0];
    int const g = color[1];
    int const b = color[2];

    Float const lengthMin = Float::scaleUp(_posArray.height) / 10;
    Float const lengthMax = lengthMin * 2;
    Color rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int const y = _posArray.array[i] / _posArray.width;
        Float const distance = std::abs(position - Float::scaleUp(y));
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
        mixPixel(_pixels + i * 3, rgb.data());
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
        int const x = _posArray.array[i] % _posArray.width;
        int const y = _posArray.array[i] / _posArray.width;
        Float nx = (Float::scaleUp(x) / _posArray.width) - Float::scaleUp(0.5);
        Float ny = -(Float::scaleUp(y) / _posArray.height) + Float::scaleUp(0.5);
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

        Float totalSinus = Utils::sin1((angle * 2 + Float::scaleUp(_prevUpdateMillis) / _loopTime));

        mixPixel(_pixels + i * 3, (totalSinus > Float::scaleUp(0.5) ? color.data() : black.data()));
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
        int const x = _posArray.array[i] % _posArray.width;
        int const y = _posArray.array[i] / _posArray.width;
        Float nx = (Float::scaleUp(x) / _posArray.width) - Float::scaleUp(0.5);
        Float ny = -(Float::scaleUp(y) / _posArray.height) + Float::scaleUp(0.5);
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

        Float totalSinus = Utils::sin1((angle * 2 + Float::scaleUp(_prevUpdateMillis) / _loopTime));

        color[0] = (r * totalSinus).scaleDown();
        color[1] = (g * totalSinus).scaleDown();
        color[2] = (b * totalSinus).scaleDown();
        mixPixel(_pixels + i * 3, color.data());
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
        int const x = _posArray.array[i] % _posArray.width;
        int const y = _posArray.array[i] / _posArray.width;
        Float nx = (Float::scaleUp(x) / _posArray.width) - Float::scaleUp(0.5);
        Float ny = -(Float::scaleUp(y) / _posArray.height) + Float::scaleUp(0.5);
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

        Float totalSinus = Utils::sin1((angle * 2 + Float::scaleUp(_prevUpdateMillis) / _loopTime));
        Float totalSinusSq = totalSinus * totalSinus;

        color[0] = (r * totalSinusSq).scaleDown();
        color[1] = (g * totalSinusSq).scaleDown();
        color[2] = (b * totalSinusSq).scaleDown();
        mixPixel(_pixels + i * 3, color.data());
    }
}

void RGBEffect::beginPlasma()
{
}

void RGBEffect::refreshPixelsPlasma()
{
    const int presetSize = 7;

    int size = presetSize / 2;

    Float const plasmaStep = Float::scaleUp(_prevUpdateMillis) / _loopTime;

    Float square = Float::scaleUp(_posArray.width > _posArray.height ? _posArray.width : _posArray.height);
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        if (_posArray.array[i] == -1) {
            std::memset(_pixels + i * 3, 0, 3);
            continue;
        }
        int x = _posArray.array[i] % _posArray.width;
        int y = _posArray.array[i] / _posArray.width;
        Float nx = Float::scaleUp(x) / square;
        Float ny = Float::scaleUp(y) / square;
        Float n = SimplexNoise1234::noise(nx * size, ny * size, plasmaStep);
        Float n01 = (n + Float::scaleUp(1)) / 2;
        mixPixel(_pixels + i * 3, getGradientColor(n01 + plasmaStep / 4).data());
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
