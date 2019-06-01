#define DEBUG

#include "FastLED.h"
#include "ESP8266WiFi.h"

// Where is the LED data line connected?
#define LED_DATA D3

// Wifi credentials
/* #define STASSID "Pretty fly for a wifi" */
/* #define STAPSK "sosave420" */
#define STASSID "Nerdsphone"
#define STAPSK "bierbierbierbettbettbett"

// How many leds are there in your strip?
#define ALL_LEDS 30 // 178
#define NUM_LEDS ALL_LEDS/2

// the array of leds
CRGB all_leds[ALL_LEDS];
CRGB half_leds[NUM_LEDS];

// the currently selected color mode
char mode = 'a';

WiFiServer telnetServer(23);
WiFiClient telnetClient;

void connectToWifi() {
    // Init WifiManager
    Serial.print("Connecting to ");
    Serial.println(STASSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(STASSID, STAPSK);

    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("WiFi connected with IP ");
    Serial.println(WiFi.localIP());
}

void startServer() {
    telnetServer.begin();
    telnetServer.setNoDelay(true);
}

void setup() {
    Serial.begin(230400);

    connectToWifi();
    startServer();

    // Initialize the LEDs
    pinMode(LED_DATA, OUTPUT);
    FastLED.addLeds<WS2812B, LED_DATA, GRB>(all_leds, ALL_LEDS);
    FastLED.setBrightness(64);

    fill_solid(all_leds, ALL_LEDS, CRGB::Black);
    FastLED.show();

    #ifdef DEBUG
        Serial.println("\n\n === Lamp_Simple1Button.ino ===\n\n");
    #endif
}

void loop() {
    if(telnetServer.hasClient()) {
        if(!telnetClient || !telnetClient.connected()) {
            if (telnetClient) {
                telnetClient.stop();
            }
            telnetClient = telnetServer.available();
            telnetClient.flush();
        }
    }

    if(telnetClient.available()) {
        mode = telnetClient.read();
        Serial.println(mode);
    }
	telnetClient.flush();

    switch (mode) {
        case 'a':
            // all white
            fill_solid(half_leds, NUM_LEDS, CRGB::White);
            // fill_solid(half_leds, 5, CRGB::Red);
            break;
        case 'b':
            // flashing ring
            fill_solid(half_leds, NUM_LEDS, CRGB::Red);
            // flashing_ring();
            break;
        case 'c':
            // moving rainbw
            // fill_rainbow(half_leds, NUM_LEDS, );
            // moving_rainbow();
            break;
    }

    spaceOutPixels();
    FastLED.show();
    delay(10);
}

void spaceOutPixels() {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        all_leds[2*i] = half_leds[i];
    }
}
