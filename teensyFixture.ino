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
    // plaf
    if (idx == 14) {
        if (val) {
            rgbEffect.startFlash();
        }
        else {
            rgbEffect.stopFlash();
        }
        return;
    }
    if (!val)
        return;
    switch (idx % 4) {
    case 0:
    default: {
        rgbEffect.prevMode();
    } break;
    case 1: {
        rgbEffect.nextMode();
    } break;
    case 2: {
        rgbEffect.prevColor();
    } break;
    case 3: {
        rgbEffect.nextColor();
    } break;
    }
}

static void faderCallback(uint8_t idx, uint16_t val) {
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
    Fader::loop();

    if (rgbEffect.refreshPixels(millis())) {
        for (unsigned int i = 0; i < NUM_LEDS; ++i) {
            leds.setPixel(i, drawingMemory[i * 3 + 0], drawingMemory[i * 3 + 1],
                    drawingMemory[i * 3 + 2]);
        }
        leds.show();
    }
}
