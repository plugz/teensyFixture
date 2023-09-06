// E1.31 Receiver code base by Andrew Huxtable (andrew@hux.net.au).Adapted and modified by
// ccr (megapixel.lighting).
//
// This code may be freely distributed and used as you see fit for non-profit
// purposes and as long as the original author is credited and it remains open
// source
//
// Please configure your Lighting product to use Unicast to the IP the device is given from your
// DHCP server Multicast is not currently supported due to bandwidth/processor limitations

#include "Log.hpp"

// clang-format off
#include <SPI.h>
#include <OctoWS2811.h>
// clang-format on

#include <array>

#include "Button.hpp"
#include "Encoder.hpp"
#include "Fader.hpp"
#include "InputMCPSPI.hpp"
#include "MyMCP3008.hpp"
#include "Translate.hpp"
#include "Utils.hpp"
#include "RGBEffectWrapper.hpp"

/// DONT CHANGE unless you know the consequences...
// 60leds*5m
// defined in wrapper
//#define NUM_LEDS_PER_STRIP 120
//#define NUM_STRIPS 8
//#define NUM_LEDS (NUM_LEDS_PER_STRIP*NUM_STRIPS)         // can not go higher than this - Runs out of SRAM

// Define the array of leds
DMAMEM uint8_t displayMemory[3 * NUM_LEDS];
uint8_t drawingMemory[3 * NUM_LEDS];

static OctoWS2811 leds(NUM_LEDS_PER_STRIP, displayMemory, nullptr, WS2811_GRB | WS2811_800kHz);

RGBEffectWrapper rgbEffect;

void flashLeds() {
    std::array<int, 3> colors[] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 0, 0}};
    for (auto const &color : colors)
    {
        for (int i = 0; i < NUM_LEDS_PER_STRIP * NUM_STRIPS; ++i)
            leds.setPixel(i, color[0], color[1], color[2]);
        leds.show();
        delay(500);
    }
}

static void updatePat0Turbo(bool turbo0, bool turbo1) {
    Float speed;
    if (!turbo0) {
        if (!turbo1)
            speed = Float::scaleUp(0.5f);
        else
            speed = Float::scaleUp(3.0f);
    }
    else {
        if (!turbo1)
            speed = Float::scaleUp(1.0f);
        else
            speed = Float::scaleUp(7.0f);
    }
    rgbEffect.pat0ChangeSpeed(speed);
}

static void updatePat1Turbo(bool turbo0, bool turbo1) {
    Float speed;
    if (!turbo0) {
        if (!turbo1)
            speed = Float::scaleUp(0.5f);
        else
            speed = Float::scaleUp(3.0f);
    }
    else {
        if (!turbo1)
            speed = Float::scaleUp(1.0f);
        else
            speed = Float::scaleUp(7.0f);
    }
    rgbEffect.pat1ChangeSpeed(speed);
}

static void btnCallback(uint8_t idx, bool val) {
    static bool pat0Turbo0 = false;
    static bool pat0Turbo1 = false;
    static bool pat1Turbo0 = false;
    static bool pat1Turbo1 = false;

    switch (idx) {
    case 0: { // btn strob soft
        rgbEffect.flashSmoothStrobe(val);
    } break;
    case 1: { // btn strob vnr
        rgbEffect.flashVnrStrobe(val);
    } break;
    case 2: { // btn smooth strobe blackout
        rgbEffect.flashSmoothOff(val);
    } break;
    case 3: { // btn full blackout
        rgbEffect.flashFullOff(val);
    } break;
    case 4: { // switch Hpart enable
        rgbEffect.hPartEnable(val);
    } break;
    case 5: { // switch Vpart enable
        rgbEffect.vPartEnable(val);
    } break;
    case 6: { // switch pattern0 turbo1
        pat0Turbo1 = val;
        updatePat0Turbo(pat0Turbo0, pat0Turbo1);
    } break;
    case 7: { // switch pattern0 turbo0
        pat0Turbo0 = val;
        updatePat0Turbo(pat0Turbo0, pat0Turbo1);
    } break;
    case 8: { // switch pattern1 turbo1
        pat1Turbo1 = val;
        updatePat1Turbo(pat1Turbo0, pat1Turbo1);
    } break;
    case 9: { // switch pattern1 turbo0
        pat1Turbo0 = val;
        updatePat1Turbo(pat1Turbo0, pat1Turbo1);
    } break;
    case 10: { // btn encoder 0 push
    } break;
    case 11: { // btn encoder 1 push
    } break;
    case 12: { // btn encoder 2 push
    } break;
    case 13: { // btn encoder 3 push
    } break;
    }
}

static void encoderCallback(uint8_t idx, bool val) {
    // val true -> CW
    // val false -> CCW
    switch (idx) {
    case 0: { // pattern 0 color
        if (val)
            rgbEffect.pat0NextColor();
        else
            rgbEffect.pat0PrevColor();
    } break;
    case 1: { // pattern 0 pattern
        if (val)
            rgbEffect.pat0NextMode();
        else
            rgbEffect.pat0PrevMode();
    } break;
    case 2: { // pattern 1 color
        if (val)
            rgbEffect.pat1NextColor();
        else
            rgbEffect.pat1PrevColor();
    } break;
    case 3: { // pattern 1 pattern
        if (val)
            rgbEffect.pat1NextMode();
        else
            rgbEffect.pat1PrevMode();
    } break;
    }
}

static void faderCallback(uint8_t idx, uint16_t val) {
    Float scaledVal = Float::scaleUp(val) / MYMCP3008_ANALOG_VALUECOUNT;
    switch (idx) {
    case 0: { // Hpart pos
        rgbEffect.hPartPos(scaledVal);
    } break;
    case 1: { // Hpart size
        rgbEffect.hPartSize(scaledVal);
    } break;
    case 2: { // Vpart pos
        rgbEffect.vPartPos(scaledVal);
    } break;
    case 3: { // Vpart size
        rgbEffect.vPartSize(scaledVal);
    } break;
    case 4: { // pattern0 dim
        rgbEffect.pat0ChangeDim(scaledVal);
    } break;
    case 5: { // pattern1 dim
        rgbEffect.pat1ChangeDim(scaledVal);
    } break;
    }
}

void setup() {
    LOGSETUP();

    Utils::begin();

    LOGLN_DEBUG("setup");
    SPI.begin();
    InputMCPSPI::setup();
    MyMCP3008::setup();
    Button::setup(btnCallback);
    Encoder::setup(encoderCallback);
    Fader::setup(faderCallback);
    Translate::setup();

    leds.begin();
    rgbEffect.begin(drawingMemory, NUM_LEDS);
    LOGLN_DEBUG("setup done");

    // Once the Ethernet is initialised, run a test on the LEDs
    flashLeds();
}

void loop() {
    InputMCPSPI::loop();
    Button::loop(); // InputMCP.update should run before so values are updated
    Encoder::loop();
    Fader::loop();

    if (rgbEffect.refreshPixels(millis())) {
        for (unsigned int i = 0; i < NUM_LEDS; ++i) {
            leds.setPixel(i, drawingMemory[i * 3 + 0], drawingMemory[i * 3 + 1],
                    drawingMemory[i * 3 + 2]);
        }
        leds.show();
    }
}
