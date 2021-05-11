#include "RGBEffect.hpp"
#include "Perlin.hpp"

#include <algorithm>
#include <cmath>
#include <iterator>

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

    if (!posArray.array.empty())
        _posArray = posArray;
    else
        _posArray = posArraySimple(pixelCount, 1);

    beginCurrentCombo();
}

RGBEffect::PosArray RGBEffect::posArraySimple(unsigned int width, unsigned int height)
{
    PosArray posArray;
    posArray.width = width;
    posArray.height = height;
    for (unsigned int i = 0; i < width * height; ++i)
        posArray.array.emplace_back(i);
    return posArray;
}

RGBEffect::PosArray RGBEffect::posArrayZigZag(unsigned int width, unsigned int height)
{
    PosArray posArray;
    posArray.width = width;
    posArray.height = height;
    for (unsigned int i = 0; i < width * height; ++i)
    {
        if ((i / width) % 2)
            posArray.array.emplace_back((i / width) * width + ((width - 1) - (i % width)));
        else
            posArray.array.emplace_back(i);
    }
    return posArray;
}

RGBEffect::PosArray RGBEffect::posArrayFromLedArray(std::vector<int> const& ledArray, unsigned int width, unsigned int height)
{
    PosArray posArray;
    posArray.width = width;
    posArray.height = height;

    int ledMax = 0;
    for (auto led : ledArray)
    {
        if (led > ledMax)
            ledMax = led;
    }
    const int ledCount = ledMax + 1;

    for (int i = 0; i < ledCount; ++i)
    {
        auto ledArrayIt = std::find(std::begin(ledArray), std::end(ledArray), i);
        if (ledArrayIt != std::end(ledArray))
        {
            posArray.array.emplace_back(ledArrayIt - std::begin(ledArray));
        }
        else
        {
            posArray.array.emplace_back(0);
        }
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
    if (currentMillis - _prevUpdateMillis >= 20) // 50fps
    {
        _prevUpdateMillis = (currentMillis / 20) * 20;
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
        case RGBEffectPattern::STRIPE:
            refreshPixelsStripe();
            break;
        case RGBEffectPattern::STRIPE_H_LEFT_RIGHT:
            refreshPixelsStripeHLeftRight();
            break;
        case RGBEffectPattern::STRIPE_V_UP_DOWN:
            refreshPixelsStripeVUpDown();
            break;
        case RGBEffectPattern::STRIPE_REV:
            refreshPixelsStripeRev();
            break;
        case RGBEffectPattern::STRIPE_H_RIGHT_LEFT:
            refreshPixelsStripeHRightLeft();
            break;
        case RGBEffectPattern::STRIPE_V_DOWN_UP:
            refreshPixelsStripeVDownUp();
            break;
        case RGBEffectPattern::STRIPE_SMOOTH:
            refreshPixelsStripeSmooth();
            break;
        case RGBEffectPattern::STRIPE_SMOOTH_H_LEFT_RIGHT:
            refreshPixelsStripeSmoothHLeftRight();
            break;
        case RGBEffectPattern::STRIPE_SMOOTH_V_UP_DOWN:
            refreshPixelsStripeSmoothVUpDown();
            break;
        case RGBEffectPattern::STRIPE_SMOOTH_REV:
            refreshPixelsStripeSmoothRev();
            break;
        case RGBEffectPattern::STRIPE_SMOOTH_H_RIGHT_LEFT:
            refreshPixelsStripeSmoothHRightLeft();
            break;
        case RGBEffectPattern::STRIPE_SMOOTH_V_DOWN_UP:
            refreshPixelsStripeSmoothVDownUp();
            break;
        case RGBEffectPattern::PING_PONG:
            refreshPixelsPingPong();
            break;
        case RGBEffectPattern::PING_PONG_H:
            refreshPixelsPingPongH();
            break;
        case RGBEffectPattern::PING_PONG_V:
            refreshPixelsPingPongV();
            break;
        case RGBEffectPattern::PING_PONG_SMOOTH:
            refreshPixelsPingPongSmooth();
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

std::array<uint8_t, 3> RGBEffect::getGradientColor(double advance)
{
    auto const& colors = getColor();

    unsigned int const sIdx = (unsigned int)(advance * colors.size()) % colors.size();
    unsigned int const eIdx = (sIdx + 1) % colors.size();

    int const sr = colors[sIdx][0];
    int const sg = colors[sIdx][1];
    int const sb = colors[sIdx][2];
    int const er = colors[eIdx][0];
    int const eg = colors[eIdx][1];
    int const eb = colors[eIdx][2];

    int const stepR = (er - sr);
    int const stepG = (eg - sg);
    int const stepB = (eb - sb);

    double const innerAdvance = (advance * colors.size()) - (int)(advance * colors.size());

    int gradR = sr + (stepR * innerAdvance);
    int gradG = sg + (stepG * innerAdvance);
    int gradB = sb + (stepB * innerAdvance);

    return std::array<uint8_t, 3>{uint8_t(gradR), uint8_t(gradG), uint8_t(gradB)};
}

void RGBEffect::beginCurrentCombo()
{
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
    case RGBEffectPattern::STRIPE:
        beginStripe();
        break;
    case RGBEffectPattern::STRIPE_H_LEFT_RIGHT:
        beginStripeHLeftRight();
        break;
    case RGBEffectPattern::STRIPE_V_UP_DOWN:
        beginStripeVUpDown();
        break;
    case RGBEffectPattern::STRIPE_REV:
        beginStripeRev();
        break;
    case RGBEffectPattern::STRIPE_H_RIGHT_LEFT:
        beginStripeHRightLeft();
        break;
    case RGBEffectPattern::STRIPE_V_DOWN_UP:
        beginStripeVDownUp();
        break;
    case RGBEffectPattern::STRIPE_SMOOTH:
        beginStripeSmooth();
        break;
    case RGBEffectPattern::STRIPE_SMOOTH_H_LEFT_RIGHT:
        beginStripeSmoothHLeftRight();
        break;
    case RGBEffectPattern::STRIPE_SMOOTH_V_UP_DOWN:
        beginStripeSmoothVUpDown();
        break;
    case RGBEffectPattern::STRIPE_SMOOTH_REV:
        beginStripeSmoothRev();
        break;
    case RGBEffectPattern::STRIPE_SMOOTH_H_RIGHT_LEFT:
        beginStripeSmoothHRightLeft();
        break;
    case RGBEffectPattern::STRIPE_SMOOTH_V_DOWN_UP:
        beginStripeSmoothVDownUp();
        break;
    case RGBEffectPattern::PING_PONG:
        beginPingPong();
        break;
    case RGBEffectPattern::PING_PONG_H:
        beginPingPongH();
        break;
    case RGBEffectPattern::PING_PONG_V:
        beginPingPongV();
        break;
    case RGBEffectPattern::PING_PONG_SMOOTH:
        beginPingPongSmooth();
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

std::vector<std::array<uint8_t, 3>> const& RGBEffect::getColor() const
{
    static const std::vector<std::array<uint8_t, 3>> flameColors = {
        {0xff, 0x00, 0x00},
        {0xff, 0x00, 0x00},
        {0x40, 0x00, 0x80},
        {0xaf, 0x00, 0x00},
        {0xff, 0x00, 0x00},
        {0xdf, 0x50, 0x00},
        {0xfe, 0xee, 0x00},
    };
    static const std::vector<std::array<uint8_t, 3>> grassColors = {
        {0x00, 0xff, 0x00},
        {0x00, 0xff, 0x00},
        {0xc7, 0xee, 0x00},
        {0x00, 0xff, 0x00},
        {0xd3, 0xa1, 0x00},
    };
    static const std::vector<std::array<uint8_t, 3>> rainbowColors = {
        {0xff, 0x00, 0xff},
        {0xff, 0x00, 0xff},
        {0xff, 0xff, 0x00},
        {0x00, 0xff, 0xff},
    };
    static const std::vector<std::array<uint8_t, 3>> oceanColors = {
        {0x00, 0x00, 0xff},
        {0x00, 0x00, 0xff},
        {0x00, 0x3a, 0xb9},
        {0x02, 0xea, 0xff},
        {0x00, 0x3a, 0xb9},
        {0xee, 0xee, 0xfe},
    };
    static const std::vector<std::array<uint8_t, 3>> goldColors = {
        {0xff, 0xd7, 0x00},
        {0xff, 0xd7, 0x00},
        {0xda, 0xa5, 0x20},
        {0xb8, 0x86, 0x0b},
        {0xee, 0xe8, 0xaa},
    };
    static const std::vector<std::array<uint8_t, 3>> whiteColors = {
        {0xff, 0xff, 0xff},
        {0xff, 0xff, 0xff},
        {0xef, 0xeb, 0xdd},
        {0xf5, 0xf5, 0xf5},
        {0xf8, 0xf8, 0xff},
    };
    static const std::vector<std::array<uint8_t, 3>> pinkColors = {
        {0xff, 0x14, 0x93},
        {0xff, 0x14, 0x93},
        {0xff, 0x00, 0xff},
        {0xc7, 0x15, 0x85},
        {0xff, 0x69, 0xb4},
        {0xda, 0x70, 0xd6},
    };

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
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size();
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    red = (red * ((_loopTime / 2) - std::abs((_loopTime / 2) - colorAdvance))) / (_loopTime / 2);
    green = (green * ((_loopTime / 2) - std::abs((_loopTime / 2) - colorAdvance))) / (_loopTime / 2);
    blue = (blue * ((_loopTime / 2) - std::abs((_loopTime / 2) - colorAdvance))) / (_loopTime / 2);
    std::array<uint8_t, 3> rgb{uint8_t(red), uint8_t(green), uint8_t(blue)};
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginSmootherOnOff()
{
}

void RGBEffect::refreshPixelsSmootherOnOff()
{
    double const colorAdvance =  double(_prevUpdateMillis % (_loopTime * getColor().size())) / double(_loopTime * getColor().size());
    double const advance = (1.0 + std::sin(double(_prevUpdateMillis * M_PI * 2.0) / double(_loopTime))) / 2.0;

    auto const color = getGradientColor(colorAdvance);
    int red = color[0];
    int green = color[1];
    int blue = color[2];
    red = red * advance;
    green = green * advance;
    blue = blue * advance;
    std::array<uint8_t, 3> rgb{uint8_t(red), uint8_t(green), uint8_t(blue)};
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginStrobe()
{
}

void RGBEffect::refreshPixelsStrobe()
{
    std::array<uint8_t, 3> rgb;
    if ((_prevUpdateMillis / (_loopTime / 2)) % 2)
    {
        int const colorLoopTime = _loopTime * getColor().size() * 10;
        rgb = getGradientColor(double(_prevUpdateMillis % colorLoopTime) / double(colorLoopTime));
    }
    else
    {
        rgb = std::array<uint8_t, 3>{0x00, 0x00, 0x00};
    }
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginStripe()
{
}

void RGBEffect::refreshPixelsStripe()
{
    int colorAdvance = _prevUpdateMillis % _loopTime;

    auto& colors = getColor();
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size();
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    int position = (_pixelCount * colorAdvance) / _loopTime;
    int length = 4;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        int distance = loopedDistance<int>(position, i, _pixelCount);
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

void RGBEffect::beginStripeHLeftRight()
{
}

void RGBEffect::refreshPixelsStripeHLeftRight()
{
    int colorAdvance = _prevUpdateMillis % _loopTime;

    auto& colors = getColor();
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size();
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    int position = (_posArray.width * colorAdvance) / _loopTime;
    int length = 2;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
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
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size();
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    int position = (_posArray.height * colorAdvance) / _loopTime;
    int length = 2;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
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

void RGBEffect::beginStripeRev()
{
}

void RGBEffect::refreshPixelsStripeRev()
{
    int colorAdvance = _prevUpdateMillis % _loopTime;

    auto& colors = getColor();
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size();
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    int position = (_pixelCount - 1) - ((_pixelCount * colorAdvance) / _loopTime);
    int length = 4;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        int distance = std::abs(position - (int)i);
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
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size();
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    int position = (_posArray.width - 1) - ((_posArray.width * colorAdvance) / _loopTime);
    int length = 2;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
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
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size();
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    int position = (_posArray.height - 1) - ((_posArray.height * colorAdvance) / _loopTime);
    int length = 2;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
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

void RGBEffect::beginStripeSmooth()
{
}

void RGBEffect::refreshPixelsStripeSmooth()
{
    double const colorAdvance =  double(_prevUpdateMillis % (_loopTime * getColor().size())) / double(_loopTime * getColor().size());
    double const advance = double(_prevUpdateMillis % _loopTime) / double(_loopTime);
    double const position = advance * _pixelCount;
    auto const color = getGradientColor(colorAdvance);
    int const red = color[0];
    int const green = color[1];
    int const blue = color[2];

    double const lengthMin = _pixelCount / 10.0;
    double const lengthMax = lengthMin * 2;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        double const distance = loopedDistance<double>(position, i, _pixelCount);
        if (distance < lengthMin)
        {
            rgb[0] = red;
            rgb[1] = green;
            rgb[2] = blue;
        }
        else if (distance < lengthMax)
        {
            double const distance10 = (distance - lengthMin) / (lengthMax - lengthMin);
            double const distance01 = 1 - distance10;
            rgb[0] = red * distance01;
            rgb[1] = green * distance01;
            rgb[2] = blue * distance01;
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
    double const colorAdvance =  double(_prevUpdateMillis % (_loopTime * getColor().size())) / double(_loopTime * getColor().size());
    double const advance = double(_prevUpdateMillis % _loopTime) / double(_loopTime);
    double const position = advance * _posArray.width;
    auto const color = getGradientColor(colorAdvance);
    int const red = color[0];
    int const green = color[1];
    int const blue = color[2];

    double const lengthMin = _posArray.width / 10.0;
    double const lengthMax = lengthMin * 2;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        int const x = _posArray.array[i] % _posArray.width;
        double const distance = loopedDistance<double>(position, x, _posArray.width);
        if (distance < lengthMin)
        {
            rgb[0] = red;
            rgb[1] = green;
            rgb[2] = blue;
        }
        else if (distance < lengthMax)
        {
            double const distance10 = (distance - lengthMin) / (lengthMax - lengthMin);
            double const distance01 = 1 - distance10;
            rgb[0] = red * distance01;
            rgb[1] = green * distance01;
            rgb[2] = blue * distance01;
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
    double const colorAdvance =  double(_prevUpdateMillis % (_loopTime * getColor().size())) / double(_loopTime * getColor().size());
    double const advance = double(_prevUpdateMillis % _loopTime) / double(_loopTime);
    double const position = advance * _posArray.height;
    auto const color = getGradientColor(colorAdvance);
    int const red = color[0];
    int const green = color[1];
    int const blue = color[2];

    double const lengthMin = _posArray.height / 10.0;
    double const lengthMax = lengthMin * 2;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        int const y = _posArray.array[i] / _posArray.width;
        double const distance = loopedDistance<double>(position, y, _posArray.height);
        if (distance < lengthMin)
        {
            rgb[0] = red;
            rgb[1] = green;
            rgb[2] = blue;
        }
        else if (distance < lengthMax)
        {
            double const distance10 = (distance - lengthMin) / (lengthMax - lengthMin);
            double const distance01 = 1 - distance10;
            rgb[0] = red * distance01;
            rgb[1] = green * distance01;
            rgb[2] = blue * distance01;
        }
        else
        {
            std::fill(std::begin(rgb), std::end(rgb), 0);
        }
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginStripeSmoothRev()
{
}

void RGBEffect::refreshPixelsStripeSmoothRev()
{
    double const colorAdvance =  double(_prevUpdateMillis % (_loopTime * getColor().size())) / double(_loopTime * getColor().size());
    double const advance = double(_prevUpdateMillis % _loopTime) / double(_loopTime);
    double const position = 1.0 - (advance * _pixelCount);
    auto const color = getGradientColor(colorAdvance);
    int const red = color[0];
    int const green = color[1];
    int const blue = color[2];

    double const lengthMin = _pixelCount / 10.0;
    double const lengthMax = lengthMin * 2;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        double const distance = loopedDistance<double>(position, i, _pixelCount);
        if (distance < lengthMin)
        {
            rgb[0] = red;
            rgb[1] = green;
            rgb[2] = blue;
        }
        else if (distance < lengthMax)
        {
            double const distance10 = (distance - lengthMin) / (lengthMax - lengthMin);
            double const distance01 = 1 - distance10;
            rgb[0] = red * distance01;
            rgb[1] = green * distance01;
            rgb[2] = blue * distance01;
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
    double const colorAdvance =  double(_prevUpdateMillis % (_loopTime * getColor().size())) / double(_loopTime * getColor().size());
    double const advance = double(_prevUpdateMillis % _loopTime) / double(_loopTime);
    double const position = 1.0 - (advance * _posArray.width);
    auto const color = getGradientColor(colorAdvance);
    int const red = color[0];
    int const green = color[1];
    int const blue = color[2];

    double const lengthMin = _posArray.width / 10.0;
    double const lengthMax = lengthMin * 2;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        int const x = _posArray.array[i] % _posArray.width;
        double const distance = loopedDistance<double>(position, x, _posArray.width);
        if (distance < lengthMin)
        {
            rgb[0] = red;
            rgb[1] = green;
            rgb[2] = blue;
        }
        else if (distance < lengthMax)
        {
            double const distance10 = (distance - lengthMin) / (lengthMax - lengthMin);
            double const distance01 = 1 - distance10;
            rgb[0] = red * distance01;
            rgb[1] = green * distance01;
            rgb[2] = blue * distance01;
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
    double const colorAdvance =  double(_prevUpdateMillis % (_loopTime * getColor().size())) / double(_loopTime * getColor().size());
    double const advance = double(_prevUpdateMillis % _loopTime) / double(_loopTime);
    double const position = 1.0 - (advance * _posArray.height);
    auto const color = getGradientColor(colorAdvance);
    int const red = color[0];
    int const green = color[1];
    int const blue = color[2];

    double const lengthMin = _posArray.height / 10.0;
    double const lengthMax = lengthMin * 2;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        int const y = _posArray.array[i] / _posArray.width;
        double const distance = loopedDistance<double>(position, y, _posArray.height);
        if (distance < lengthMin)
        {
            rgb[0] = red;
            rgb[1] = green;
            rgb[2] = blue;
        }
        else if (distance < lengthMax)
        {
            double const distance10 = (distance - lengthMin) / (lengthMax - lengthMin);
            double const distance01 = 1 - distance10;
            rgb[0] = red * distance01;
            rgb[1] = green * distance01;
            rgb[2] = blue * distance01;
        }
        else
        {
            std::fill(std::begin(rgb), std::end(rgb), 0);
        }
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginPingPong()
{
}

void RGBEffect::refreshPixelsPingPong()
{
    int colorAdvance = _prevUpdateMillis % _loopTime;

    auto& colors = getColor();
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size();
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    int position = (_pixelCount * ((_loopTime / 2) - std::abs((_loopTime / 2) - colorAdvance))) / (_loopTime / 2);
    int length = 4;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        int distance = std::abs(position - (int)i);
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

void RGBEffect::beginPingPongH()
{
}

void RGBEffect::refreshPixelsPingPongH()
{
    int colorAdvance = _prevUpdateMillis % _loopTime;

    auto& colors = getColor();
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size();
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    int position = (_posArray.width * ((_loopTime / 2) - std::abs((_loopTime / 2) - colorAdvance))) / (_loopTime / 2);
    int length = 2;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
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
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size();
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];
    int position = (_posArray.height * ((_loopTime / 2) - std::abs((_loopTime / 2) - colorAdvance))) / (_loopTime / 2);
    int length = 2;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
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

void RGBEffect::beginPingPongSmooth()
{
}

void RGBEffect::refreshPixelsPingPongSmooth()
{
    double const colorAdvance =  double(_prevUpdateMillis % (_loopTime * getColor().size())) / double(_loopTime * getColor().size());
    double const advance = double(_prevUpdateMillis % _loopTime) / double(_loopTime);
    double const position = std::abs(advance * 2 - 1) * _pixelCount;
    auto const color = getGradientColor(colorAdvance);
    int const red = color[0];
    int const green = color[1];
    int const blue = color[2];

    double const lengthMin = _pixelCount / 10.0;
    double const lengthMax = lengthMin * 2;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        double const distance = std::abs(position - i);
        if (distance < lengthMin)
        {
            rgb[0] = red;
            rgb[1] = green;
            rgb[2] = blue;
        }
        else if (distance < lengthMax)
        {
            double const distance10 = (distance - lengthMin) / (lengthMax - lengthMin);
            double const distance01 = 1 - distance10;
            rgb[0] = red * distance01;
            rgb[1] = green * distance01;
            rgb[2] = blue * distance01;
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
    double const colorAdvance =  double(_prevUpdateMillis % (_loopTime * getColor().size())) / double(_loopTime * getColor().size());
    double const advance = double(_prevUpdateMillis % _loopTime) / double(_loopTime);
    double const position = std::abs(advance * 2 - 1) * _posArray.width;
    auto const color = getGradientColor(colorAdvance);
    int const red = color[0];
    int const green = color[1];
    int const blue = color[2];

    double const lengthMin = _posArray.width / 10.0;
    double const lengthMax = lengthMin * 2;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        int const x = _posArray.array[i] % _posArray.width;
        double const distance = std::abs(position - x);
        if (distance < lengthMin)
        {
            rgb[0] = red;
            rgb[1] = green;
            rgb[2] = blue;
        }
        else if (distance < lengthMax)
        {
            double const distance10 = (distance - lengthMin) / (lengthMax - lengthMin);
            double const distance01 = 1 - distance10;
            rgb[0] = red * distance01;
            rgb[1] = green * distance01;
            rgb[2] = blue * distance01;
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
    double const colorAdvance =  double(_prevUpdateMillis % (_loopTime * getColor().size())) / double(_loopTime * getColor().size());
    double const advance = double(_prevUpdateMillis % _loopTime) / double(_loopTime);
    double const position = std::abs(advance * 2 - 1) * _posArray.height;
    auto const color = getGradientColor(colorAdvance);
    int const red = color[0];
    int const green = color[1];
    int const blue = color[2];

    double const lengthMin = _posArray.height / 10.0;
    double const lengthMax = lengthMin * 2;
    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        int const y = _posArray.array[i] / _posArray.width;
        double const distance = std::abs(position - y);
        if (distance < lengthMin)
        {
            rgb[0] = red;
            rgb[1] = green;
            rgb[2] = blue;
        }
        else if (distance < lengthMax)
        {
            double const distance10 = (distance - lengthMin) / (lengthMax - lengthMin);
            double const distance01 = 1 - distance10;
            rgb[0] = red * distance01;
            rgb[1] = green * distance01;
            rgb[2] = blue * distance01;
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
    auto& colors = getColor();
    int colorIdx = (_prevUpdateMillis / _loopTime) % colors.size();
    int red = colors[colorIdx][0];
    int green = colors[colorIdx][1];
    int blue = colors[colorIdx][2];

    //3.14159
    //6.28319

    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        int x = _posArray.array[i] % _posArray.width;
        int y = _posArray.array[i] / _posArray.width;
        float nx = (float(x) + 0.5f) - (float(_posArray.width) * 0.5f);
        float ny = -((float(y) + 0.5f) - (float(_posArray.height) * 0.5f));

        float angle = std::atan2(ny, nx);
        int updateMillis = _prevUpdateMillis % _loopTime;
        updateMillis *= 6283;
        updateMillis /= _loopTime;
        float finalAngle = angle + float(updateMillis) / 1000.0f;
        float sine = std::sin(finalAngle * 2.0f);
        if (sine < 0)
        {
            rgb[0] = red;
            rgb[1] = green;
            rgb[2] = blue;
        }
        else
        {
            std::fill(std::begin(rgb), std::end(rgb), 0);
        }
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginRotationSmooth()
{
}

void RGBEffect::refreshPixelsRotationSmooth()
{
    auto const& colors = getColor();
    auto const color = getGradientColor(
                double(_prevUpdateMillis % (_loopTime * colors.size()))
                /
                double(_loopTime * colors.size())
                );
    int red = color[0];
    int green = color[1];
    int blue = color[2];

    //3.14159
    //6.28319

    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        int x = _posArray.array[i] % _posArray.width;
        int y = _posArray.array[i] / _posArray.width;
        float nx = (float(x) + 0.5f) - (float(_posArray.width) * 0.5f);
        float ny = -((float(y) + 0.5f) - (float(_posArray.height) * 0.5f));


        float angle = std::atan2(ny, nx);
        int updateMillis = _prevUpdateMillis % _loopTime;
        updateMillis *= 6283;
        updateMillis /= _loopTime;
        float finalAngle = angle + float(updateMillis) / 1000.0f;
        float sine = std::sin(finalAngle * 2.0f);
        float sinePositive = (sine + 1.0f) * 0.5f;

        rgb[0] = red * sinePositive;
        rgb[1] = green * sinePositive;
        rgb[2] = blue * sinePositive;
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginRotationSmoothThin()
{
}

void RGBEffect::refreshPixelsRotationSmoothThin()
{
    auto const& colors = getColor();
    auto const color = getGradientColor(
                double(_prevUpdateMillis % (_loopTime * colors.size()))
                /
                double(_loopTime * colors.size())
                );
    int red = color[0];
    int green = color[1];
    int blue = color[2];

    //3.14159
    //6.28319

    std::array<uint8_t, 3> rgb;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        int x = _posArray.array[i] % _posArray.width;
        int y = _posArray.array[i] / _posArray.width;
        float nx = (float(x) + 0.5f) - (float(_posArray.width) * 0.5f);
        float ny = -((float(y) + 0.5f) - (float(_posArray.height) * 0.5f));


        float angle = std::atan2(ny, nx);
        int updateMillis = _prevUpdateMillis % _loopTime;
        updateMillis *= 6283;
        updateMillis /= _loopTime;
        float finalAngle = angle + float(updateMillis) / 1000.0f;
        float sine = std::sin(finalAngle * 2.0f);
        float sinePositive = (sine + 1.0f) * 0.5f;

        rgb[0] = red * sinePositive * sinePositive;
        rgb[1] = green * sinePositive * sinePositive;
        rgb[2] = blue * sinePositive * sinePositive;
        mixPixel(_pixels + i * 3, rgb.data());
    }
}

void RGBEffect::beginPlasma()
{
}

void RGBEffect::refreshPixelsPlasma()
{
    const int presetSize = 7;
    const int speed = 50000 / _loopTime;

    int size = presetSize / 2;
    int plasmaStepCount = (_prevUpdateMillis / 20) * speed;
    plasmaStepCount %= 256000;

    double square = _posArray.width > _posArray.height ? _posArray.width : _posArray.height;
    for (unsigned int i = 0; i < _pixelCount; ++i)
    {
        int x = _posArray.array[i] % _posArray.width;
        int y = _posArray.array[i] / _posArray.width;
        double nx = double(x) / square;
        double ny = double(y) / square;
        double n = Perlin::noise(size * nx, size * ny, double(plasmaStepCount) / 1000.0);
        double n01 = (n + 1) / 2;
        mixPixel(_pixels + i * 3, getGradientColor(n01).data());
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
