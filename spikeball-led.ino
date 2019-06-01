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
char mode = 'c';

int brightness = 32;
byte speed_div = 4;

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

void readTelnet() {
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
        char input = telnetClient.read();
        switch (input) {
            case 'a':
                // change speed
                char speed_inp;
				speed_inp = telnetClient.read();
                switch (speed_inp) {
                    case '0':
                        speed_div = 0;
                        break;
                    case '1':
                        speed_div = 2;
                        break;
                    case '2':
                        speed_div = 4;
                        break;
                    case '3':
                        speed_div = 6;
                        break;

                }
                break;
            case 'b':
                // change speed_div
                char bright_inp;
				bright_inp = telnetClient.read();
                switch (bright_inp) {
                    case '0':
                        brightness = 8;
                        break;
                    case '1':
                        brightness = 16;
                        break;
                    case '2':
                        brightness = 32;
                        break;
                    case '3':
                        brightness = 64;
                        break;
                    case '4':
                        brightness = 96;
                        break;
                    case '5':
                        brightness = 128;
                        break;
                }
                break;
            default:
                mode = input;
                break;
        }

        Serial.println(mode);
    }
	telnetClient.flush();
}

void setup() {
    Serial.begin(230400);

    connectToWifi();
    startServer();

    // Initialize the LEDs
    pinMode(LED_DATA, OUTPUT);
    FastLED.addLeds<WS2812B, LED_DATA, GRB>(all_leds, ALL_LEDS);
    FastLED.setBrightness(brightness);

    fill_solid(all_leds, ALL_LEDS, CRGB::Black);
    FastLED.show();

    #ifdef DEBUG
        Serial.println("\n\n === Lamp_Simple1Button.ino ===\n\n");
    #endif
}

void loop() {
    uint32_t ms = millis();

	readTelnet();
	FastLED.setBrightness(brightness);

    switch (mode) {
        case 'c':
            // all white
            fill_solid(half_leds, NUM_LEDS, CRGB::White);
            // fill_solid(half_leds, 5, CRGB::Red);
            break;
        case 'd':
            // flashing ring
            fill_solid(half_leds, NUM_LEDS, CRGB::Red);
            // flashing_ring();
            break;
        case 'e':
            // moving rainbw
            fill_rainbow(half_leds, NUM_LEDS, (ms >> speed_div) & 0xFF, 255/NUM_LEDS);
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
