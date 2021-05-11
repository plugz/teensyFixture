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
#include <vector>

#include "Button.hpp"
#include "Fader.hpp"
#include "InputMCPSPI.hpp"
#include "MyMCP3008.hpp"
#include "Translate.hpp"
#include "Utils.hpp"

/// DONT CHANGE unless you know the consequences...
// 60leds*5m
#define NUM_LEDS_PER_STRIP 300
#define NUM_STRIPS 8
#define NUM_LEDS (NUM_LEDS_PER_STRIP*NUM_STRIPS)         // can not go higher than this - Runs out of SRAM

// Define the array of leds
DMAMEM uint8_t displayMemory[3 * NUM_LEDS];
uint8_t drawingMemory[3 * NUM_LEDS];

static OctoWS2811 leds(NUM_LEDS_PER_STRIP, displayMemory, drawingMemory, WS2811_GRB | WS2811_800kHz);

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
    LOGLN_DEBUG("setup done");

    // Once the Ethernet is initialised, run a test on the LEDs
    flashLeds();
}

void loop() {
    InputMCPSPI::loop();
    Button::loop(); // InputMCP.update should run before so values are updated
    Fader::loop();

    // 50fps au pif
    static TimerMillis refreshTimer(1000 / 50);
    if (refreshTimer.advance()) {
        leds.show();
    }
}
