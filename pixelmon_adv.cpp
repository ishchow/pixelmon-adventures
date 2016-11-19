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

// extern const int NUM_PIXELMON_TYPES;
extern pixelmon_type allPixelmon[];

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

bool execAttack(pixelmon *attacker, pixelmon *victim, int attack_id) {
	int prev_health = victim->health;
	int hit_probability = random(0, 101);
	if (hit_probability > 25) {
		victim->health -= allPixelmon[attacker->pixelmon_id].attacks[attack_id].dmg;
	}
	if (victim->health < prev_health) return true;
	else return false;
}

void hitAnimation(pixelmon *injured, int16_t injured_x, int16_t injured_y, uint16_t bmp_color, uint16_t bg_color) {
	delay(500);
	erasePixelmon(injured, injured_x, injured_y, bg_color);
	delay(500);
	drawPixelmon(injured, injured_x, injured_y, bmp_color);
}

void deathAnimation(pixelmon *killed, int16_t killed_x, int16_t killed_y, uint16_t bg_color) {
	drawPixelmon(killed, killed_x, killed_y, ST7735_RED);
	erasePixelmon(killed, killed_x, killed_y, bg_color);
	delay(500);
}

// TODO: Finish function
void dodgeAnimation(pixelmon *px, int16_t x, int16_t y, uint16_t bmp_color,
					uint16_t bg_color, bool isPlayer)
{
	int start_x = x;
	int last_x = x;
	const int DODGE_LENGTH = 10;
	const int MOVE_SPEED = 2;
	int direction = 1;  // Direction relative to pokemon
						// Forward: 1, Reverse: -1

	if (isPlayer) {
		while (true) {
			if (direction == 1) x = constrain(x+MOVE_SPEED, start_x, start_x + DODGE_LENGTH);
			else if (direction == -1) x = constrain(x-MOVE_SPEED, start_x, start_x + DODGE_LENGTH);

			if (x != last_x) {
				erasePixelmon(px, last_x, y, ST7735_BLACK);
				delay(50);
				drawPixelmon(px, x, y, ST7735_WHITE);
				last_x = x;
			}

			if (x == start_x + DODGE_LENGTH) direction = -1;
			if (x == start_x && direction == -1) break;
		}
	} else {
		while (true) {
			if (direction == 1) x = constrain(x-MOVE_SPEED, start_x - DODGE_LENGTH, start_x);
			else if (direction == -1) x = constrain(x+MOVE_SPEED, start_x - DODGE_LENGTH, start_x);

			if (x != last_x) {
				erasePixelmon(px, last_x, y, ST7735_BLACK);
				delay(50);
				drawPixelmon(px, x, y, ST7735_WHITE);
				last_x = x;
			}

			if (x == start_x - DODGE_LENGTH) direction = -1;
			if (x == start_x && direction == -1) break;
		}
	}
	delay(500);
}

void generatePixelmon(pixelmon *px) {
		px->pixelmon_id = random(2);
		px->health = random(75, 100);
		px->level = random(1, 11);
		px->xp = 0;
}

void battleMode(pixelmon *player, pixelmon *wild) {
	int player_x = 0, player_y = 32;
	int wild_x = (TFT_WIDTH - 1) - 32, wild_y = 0;
	drawPixelmon(player, player_x, player_y, ST7735_WHITE);
	drawPixelmon(wild, wild_x, wild_y, ST7735_WHITE);

	bool playerTurn = true;
	while (player->health > 0 && wild->health > 0) {
		int attack_id = random(4);
		if (playerTurn) { // Player
			Serial.print("Player attacks with: ");
			Serial.println(allPixelmon[player->pixelmon_id].attacks[attack_id].name);
			Serial.println();
			bool wild_hit = execAttack(player, wild, attack_id);
			if (wild_hit) hitAnimation(wild, wild_x, wild_y, ST7735_WHITE, ST7735_BLACK);
			else dodgeAnimation(wild, wild_x, wild_y, ST7735_WHITE, ST7735_BLACK, false);
			playerTurn = false;
			delay(500);
		} else { // Wild Pokemon
			Serial.print("Wild attacks with: ");
			Serial.println(allPixelmon[wild->pixelmon_id].attacks[attack_id].name);
			Serial.println();
			bool player_hit = execAttack(wild, player, attack_id);
			if (player_hit) hitAnimation(player, player_x, player_y, ST7735_WHITE, ST7735_BLACK);
			else dodgeAnimation(player, player_x, player_y, ST7735_WHITE, ST7735_BLACK, true);
			playerTurn = true;
			// delay(500);
		}

		if (wild->health < 0) {
			Serial.println("Winner: "); Serial.println("Player\n");
			deathAnimation(wild, wild_x, wild_y, ST7735_BLACK);
		} else if (player->health < 0) {
			Serial.println("Winner: "); Serial.println("Wild\n");
			deathAnimation(player, player_x, player_y, ST7735_BLACK);
		}
	}

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

	pixelmon pl; // Player
	generatePixelmon(&pl);
	pixelmon wd; // Wild
	generatePixelmon(&wd);
	battleMode(&pl, &wd);

	// pixelmon px;
	// generatePixelmon(&px);
	// int player_x = 0, player_y = 32;
	// int wild_x = (TFT_WIDTH - 1) - 32, wild_y = 0;

	// drawPixelmon(&px, player_x, player_y, ST7735_WHITE);
	// dodgeAnimation(&px, player_x, player_y, ST7735_WHITE, ST7735_BLACK, true);

	// drawPixelmon(&px, wild_x, wild_y, ST7735_WHITE);
	// dodgeAnimation(&px, wild_x, wild_y, ST7735_WHITE, ST7735_BLACK, false);

	while (true) {}

    Serial.end();
    return 0;
}
