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
#include "Utils.hpp"

// clang-format off
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OctoWS2811.h>
// clang-format on

#include <array>
#include <vector>

// enter desired universe and subnet  (sACN first universe is 1)
#define DMX_SUBNET 0
#define DMX_UNIVERSE 1 //**Start** universe

// Set a different MAC address for each...
byte mac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x15};
// IP address of ethernet shield
static const IPAddress ip(192, 168, 2, 2);
#define SACN_PORT 5568
#define ETHERNET_BUFFER 636 // 540
static unsigned char packetBuffer[ETHERNET_BUFFER];

/// DONT CHANGE unless you know the consequences...
#define CHANNEL_COUNT 4800 // because it divides by 3 nicely
#define NUM_LEDS (170*8)         // can not go higher than this - Runs out of SRAM
#define NUM_LEDS_PER_STRIP 170
#define NUM_STRIPS 8
#define UNIVERSE_COUNT 8
#define LEDS_PER_UNIVERSE 170

static EthernetUDP Udps[UNIVERSE_COUNT];

// MAX VALUES: 32 universes (~25 fps ?)
// #define CHANNEL_COUNT 16320 //because it divides by 3 nicely
// #define NUM_LEDS 5440 // can not go higher than this - Runs out of SRAM
// #define NUM_LEDS_PER_STRIP 680
// #define NUM_STRIPS 8
// #define UNIVERSE_COUNT 32
// #define LEDS_PER_UNIVERSE 170

// Define the array of leds
DMAMEM uint8_t displayMemory[3 * NUM_LEDS_PER_STRIP * NUM_STRIPS];
uint8_t drawingMemory[3 * NUM_LEDS_PER_STRIP * NUM_STRIPS];

static OctoWS2811 leds(NUM_LEDS_PER_STRIP, displayMemory, drawingMemory, WS2811_RGB | WS2813_800kHz);

// fps log
static float fps = 0;
static unsigned long currentMillis = 0;
static unsigned long previousMillis = 0;

bool receivedUniverses[UNIVERSE_COUNT] = {
    false,
};

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

void setup() {
    pinMode(9, OUTPUT);
    digitalWrite(9, LOW); // reset the WIZ820io
    delay(10);
    digitalWrite(9, HIGH); // reset the WIZ820io

    LOGSETUP();

    leds.begin();

    // static ip
    Ethernet.begin(mac, ip);
    // dhcp
    // int status;
    // do {
    //    status = Ethernet.begin(mac);
    //    LOG_DEBUG("ethernet status: ");
    //    LOGLN_DEBUG(status);
    //} while (status == 0);
    LOG_DEBUG("IP: ");
    LOGLN_DEBUG(Ethernet.localIP());

    // unicast
    // Udp.begin(SACN_PORT);
    // multicast
    unsigned int i = 1;
    for (auto& Udp: Udps)
    {
        Udp.beginMulticast(IPAddress(239, 255, 0, i++), SACN_PORT);
    }

    // Once the Ethernet is initialised, run a test on the LEDs
    flashLeds();
}

void handleData(unsigned int universe, uint8_t *data, unsigned int dataSize) {
    unsigned int ledNumber = (universe - DMX_UNIVERSE) * LEDS_PER_UNIVERSE;
    for (unsigned int i = 0; i < dataSize; i += 3)
    {
        leds.setPixel(ledNumber++, data[i + 0], data[i + 1], data[i + 2]);
    }
    // TODO Test this also
    // memcpy(drawingMemory + (ledNumber * 3), data, dataSize);
}

void clearReceivedUniverses() {
    for (auto &receivedUniverse : receivedUniverses)
        receivedUniverse = false;
}

bool refreshLeds(unsigned int universe) {
    // transform universe to index in receivedUniverse
    universe = universe - DMX_UNIVERSE;

    // A universe from the previous frame was probably lost.
    // Display leds now, before updating data.
    if (receivedUniverses[universe]) {
        clearReceivedUniverses();
        leds.show();
        return false;
    }

    receivedUniverses[universe] = true;
    for (auto receivedUniverse : receivedUniverses) {
        // This frame is not complete, don't display it.
        // Only refresh data.
        if (!receivedUniverse)
            return false;
    }

    // This frame is complete.
    // Display leds after updating data.
    clearReceivedUniverses();
    return true;
}

void sacnDMXReceived(unsigned char *pbuff, int count) {
    if (count > CHANNEL_COUNT)
        count = CHANNEL_COUNT;
    byte b = pbuff[113]; // DMX Subnet
    if (b == DMX_SUBNET) {
        LOGLN_DEBUG("subnet OK");

        b = pbuff[114]; // DMX Universe

        if (b >= DMX_UNIVERSE && b <= DMX_UNIVERSE + UNIVERSE_COUNT) {
            LOGLN_DEBUG("universe OK");

            if (pbuff[125] == 0) // start code must be 0
            {
                LOGLN_DEBUG("startCode OK");
                bool refresh = refreshLeds(b);
                handleData(b, pbuff + 126, count);
                if (refresh)
                    leds.show();
            }
        }
    }
}

int checkACNHeaders(unsigned char *messagein, int messagelength) {
    // Do some VERY basic checks to see if it's an E1.31 packet.
    // Bytes 4 to 12 of an E1.31 Packet contain "ACN-E1.17"
    // Only checking for the A and the 7 in the right places as well as 0x10 as the header.
    // Technically this is outside of spec and could cause problems but its enough checks for us
    // to determine if the packet should be tossed or used.
    // This improves the speed of packet processing as well as reducing the memory overhead.
    // On an Isolated network this should never be a problem....
    if (messagein[1] == 0x10 && messagein[4] == 0x41 && messagein[12] == 0x37) {
        // number of values plus start code
        int addresscount = (byte)messagein[123] * 256 + (byte)messagein[124];
        return addresscount - 1; // Return how many values are in the packet.
    }
    return 0;
}

void loop() {
    // Process packets
    for (auto& Udp : Udps)
    {
    int packetSize = Udp.parsePacket(); // Read UDP packet count
    if (packetSize > 0) {
        LOGLN_DEBUG(packetSize);
        LOGLN_DEBUG("reading");
        Udp.read(packetBuffer, ETHERNET_BUFFER); // read UDP packet
        LOGLN_DEBUG("read done");
        int count = checkACNHeaders(packetBuffer, packetSize);
        LOGLN_DEBUG("check done");
        if (count) {
            LOG_DEBUG("packet size first ");
            LOGLN_DEBUG(packetSize);
            // calculate framerate
            currentMillis = millis();
            if (currentMillis > previousMillis) {
                fps = 1 / ((currentMillis - previousMillis) * 0.001);
            } else {
                fps = 0;
            }
            previousMillis = currentMillis;
            if (fps > 10 && fps < 500) // don't show numbers below or over given ammount
                LOGLN_DEBUG(fps);
            sacnDMXReceived(packetBuffer, count); // process data function
        } else
            LOGLN_DEBUG("not sacn");
    }
    }
}
