#define DEBUG

#include "FastLED.h"
#include "ESP8266WiFi.h"

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

// How many leds are there in your strip?
#define NUM_LEDS 250

// Where is the LED data line connected?
#define LED_DATA D3


// Speed 0..8 (quadratic divisor of milliseconds)
// 0 = so fast that it's definitely not recommended for photosensitive people
// 4 = a nice pace, you could watch it all day
// 7 = so slow, you almost don't see it moving
// 8 and above = animation is stopped
#define SPEED_DIV 3

// state variable for brightness.
byte bright = 1;

// the array of leds
CRGB leds[NUM_LEDS];

void setup() {
	// Init WifiManager
	WiFiManager wifiManager;
	wifiManager.autoConnect("spikeball-led","spike420");
	
	// Initialize the LEDs
	pinMode(LED_DATA, OUTPUT);
	FastLED.addLeds<WS2812B, LED_DATA, GRB>(leds, NUM_LEDS);
	FastLED.setBrightness(64);

#ifdef DEBUG
	Serial.begin(230400);
	Serial.println("\n\n === Lamp_Simple1Button.ino ===\n\n");
#endif
}

void loop() {
	// I usually save the current millisecond count per loop.
	uint32_t ms = millis();

	fill_solid(leds, NUM_LEDS, CRGB::Black);
	leds[0]=CRGB::Green;
	leds[50]=CRGB::Red;
	leds[100]=CRGB::Red;
	leds[150]=CRGB::Red;
	leds[177]=CRGB::Yellow;
	FastLED.show();

	wdt_reset();
}
