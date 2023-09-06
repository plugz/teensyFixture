#ifndef __FIXTURE2_HPP__
#define __FIXTURE2_HPP__

#include "FakeFloat.hpp"
#include "Utils.hpp"

#include <array>
#include <cstdint>

// airDJ
#define ADJ_WIDTH 120
#define ADJ_HEIGHT 60
#define ADJ_DEPTH 30

enum class RGBEffectPattern
{
    PLAIN,
    STROBE,
    ON_OFF,
    SMOOTH_ON_OFF,
    SMOOTHER_ON_OFF,
    STRIPE,
    STRIPE_SMOOTH,
    ROTATION,
    ROTATION_THIN,
    ROTATION_SMOOTH,
    ROTATION_SMOOTH_THIN,
    PLASMA,

    COUNT
};

enum class RGBEffectColor
{
    BLACK,
    RED,
    YELLOW,
    GREEN,
    CYAN,
    BLUE,
    MAGENTA,
    WHITE,
    FLAME,
    GRASS,
    OCEAN,
    RAINBOW,
    GOLD,
    WHITEMIXED,
    PINK,
    ETHEREAL,
    WARM_SUNSET,
    DEEP_BLUE,
    MARGUERITA,
    PEACH,
    FRESH,
    VIOLET_SHADOWS,
    DARK_SIDE,

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

enum class RGBEffectAxis
{
    HORIZONTAL,
    VERTICAL,
    FRONTAL,
};

enum class RGBEffectDirection
{
    FORWARD,
    BACKWARD,
    PING_PONG,
};

class RGBEffect
{
  public:
    using Color = std::array<uint8_t, 3>;

    struct PatternDesc {
        RGBEffectPattern pattern;
        RGBEffectAxis axis = RGBEffectAxis::HORIZONTAL;
        RGBEffectDirection direction = RGBEffectDirection::FORWARD;
    };
    struct Desc {
        PatternDesc pattern;
        int loopTime;
        RGBEffectMixingMode mixingMode = RGBEffectMixingMode::REPLACE;
        RGBEffectColor color = RGBEffectColor::RAINBOW;
        Float dimmer = Float::scaleUp(1);
    };
    struct PosArray
    {
        // -1 means no binding
        StaticVector<int> array;
        unsigned int width;
        unsigned int height;
        unsigned int depth;
    };
    void begin(Desc const& desc,
               uint8_t *pixels,
               unsigned int pixelCount,
               PosArray const& posArray);
    static PosArray posArraySimple(StaticVector<int>& targetBuffer, unsigned int width,
                                   unsigned int height, unsigned int depth);
    static PosArray posArrayZigZag(StaticVector<int>& targetBuffer, unsigned int width,
                                   unsigned int height, unsigned int depth);
    static PosArray posArrayFromLedArray(StaticVector<int>& targetBuffer, unsigned int width,
                                         unsigned int height, unsigned int depth,
                                         StaticVector<int> const& ledArray);
    static PosArray posArrayAirDJ(StaticVector<int>& targetBuffer);

    void setPattern(RGBEffectPattern pattern);
    void setAxis(RGBEffectAxis axis);
    void setDirection(RGBEffectDirection direction);
    void setMixingMode(RGBEffectMixingMode mixingMode);
    int loopTime() const;
    void setLoopTime(int loopTime);
    void setColor(RGBEffectColor color);
    void setDimmer(Float dim);
    void setPosArray(PosArray const& posArray);

    bool refreshPixels(unsigned long currentMillis);

  private:
    Float advance() const; // advance in loop, [0-1]
    unsigned int axisSize() const;
    unsigned int perpendicularW() const;
    unsigned int perpendicularH() const;
    unsigned int idxToAxisIdx(unsigned int idx) const;
    unsigned int idxToPerpendicularX(unsigned int idx) const;
    unsigned int idxToPerpendicularY(unsigned int idx) const;
    std::array<uint8_t, 3> getTimeGradientColor(Float timeMultiplier = Float::scaleUp(1));
    std::array<uint8_t, 3> getGradientColor(Float advance);
    Color dimColor(Color color);
    static Color dimColor(Color color, Float);
    void beginCurrentCombo();
    StaticVector<std::array<uint8_t, 3>> const& getColor() const;

    uint8_t* _pixels;
    unsigned int _pixelCount;
    PosArray _posArray;
    unsigned long _startTime = 0;
    unsigned long _prevUpdateMillis = 0;
    Desc _desc;

    // plain
    void beginPlain();
    void refreshPixelsPlain();

    // strobe
    void beginStrobe();
    void refreshPixelsStrobe();

    // on-off
    void beginOnOff();
    void refreshPixelsOnOff();

    // smooth on-off
    void beginSmoothOnOff();
    void refreshPixelsSmoothOnOff();

    // smoother on-off
    void beginSmootherOnOff();
    void refreshPixelsSmootherOnOff();

    // stripe
    void beginStripe();
    void refreshPixelsStripe();

    // stripe smooth
    void beginStripeSmooth();
    void refreshPixelsStripeSmooth();

    // rotation (2-branch)
    void beginRotation();
    void refreshPixelsRotation();

    // rotation thin (2-branch)
    void beginRotationThin();
    void refreshPixelsRotationThin();

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
