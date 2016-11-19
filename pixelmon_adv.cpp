/*
ard_template.cpp

Authors:
Ishaat Chowdhury	Section: EA1
Sam Wu              Section: EA2

Description:

*/

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

#include "lcd_image.h"
#include <avr/pgmspace.h> // For PROGMEM
#include "allpixelmon.h"

// Display pins:
// standard U of A library settings, assuming Atmel Mega SPI pins
#define SD_CS    5 // Chip select line for SD card
#define TFT_CS   6 // Chip select line for TFT display
#define TFT_DC   7 // Data/command line for TFT
#define TFT_RST  8 // Reset Line for TFT (or connect to +5V)

// #define JOY_VERT_ANALOG 0
// #define JOY_HORIZ_ANALOG 1
// #define JOY_DEADZONE 64
// #define JOY_CENTRE 512
// #define JOY_SPEED 5
// #define JOY_SEL 9

// #define MILLIS_PER_FRAME 50 // 20fps
// #define STEPS_PER_PIXEL 64

#define TFT_WIDTH 128
#define TFT_HEIGHT 160

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
Sd2Card card;

extern pixelmon_type allPixelmon[];
// extern const int NUM_PIXELMON_TYPES;
const int MAX_OWNED = 6;
pixelmon ownedPixelmon[MAX_OWNED];

void drawPixelmon(int pxlmon_id, int16_t x, int16_t y, uint16_t color) {
	tft.drawBitmap(0, 0, allPixelmon[pxlmon_id].bitmap,
				   allPixelmon[pxlmon_id].bitmap_width, allPixelmon[pxlmon_id].bitmap_height,
				   color);
}

void erasePixelmon(int pxlmon_id, int16_t x, int16_t y, uint16_t color) {
	tft.fillRect(x, y,
				 allPixelmon[pxlmon_id].bitmap_width, allPixelmon[pxlmon_id].bitmap_height,
				 color);
}

bool pixelmonEqual(pixelmon px1, pixelmon px2) {
	if (px1.pixelmon_id != px2.pixelmon_id) return false;
	else if (px1.health != px2.health) return false;
	else if (px1.level != px2.level) return false;
	else if (px1.xp != px2.xp) return false;
	else return true;
}

void setup() {
    init();
    Serial.begin(9600);
	randomSeed(analogRead(7));

    Serial.print("Initializing SD card...");
    if (!SD.begin(SD_CS)) {
        Serial.println("failed!");
        return;
    }

    tft.initR(INITR_BLACKTAB);   // initialize a ST7735R chip, green tab
    tft.fillScreen(ST7735_BLACK);

    Serial.println("OK!");
}

int main() {
    setup();
	loadAllPixelmon();

	// for (int i = 0; i < MAX_OWNED; ++i) {
	// 	pixelmon wild_pixelmon;
	// 	wild_pixelmon.pixelmon_id = random(2);
	// 	wild_pixelmon.health = random(4,7);
	// 	wild_pixelmon.level = random(1, 11);
	// 	wild_pixelmon.xp = 0;
	//
	// 	ownedPixelmon[i] = wild_pixelmon;
	// 	if (!pixelmonEqual(ownedPixelmon[i], wild_pixelmon)) {
	// 		Serial.println("Not Equal!");
	// 	} else {
	// 		Serial.println("Equal!");
	// 	}
	//
	// 	drawPixelmon(wild_pixelmon.pixelmon_id, 0, 0, ST7735_WHITE);
	// 	delay(1000);
	// 	erasePixelmon(wild_pixelmon.pixelmon_id, 0, 0, ST7735_BLACK);
	// }

	// for (int i = 0; i < 2; ++i) {
	// 	for (int j = 0; j < 4; ++j) {
	// 		Serial.println(allPixelmon[i].attacks[j].name);
	// 	}
	// 	Serial.println();
	// }

	pixelmon sqt = {0, 100, 50, 0}; // Yours
	pixelmon bulb = {1, 100, 50, 0}; // Wild

	drawPixelmon()
	int move = 1; // Player: 1, Wild Pokemon: 2
	while (true) {

	}

    Serial.end();
    return 0;
}
