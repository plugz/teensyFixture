#ifndef __FIXTURE2_HPP__
#define __FIXTURE2_HPP__

#include <array>
#include <cstdint>
#include <vector>

enum class RGBEffectPattern
{
    SMOOTH_ON_OFF,
    SMOOTHER_ON_OFF,
    STROBE,
    STRIPE,
    STRIPE_H_LEFT_RIGHT,
    STRIPE_V_UP_DOWN,
    STRIPE_REV,
    STRIPE_H_RIGHT_LEFT,
    STRIPE_V_DOWN_UP,
    STRIPE_SMOOTH,
    STRIPE_SMOOTH_H_LEFT_RIGHT,
    STRIPE_SMOOTH_V_UP_DOWN,
    STRIPE_SMOOTH_REV,
    STRIPE_SMOOTH_H_RIGHT_LEFT,
    STRIPE_SMOOTH_V_DOWN_UP,
    PING_PONG,
    PING_PONG_H,
    PING_PONG_V,
    PING_PONG_SMOOTH,
    PING_PONG_SMOOTH_H,
    PING_PONG_SMOOTH_V,
    ROTATION,
    ROTATION_SMOOTH,
    ROTATION_SMOOTH_THIN,
    PLASMA,

    COUNT
};

enum class RGBEffectColor
{
    FLAME,
    GRASS,
    OCEAN,
    RAINBOW,
    GOLD,
    WHITE,
    PINK,

    COUNT
};

enum class RGBEffectMixingMode
{
    REPLACE,
    MAX,
    MIN,
    ADD,
    SUB,
    MULTIPLY,
    DIVIDE,
    SCREEN,
    OVERLAY,
    DODGE, // BUG
    BURN, // BUG
    HARDLIGHT,
    SOFTLIGHT, // BUG
    GRAINEXTRACT,
    GRAINMERGE,
    DIFFERENCE,

    COUNT,
};

class RGBEffect
{
  public:
    struct PosArray
    {
        std::vector<int> array;
        unsigned int width;
        unsigned int height;
    };
    void begin(RGBEffectPattern pattern,
               RGBEffectColor color,
               RGBEffectMixingMode mixingMode,
               uint8_t *pixels,
               unsigned int pixelCount,
               PosArray const& posArray = {{}, 0, 0});
    static PosArray posArraySimple(unsigned int width, unsigned int height);
    static PosArray posArrayZigZag(unsigned int width, unsigned int height);
    static PosArray posArrayFromLedArray(std::vector<int> const& ledArray, unsigned int width, unsigned int height);

    void setPattern(RGBEffectPattern pattern);
    void setColor(RGBEffectColor color);
    void setMixingMode(RGBEffectMixingMode mixingMode);
    void setPosArray(PosArray const& posArray);
    int loopTime() const;
    void setLoopTime(int loopTime);
    bool refreshPixels(unsigned long currentMillis);

  private:
    std::array<uint8_t, 3> getGradientColor(double advance);
    void beginCurrentCombo();
    std::vector<std::array<uint8_t, 3>> const& getColor() const;
    uint8_t* _pixels;
    unsigned int _pixelCount;
    PosArray _posArray;
    unsigned long _prevUpdateMillis = 0;
    RGBEffectPattern _pattern;
    RGBEffectColor _color;
    RGBEffectMixingMode _mixingMode;
    int _loopTime = 8000;

    // smooth on-off
    void beginSmoothOnOff();
    void refreshPixelsSmoothOnOff();

    // smoother on-off
    void beginSmootherOnOff();
    void refreshPixelsSmootherOnOff();

    // strobe
    void beginStrobe();
    void refreshPixelsStrobe();

    // stripe
    void beginStripe();
    void refreshPixelsStripe();

    // stripe
    void beginStripeHLeftRight();
    void refreshPixelsStripeHLeftRight();

    // stripe
    void beginStripeVUpDown();
    void refreshPixelsStripeVUpDown();

    // stripe
    void beginStripeRev();
    void refreshPixelsStripeRev();

    // stripe
    void beginStripeHRightLeft();
    void refreshPixelsStripeHRightLeft();

    // stripe
    void beginStripeVDownUp();
    void refreshPixelsStripeVDownUp();

    // stripe smooth
    void beginStripeSmooth();
    void refreshPixelsStripeSmooth();

    // stripe smooth
    void beginStripeSmoothHLeftRight();
    void refreshPixelsStripeSmoothHLeftRight();

    // stripe smooth
    void beginStripeSmoothVUpDown();
    void refreshPixelsStripeSmoothVUpDown();

    // stripe smooth
    void beginStripeSmoothRev();
    void refreshPixelsStripeSmoothRev();

    // stripe smooth
    void beginStripeSmoothHRightLeft();
    void refreshPixelsStripeSmoothHRightLeft();

    // stripe smooth
    void beginStripeSmoothVDownUp();
    void refreshPixelsStripeSmoothVDownUp();

    // ping pong (single pixel)
    void beginPingPong();
    void refreshPixelsPingPong();

    // ping pong (single pixel)
    void beginPingPongH();
    void refreshPixelsPingPongH();

    // ping pong (single pixel)
    void beginPingPongV();
    void refreshPixelsPingPongV();

    // ping pong smooth
    void beginPingPongSmooth();
    void refreshPixelsPingPongSmooth();

    // ping pong smooth
    void beginPingPongSmoothH();
    void refreshPixelsPingPongSmoothH();

    // ping pong smooth
    void beginPingPongSmoothV();
    void refreshPixelsPingPongSmoothV();

    // rotation (2-branch)
    void beginRotation();
    void refreshPixelsRotation();

    // rotation smooth (2-branch)
    void beginRotationSmooth();
    void refreshPixelsRotationSmooth();

    // rotation smooth thin (2-branch)
    void beginRotationSmoothThin();
    void refreshPixelsRotationSmoothThin();

    // plasma
    void beginPlasma();
    void refreshPixelsPlasma();

    void (*mixPixel)(uint8_t* pixel, uint8_t const* values);
    static void mixPixelReplace(uint8_t* pixel, uint8_t const* values);
    static void mixPixelMax(uint8_t* pixel, uint8_t const* values);
    static void mixPixelMin(uint8_t* pixel, uint8_t const* values);
    static void mixPixelAdd(uint8_t* pixel, uint8_t const* values);
    static void mixPixelSub(uint8_t* pixel, uint8_t const* values);
    static void mixPixelMultiply(uint8_t* pixel, uint8_t const* values);
    static void mixPixelDivide(uint8_t* pixel, uint8_t const* values);
    static void mixPixelScreen(uint8_t* pixel, uint8_t const* values);
    static void mixPixelOverlay(uint8_t* pixel, uint8_t const* values);
    static void mixPixelDodge(uint8_t* pixel, uint8_t const* values);
    static void mixPixelBurn(uint8_t* pixel, uint8_t const* values);
    static void mixPixelHardLight(uint8_t* pixel, uint8_t const* values);
    static void mixPixelSoftLight(uint8_t* pixel, uint8_t const* values);
    static void mixPixelGrainExtract(uint8_t* pixel, uint8_t const* values);
    static void mixPixelGrainMerge(uint8_t* pixel, uint8_t const* values);
    static void mixPixelDifference(uint8_t* pixel, uint8_t const* values);
};

#endif
