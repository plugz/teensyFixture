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

#define ARTNET_PORT 6454

#define ARTNET_POLL           0x2000
#define ARTNET_POLLREPLY      0x2100
#define ARTNET_DIAGDATA       0x2300
#define ARTNET_COMMAND        0x2400
#define ARTNET_DMX            0x5000
#define ARTNET_NZS            0x5100
#define ARTNET_ADDRESS        0x6000
#define ARTNET_INPUT          0x7000
#define ARTNET_TODREQUEST     0x8000
#define ARTNET_TODDATA        0x8100
#define ARTNET_TODCONTROL     0x8200
#define ARTNET_RDM            0x8300
#define ARTNET_RDMSUB         0x8400
#define ARTNET_VIDEOSTEUP     0xa010
#define ARTNET_VIDEOPALETTE   0xa020
#define ARTNET_VIDEODATA      0xa040
#define ARTNET_MACMASTER      0xf000
#define ARTNET_MACSLAVE       0xf100
#define ARTNET_FIRMWAREMASTER 0xf200
#define ARTNET_FIRMWAREREPLY  0xf300
#define ARTNET_FILETNMASTER   0xf400
#define ARTNET_FILEFNMASTER   0xf500
#define ARTNET_FILEFNREPLY    0xf600
#define ARTNET_IPPROG         0xf800
#define ARTNET_IPREPLY        0xf900
#define ARTNET_MEDIA          0x9000
#define ARTNET_MEDIAPATCH     0x9100
#define ARTNET_MEDIACONTROL   0x9200
#define ARTNET_MEDIACONTROLREPLY 0x9300
#define ARTNET_TIMECODE       0x9700
#define ARTNET_TIMESYNC       0x9800
#define ARTNET_TRIGGER        0x9900
#define ARTNET_DIRECTORY      0x9a00
#define ARTNET_DIRECTORYREPLY 0x9b00

#define CTRL_NUMBER 0

// Set a different MAC address for each...
byte mac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x15 + CTRL_NUMBER};
// IP address of ethernet shield
static const IPAddress ip(192, 168, 2, 2 + CTRL_NUMBER);
#define ETHERNET_BUFFER 636 // 540
static unsigned char packetBuffer[ETHERNET_BUFFER];

/// DONT CHANGE unless you know the consequences...
#define NUM_LEDS_PER_STRIP (18 * 9 * 2)
#define NUM_STRIPS 8
#define NUM_LEDS (NUM_LEDS_PER_STRIP*NUM_STRIPS)         // can not go higher than this - Runs out of SRAM
// 6 universes per strip
// 8 strips
#define UNIVERSE_COUNT (2 * 8)
#define LEDS_PER_UNIVERSE (18 * 9)
#define CHANNELS_PER_UNIVERSE (LEDS_PER_UNIVERSE * 3)

// enter desired universe and subnet  (artnet first universe is 0)
#define DMX_SUBNET 0
#define DMX_UNIVERSE (0 + CTRL_NUMBER * UNIVERSE_COUNT) //**Start** universe

static EthernetUDP udp;

// Define the array of leds
DMAMEM uint8_t displayMemory[3 * NUM_LEDS];
uint8_t drawingMemory[3 * NUM_LEDS];

static OctoWS2811 leds(NUM_LEDS_PER_STRIP, displayMemory, drawingMemory, WS2811_GRB | WS2811_800kHz);

static unsigned long currentMillis = 0;
static unsigned long previousMillis = 0;
static unsigned long previousDisplayMillis = 0;
static bool cleared = false;

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
    udp.begin(ARTNET_PORT);
    // multicast
    //unsigned int i = 9;
    //unsigned int i = 1 + (CTRL_NUMBER * 8);
    //for (auto& udp: udps)
    //{
    //    udp.beginMulticast(IPAddress(239, 255, 0, i++), ARTNET_PORT);
    //}

    // Once the Ethernet is initialised, run a test on the LEDs
    flashLeds();
}

void handleData(unsigned int universe, uint8_t *data, unsigned int dataSize) {
    unsigned int ledNumber = (universe - DMX_UNIVERSE) * LEDS_PER_UNIVERSE;
    for (unsigned int i = 0; i < dataSize; i += 3)
    {
        //if (i >= 360)
        //    return;
        leds.setPixel(ledNumber++, data[i + 0], data[i + 1], data[i + 2]);
    }
}

void clearReceivedUniverses() {
    for (auto &receivedUniverse : receivedUniverses)
        receivedUniverse = false;
    cleared = true;

    // these are not connected
    receivedUniverses[10] = true;
    receivedUniverses[11] = true;
}

bool refreshLeds(unsigned int universe) {
    // transform universe to index in receivedUniverse
    universe = universe - DMX_UNIVERSE;

    // A universe from the previous frame was probably lost.
    // Display leds now, before updating data.
    if (receivedUniverses[universe]) {
        clearReceivedUniverses();
        receivedUniverses[universe] = true;
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

void artnetDMXReceived(unsigned char *pbuff, int count) {
    if (count < 19)
        return;

    int universe = ((int)pbuff[15] << 8) | pbuff[14];
    int dmxLen = ((int)pbuff[16] << 8) | pbuff[17];
    int sequence = pbuff[12];

    if (dmxLen > CHANNELS_PER_UNIVERSE)
        dmxLen = CHANNELS_PER_UNIVERSE;

    if (universe >= DMX_UNIVERSE && universe <= DMX_UNIVERSE + UNIVERSE_COUNT) {
        LOGLN_DEBUG("universe OK");

        handleData(universe, pbuff + 18, dmxLen);
    }
}

static bool checkPacket(uint8_t const* buf, unsigned int len, int* opCode)
{
    if (len < 12)
        return false;
    if (memcmp(buf, "Art-Net", 7))
        return false;
    if (buf[7] != 0)
        return false;

    *opCode = ((int)buf[9] << 8) | buf[8];
    return true;
}

void loop() {
    static bool receivedStuff = false;
    static int receivedTime = 0;
    // Process packets
    bool receivedStuffNow = false;
    //for (auto& udp : udps)
    {
        int packetSize;
        while ((packetSize = udp.parsePacket()) > 0) // Read UDP packet count
        {
            LOGLN_DEBUG(packetSize);
            LOGLN_DEBUG("reading");
            udp.read(packetBuffer, sizeof(packetBuffer)); // read UDP packet
            LOGLN_DEBUG("read done");
            int opcode;
            bool check = checkPacket(packetBuffer, packetSize, &opcode);
            LOGLN_DEBUG("check done");
            if (check && opcode == ARTNET_DMX) {
                LOG_DEBUG("packet size first ");
                LOGLN_DEBUG(packetSize);
                artnetDMXReceived(packetBuffer, packetSize); // process data function
                previousMillis = millis();
                cleared = false;
                receivedStuffNow = true;
            } else
                LOGLN_DEBUG("not artnet");
        }
    }
    if (receivedStuffNow)
    {
        receivedStuff = true;
        receivedTime = millis();
    }
    if (receivedStuff && (millis() - receivedTime > 3))
    {
        receivedStuff = false;
        leds.show();
    }
//    if (!cleared)
//    {
//        if (millis() - previousMillis > 1000)
//        {
//            clearReceivedUniverses();
//        }
//    }
//    int currentTime = millis();
//    if (currentTime - previousDisplayMillis > 40) // 25 fps
//    {
//        previousDisplayMillis = currentTime;
//        leds.show();
//    }
}
