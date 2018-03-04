#ifndef __FIXTURE_HPP__
#define __FIXTURE_HPP__

#include <cstdint>

class PixelDriver;

#define TARGET_FREQ_HZ 50

enum class FixtureMode {
    SIMPLE,       // RGB, RGB, RGB.....
    R_G_B_LEVELS, // 1 RGB for all pixels, then brightness of each pixel
    DEMO,
};

struct CRGB;

class Fixture {
  public:
    void begin(CRGB *pixels, int pixelCount);
    void updateInput(uint8_t const *data, unsigned int size);
    bool refreshPixels();

  private:
    void setAlwaysOn();
    void updateInputSimple(uint8_t const *data);
    void updateInputRGBLevels(uint8_t const *data);
    void updateInputDemo(uint8_t const *data);
    unsigned int getNumChannels() const;
    void refreshPixelsDemo();
    CRGB *_pixels = nullptr;
    int _pixelCount = 0;
    FixtureMode _mode;
    unsigned long _prevUpdateMillis = 0;

    long int demoPrevColorMillis = 0;
    long int demoPrevModeMillis = 0;
    int demoCurrentColorIdx = 0;
    int demoCurrentModeIdx = 0;
    void refreshPixelsDemoPlainSwitch(int colorAdvance);
    void refreshPixelsDemoPlainBlink(int colorAdvance);
    void refreshPixelsDemoPlainFade(int colorAdvance);
    void refreshPixelsDemoSnake(int colorAdvance);
    void refreshPixelsDemoSnakeFade(int colorAdvance);
};

#endif
