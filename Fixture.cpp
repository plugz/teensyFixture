#include "Fixture.hpp"

#include <iterator>
#include <FastLED.h>

#define ARRAYSIZE(array) (sizeof(array) / sizeof(*array))

void Fixture::begin(CRGB* pixels, int pixelCount)
{
    _pixels = pixels;
    _pixelCount = pixelCount;
    _mode = FixtureMode::DEMO;
}

void Fixture::updateInput(uint8_t const* data, unsigned int size)
{
    if (size < getNumChannels())
    {
        //LOG_PORT.println("Not enough bytes in data, abandon");
        return;
    }

    switch (_mode)
    {
        case FixtureMode::SIMPLE:
        default:
            updateInputSimple(data);
            break;
        case FixtureMode::R_G_B_LEVELS:
            updateInputRGBLevels(data);
            break;
        case FixtureMode::DEMO:
            updateInputDemo(data);
            break;
    }
}

void Fixture::updateInputSimple(uint8_t const* data)
{
    for (unsigned int i = 0; i < getNumChannels() / 3; ++i)
    {
        _pixels[i] = CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
}

void Fixture::updateInputRGBLevels(uint8_t const* data)
{
    int r = data[0];
    int g = data[1];
    int b = data[2];
    for (unsigned int i = 0; i < getNumChannels() - 3; ++i)
    {
        int brightness = data[i + 3];
        _pixels[i] = CRGB((r * brightness) / 255, (g * brightness) / 255, (b * brightness) / 255);
    }
}

void Fixture::updateInputDemo(uint8_t const*)
{
}

bool Fixture::refreshPixels()
{
    unsigned long currentMillis = millis();
    if (currentMillis - _prevUpdateMillis > 20) // 50fps
    {
        _prevUpdateMillis += 20;
        switch (_mode)
        {
            case FixtureMode::SIMPLE:
            default:
            case FixtureMode::R_G_B_LEVELS:
                break;
            case FixtureMode::DEMO:
                refreshPixelsDemo();
                break;
        }
        return true;
    }
    return false;
}

unsigned int Fixture::getNumChannels() const
{
    switch (_mode)
    {
        case FixtureMode::SIMPLE:
        default:
            return _pixelCount * 3;
        case FixtureMode::R_G_B_LEVELS:
            return 3 + _pixelCount;
        case FixtureMode::DEMO:
            return 1;
    }
}

enum class DemoMode
{
    PLAINSWITCH,
    PLAINBLINK,
    PLAINFADE,
    SNAKE,
    SNAKEFADE
};

static DemoMode demoModes[] = {
    DemoMode::PLAINSWITCH,
    DemoMode::PLAINFADE,
    DemoMode::PLAINBLINK,
    DemoMode::PLAINFADE,
//    DemoMode::SNAKE,
//    DemoMode::SNAKEFADE
};

enum class DemoColor
{
    RED = 0xff0000,
    YELLOW = 0xffff00,
    GREEN = 0x00ff00,
    CYAN = 0x00ffff,
    BLUE = 0x0000ff,
    MAGENTA = 0xff00ff,
    WHITE = 0xffffff,
};

static DemoColor demoColors[] = {
//    DemoColor::RED,
//    DemoColor::GREEN,
//    DemoColor::BLUE,
//    DemoColor::YELLOW,
//    DemoColor::CYAN,
//    DemoColor::MAGENTA,
    DemoColor::WHITE
};

// 10000ms switch
#define DEMO_SWITCHTIME 10000

void Fixture::refreshPixelsDemo()
{
    long int currentMillis = millis();
    long int colorAdvance = currentMillis - demoPrevColorMillis;
    demoCurrentColorIdx += colorAdvance / DEMO_SWITCHTIME;
    demoCurrentColorIdx %= ARRAYSIZE(demoColors);
    demoPrevColorMillis += (colorAdvance / DEMO_SWITCHTIME) * DEMO_SWITCHTIME;
    colorAdvance -= (colorAdvance / DEMO_SWITCHTIME) * DEMO_SWITCHTIME;
    long int modeAdvance = currentMillis - demoPrevModeMillis;
    demoCurrentModeIdx += modeAdvance / (DEMO_SWITCHTIME * ARRAYSIZE(demoColors));
    demoCurrentModeIdx %= ARRAYSIZE(demoModes);
    demoPrevModeMillis += (modeAdvance / (DEMO_SWITCHTIME * ARRAYSIZE(demoColors))) * (DEMO_SWITCHTIME * ARRAYSIZE(demoColors));

    switch (demoModes[demoCurrentModeIdx])
    {
        case DemoMode::PLAINSWITCH:
            refreshPixelsDemoPlainSwitch(colorAdvance);
            break;
        case DemoMode::PLAINBLINK:
            refreshPixelsDemoPlainBlink(colorAdvance);
            break;
        case DemoMode::PLAINFADE:
            refreshPixelsDemoPlainFade(colorAdvance);
            break;
        case DemoMode::SNAKE:
            refreshPixelsDemoSnake(colorAdvance);
            break;
        case DemoMode::SNAKEFADE:
            refreshPixelsDemoSnakeFade(colorAdvance);
            break;
    }
}

void Fixture::refreshPixelsDemoPlainSwitch(int)
{
    int red = ((int)demoColors[demoCurrentColorIdx] >> 16) & 0xff;
    int green = ((int)demoColors[demoCurrentColorIdx] >> 8) & 0xff;
    int blue = ((int)demoColors[demoCurrentColorIdx]) & 0xff;
    for (int i = 0; i < _pixelCount; ++i)
    {
        _pixels[i] = CRGB(red, green, blue);
    }
}

void Fixture::refreshPixelsDemoPlainBlink(int colorAdvance)
{
    int red = ((int)demoColors[demoCurrentColorIdx] >> 16) & 0xff;
    int green = ((int)demoColors[demoCurrentColorIdx] >> 8) & 0xff;
    int blue = ((int)demoColors[demoCurrentColorIdx]) & 0xff;
    red = red * (((colorAdvance * 10) / DEMO_SWITCHTIME) % 2);
    green = green * (((colorAdvance * 10) / DEMO_SWITCHTIME) % 2);
    blue = blue * (((colorAdvance * 10) / DEMO_SWITCHTIME) % 2);
    for (int i = 0; i < _pixelCount; ++i)
    {
        _pixels[i] = CRGB(red, green, blue);
    }
}

void Fixture::refreshPixelsDemoPlainFade(int colorAdvance)
{
    int red = ((int)demoColors[demoCurrentColorIdx] >> 16) & 0xff;
    int green = ((int)demoColors[demoCurrentColorIdx] >> 8) & 0xff;
    int blue = ((int)demoColors[demoCurrentColorIdx]) & 0xff;
    red = (red * ((DEMO_SWITCHTIME / 2) - abs((DEMO_SWITCHTIME / 2) - colorAdvance))) / (DEMO_SWITCHTIME / 2);
    green = (green * ((DEMO_SWITCHTIME / 2) - abs((DEMO_SWITCHTIME / 2) - colorAdvance))) / (DEMO_SWITCHTIME / 2);
    blue = (blue * ((DEMO_SWITCHTIME / 2) - abs((DEMO_SWITCHTIME / 2) - colorAdvance))) / (DEMO_SWITCHTIME / 2);
    for (int i = 0; i < _pixelCount; ++i)
    {
        _pixels[i] = CRGB(red, green, blue);
    }
}

void Fixture::refreshPixelsDemoSnake(int colorAdvance)
{
    int red = ((int)demoColors[demoCurrentColorIdx] >> 16) & 0xff;
    int green = ((int)demoColors[demoCurrentColorIdx] >> 8) & 0xff;
    int blue = ((int)demoColors[demoCurrentColorIdx]) & 0xff;
    int position = (_pixelCount * ((DEMO_SWITCHTIME / 2) - abs((DEMO_SWITCHTIME / 2) - colorAdvance))) / (DEMO_SWITCHTIME / 2);
    for (int i = 0; i < _pixelCount; ++i)
    {
        if (abs(position - i) < 4)
        {
            _pixels[i] = CRGB(red, green, blue);
        }
        else
        {
            _pixels[i] = CRGB(0, 0, 0);
        }
    }
}

void Fixture::refreshPixelsDemoSnakeFade(int colorAdvance)
{
    int red = ((int)demoColors[demoCurrentColorIdx] >> 16) & 0xff;
    int green = ((int)demoColors[demoCurrentColorIdx] >> 8) & 0xff;
    int blue = ((int)demoColors[demoCurrentColorIdx]) & 0xff;
    int position = (_pixelCount * colorAdvance) / DEMO_SWITCHTIME;
    for (int i = 0; i < _pixelCount; ++i)
    {
        int r = (red * (_pixelCount - abs(position - i))) / _pixelCount;
        int g = (green * (_pixelCount - abs(position - i))) / _pixelCount;
        int b = (blue * (_pixelCount - abs(position - i))) / _pixelCount;
        _pixels[i] = CRGB(r, g, b);
    }
}
