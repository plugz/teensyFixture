// E1.31 Receiver code base by Andrew Huxtable (andrew@hux.net.au).Adapted and modified by
// ccr (megapixel.lighting).
//
// This code may be freely distributed and used as you see fit for non-profit
// purposes and as long as the original author is credited and it remains open
// source
//
// Please configure your Lighting product to use Unicast to the IP the device is given from your
// DHCP server Multicast is not currently supported due to bandwidth/processor limitations

// fix undefined reference blabla
extern "C" {
    int getpid() { return -1; }
    int _kill(int, int) { return -1; }
}
namespace std {
    void __throw_bad_alloc() {
        while (1);
    }
    void __throw_length_error(char const*) {
        while (1);
    }
}

#include "Log.hpp"

// clang-format off
#include <SPI.h>
#include <OctoWS2811.h>
// clang-format on

#include <array>
#include <vector>

#include "Button.hpp"
#include "Fader.hpp"
#include "InputMCPSPI.hpp"
#include "MyMCP3008.hpp"
#include "Translate.hpp"
#include "Utils.hpp"
#include "RGBEffectWrapper.hpp"

/// DONT CHANGE unless you know the consequences...
// 60leds*5m
#define NUM_LEDS_PER_STRIP 300
#define NUM_STRIPS 8
#define NUM_LEDS (NUM_LEDS_PER_STRIP*NUM_STRIPS)         // can not go higher than this - Runs out of SRAM

// Define the array of leds
DMAMEM uint8_t displayMemory[3 * NUM_LEDS];
uint8_t drawingMemory[3 * NUM_LEDS];

static OctoWS2811 leds(NUM_LEDS_PER_STRIP, displayMemory, nullptr, WS2811_GRB | WS2811_800kHz);

RGBEffectWrapper rgbEffect;

void flashLeds() {
    for (std::array<int, 3> const &color :
    std::vector<std::array<int, 3>>{{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 0, 0}})
    {
        for (int i = 0; i < NUM_LEDS_PER_STRIP * NUM_STRIPS; ++i)
            leds.setPixel(i, color[0], color[1], color[2]);
        leds.show();
        delay(500);
    }
}

static void btnCallback(uint8_t idx, bool val) {
    if (idx == 0 && val) {
        rgbEffect.nextMode();
    }
    else if (idx == 1 && val) {
        rgbEffect.nextColor();
    }
}

static void faderCallback(uint8_t idx, uint16_t val) {
}

void setup() {
    LOGSETUP();

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
