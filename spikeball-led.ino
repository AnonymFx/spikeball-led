#define DEBUG

#include "FastLED.h"
#include "ESP8266WiFi.h"

// Where is the LED data line connected?
#define LED_DATA D3

// Wifi credentials
#define STASSID "Pretty fly for a wifi"
#define STAPSK "sosave420"

// How many leds are there in your strip?
#define ALL_LEDS 30 //178
#define NUM_LEDS ALL_LEDS/2

// the array of leds
CRGB all_leds[ALL_LEDS];
CRGB half_leds[NUM_LEDS];

// the currently selected color mode
char mode = 'c';

int brightness = 128;
byte speed_div = 5;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int counter = 0;

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
			telnetClient.write("\n\nspeed change (a[0-3])\nbrightness change (b[0-5])\nall white (c)\nall red (d)\nrainbow (e)\nrainbowWithGlitter(f)\nconfetti (g)\nsinelon (h)\nbpm (i)\njuggle (j)\n\n");
			telnetClient.flush();
		}
	}

	if(telnetClient.available()) {
		char input = telnetClient.read();
		Serial.println(input);
		switch (input) {
			case 13:
			case 10:
				break;
			case 'a':
				{
					// change speed
					char speed_inp;
					speed_inp = telnetClient.read();
					Serial.println(speed_inp);
					switch (speed_inp) {
						case '0':
							speed_div = 1;
							break;
						case '1':
							speed_div = 5;
							break;
						case '2':
							speed_div = 10;
							break;
						case '3':
							speed_div = 20;
							break;

					}
				}
				break;
			case 'b':
				{
					// change speed_div
					char bright_inp;
					bright_inp = telnetClient.read();
					Serial.println(bright_inp);
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
				}
				break;
			default:
				{
					mode = input;
					Serial.println("Mode set");
				}
				break;
		}
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
			rainbow();
			break;
		case 'f':
			// moving rainbw
			rainbowWithGlitter();
			break;
		case 'g':
			// moving rainbw
			confetti();
			break;
		case 'h':
			// moving rainbw
			sinelon();
			break;
		case 'i':
			// moving rainbw
			bpm();
			break;
		case 'j':
			// moving rainbw
			juggle();
			break;
        case 'k':
            flash_all();
            break;
	}

	spaceOutPixels();
	FastLED.show();

	gHue += speed_div;
	delay(10);

	/*
	   counter ++;

	   if (counter > speed_div) {
	   gHue++;
	   counter = 0;
	   }
	 */
}

void spaceOutPixels() {
	for (uint16_t i = 0; i < NUM_LEDS; i++) {
		all_leds[2*i] = half_leds[i];
	}
}

void rainbow()
{
	// FastLED's built-in rainbow generator
	fill_rainbow( half_leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
	// built-in FastLED rainbow, plus some random sparkly glitter
	rainbow();
	addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
	if( random8() < chanceOfGlitter) {
		half_leds[ random16(NUM_LEDS) ] += CRGB::White;
	}
}

void confetti()
{
	// random colored speckles that blink in and fade smoothly
	fadeToBlackBy( half_leds, NUM_LEDS, 10);
	int pos = random16(NUM_LEDS);
	half_leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
	// a colored dot sweeping back and forth, with fading trails
	fadeToBlackBy( half_leds, NUM_LEDS, 20);
	int pos = beatsin16( 13, 0, NUM_LEDS-1 );
	half_leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
	// colored stripes pulsing at a defined Beats-Per-Minute (BPM)
	uint8_t BeatsPerMinute = 62;
	CRGBPalette16 palette = PartyColors_p;
	uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
	for( int i = 0; i < NUM_LEDS; i++) { //9948
		half_leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
	}
}

void juggle() {
	// eight colored dots, weaving in and out of sync with each other
	fadeToBlackBy( half_leds, NUM_LEDS, 20);
	byte dothue = 0;
	for( int i = 0; i < 8; i++) {
		half_leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
		dothue += 32;
	}
}

void flash_all() {
    int treshold = 100;
    if (gHue > treshold) {
        gHue = 0;
        fill_solid(half_leds, NUM_LEDS, CRGB::Red);
    } else {
        fill_solid(half_leds, NUM_LEDS, CRGB::Black);
    }
}
