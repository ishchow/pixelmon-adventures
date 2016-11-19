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

void drawPixelmon(pixelmon *px, int16_t x, int16_t y, uint16_t bmp_color) {
	tft.drawBitmap(x, y, allPixelmon[px->pixelmon_id].bitmap,
				   allPixelmon[px->pixelmon_id].bitmap_width,
				   allPixelmon[px->pixelmon_id].bitmap_height, bmp_color);
}

void erasePixelmon(pixelmon *px, int16_t x, int16_t y, uint16_t bg_color) {
	tft.fillRect(x, y,
				 allPixelmon[px->pixelmon_id].bitmap_width,
				 allPixelmon[px->pixelmon_id].bitmap_height, bg_color);
}

bool pixelmonEqual(pixelmon *px1, pixelmon *px2) {
	if (px1->pixelmon_id != px2->pixelmon_id) return false;
	else if (px1->health != px2->health) return false;
	else if (px1->level != px2->level) return false;
	else if (px1->xp != px2->xp) return false;
	else return true;
}

void execAttack(pixelmon *attacker, pixelmon *victim, int attack_id) {
	victim->health -= allPixelmon[attacker->pixelmon_id].attacks[attack_id].dmg;
}

void hitAnimation(pixelmon *injured, int16_t injured_x, int16_t injured_y, uint16_t bmp_color, uint16_t bg_color) {
	delay(500);
	erasePixelmon(injured, injured_x, injured_y, bg_color);
	delay(500);
	drawPixelmon(injured, injured_x, injured_y, bmp_color);
}

void deathAnimation(pixelmon *injured, int16_t injured_x, int16_t injured_y, uint16_t bg_color) {
	drawPixelmon(injured, injured_x, injured_y, ST7735_RED);
	erasePixelmon(injured, injured_x, injured_y, bg_color);
	delay(500);
}

// TODO: Finish function
void dodgeAnimation(pixelmon *px, int16_t x, int16_t y, uint16_t bmp_color, uint16_t bg_color, uint8_t screen_side) {
	// screen_side: -1 (Left), 1 (Right)
	const int DODGE_LENGTH = 10;
	delay(500);
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

	pixelmon s = {0, 100, 50, 0}; // Yours
	pixelmon b = {1, 100, 50, 0}; // Wild

	drawPixelmon(&s, 0, 32, ST7735_WHITE);
	drawPixelmon(&b, TFT_WIDTH - 32, 0, ST7735_WHITE);

	hitAnimation(&s, 0, 32, ST7735_WHITE, ST7735_BLACK);

	// int turn = 1;
	// while (s.health > 0 && b.health > 0) {
	// 	int turn_id = random(4);
	// 	if (turn == 1) { // Player
	// 		execAttack(&s, &b, turn_id);
	// 		turn = 2;
	// 	} else { // Wild Pokemon
	// 		execAttack(&b, &s, turn_id);
	// 		turn = 1;
	// 	}
	//
	// 	if (s.health < 0) {
	// 		Serial.println("Winner: "); Serial.println(allPixelmon[b.pixelmon_id].name);
	// 		Serial.println();
	// 	} else if (b.health < 0) {
	// 		Serial.println("Winner: "); Serial.println(allPixelmon[s.pixelmon_id].name);
	// 		Serial.println();
	// 	}
	// }

    Serial.end();
    return 0;
}
