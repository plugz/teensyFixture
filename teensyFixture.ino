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
#define NUM_LEDS_PER_STRIP ADJ_HEIGHT
#define NUM_STRIPS 8
#define NUM_LEDS (NUM_LEDS_PER_STRIP*NUM_STRIPS)         // can not go higher than this - Runs out of SRAM

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

static void btnCallback(uint8_t idx, bool val) {
    switch (idx) {
    case 0: { // btn strob soft
        if (val) {
            rgbEffect.startFlash();
        }
        else {
            rgbEffect.stopFlash();
        }
        return;
    } break;
    case 1: { // btn strob vnr
        if (val) {
            rgbEffect.startFlash();
        }
        else {
            rgbEffect.stopFlash();
        }
    } break;
    case 2: { // btn smooth strobe blackout
        if (val) {
            rgbEffect.startFlash();
        }
        else {
            rgbEffect.stopFlash();
        }
    } break;
    case 3: { // btn full blackout
        if (val) {
            rgbEffect.startFlash();
        }
        else {
            rgbEffect.stopFlash();
        }
    } break;
    case 4: { // switch Hpart enable
    } break;
    case 5: { // switch Vpart enable
    } break;
    case 6: { // switch pattern0 enable
    } break;
    case 7: { // switch pattern0 turbo
    } break;
    case 8: { // switch pattern1 enable
    } break;
    case 9: { // switch pattern1 turbo
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
            rgbEffect.nextColor();
        else
            rgbEffect.prevColor();
    } break;
    case 1: { // pattern 0 pattern
        if (val)
            rgbEffect.nextMode();
        else
            rgbEffect.prevMode();
    } break;
    case 2: { // pattern 1 color
        if (val)
            rgbEffect.nextColor();
        else
            rgbEffect.prevColor();
    } break;
    case 3: { // pattern 0 color
        if (val)
            rgbEffect.nextMode();
        else
            rgbEffect.prevMode();
    } break;
    }
}

static void faderCallback(uint8_t idx, uint16_t val) {
    switch (idx) {
    case 0: { // Hpart pos
    } break;
    case 1: { // Hpart size
    } break;
    case 2: { // Vpart pos
    } break;
    case 3: { // Vpart size
    } break;
    case 4: { // pattern0 dim
    } break;
    case 5: { // pattern1 dim
    } break;
    }

    // TODO
//        rgbEffect.changeSpeed(Float::scaleUp(0.5));//Float::scaleUp(val) / Float::scaleUp(MYMCP3008_ANALOG_VALUEMAX));
//    switch (idx) {
//    case 0:
//    default: {
//        rgbEffect.changeSpeed(Float::scaleUp(val) / Float::scaleUp(MYMCP3008_ANALOG_VALUEMAX));
//    } break;
//    case 1: {
//        val = 0;
//    } break;
//    case 2: {
//        val = 0;
//    } break;
//    case 3: {
//        val = 0;
//    } break;
//    case 4: {
//        val = 0;
//    } break;
//    }
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
