#define DEBUG

#include "FastLED.h"
#include "ESP8266WiFi.h"

// How many leds are there in your strip?
#define NUM_LEDS 178

// Where is the LED data line connected?
#define LED_DATA D3

// Wifi credentials
/* #define STASSID "Pretty fly for a wifi" */
/* #define STAPSK "sosave420" */
#define STASSID "Nerdsphone"
#define STAPSK "bierbierbierbettbettbett"

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
	FastLED.addLeds<WS2812B, LED_DATA, GRB>(leds, NUM_LEDS);
	FastLED.setBrightness(64);

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

	while(telnetClient.available()) {
		Serial.println(telnetClient.read());
	}
	delay(10);

	// I usually save the current millisecond count per loop.
	uint32_t ms = millis();

	fill_solid(leds, NUM_LEDS, CRGB::Black);
	leds[0]=CRGB::Green;
	leds[50]=CRGB::Red;
	leds[100]=CRGB::Red;
	leds[150]=CRGB::Red;
	leds[177]=CRGB::Yellow;
	FastLED.show();
}
