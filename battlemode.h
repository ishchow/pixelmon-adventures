#ifndef BATTLEMODE_H
#define BATTLEMODE_H

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

#include "lcd_image.h"
#include <avr/pgmspace.h> // For PROGMEM
#include "pixelmondata.h"

// fxn creates a random pixelmon with random stats
void generatePixelmon(pixelmon *px);

// fxn to check if two pixelmon are same - for testing
bool pixelmonEqual(pixelmon *px1, pixelmon *px2);

// fxn to show pixelmon stats on serial-mon - mostly for testing
void printPixelmon(pixelmon *px);

// Checks if all owned pixelmon have no health, returns true if this is the case
bool allOwnedPixelmonDead();

// Reset health to max for all owned pixelmon
void healAllOwnedPixelmon();

void drawPixelmon(pixelmon *px, int16_t x, int16_t y, uint16_t bmp_color);

void erasePixelmon(int16_t x, int16_t y, uint16_t bg_color);

// fxn to calc. dmg, returns if true if attack is succesful
bool execAttack(pixelmon *attacker, pixelmon *victim, int attack_id);

void hitAnimation(pixelmon *injured, int16_t injured_x, int16_t injured_y, uint16_t bmp_color, uint16_t bg_color);

void deathAnimation(pixelmon *killed, int16_t killed_x, int16_t killed_y, uint16_t bg_color);

void dodgeAnimation(pixelmon *px, int16_t x, int16_t y, uint16_t bmp_color,
					uint16_t bg_color, char attacked_pxm);

// print stats of pixelmon to tft in battle
void displayPixelmonStats(pixelmon *player_pxm, pixelmon *wild_pxm);

// display new health on screen
void updateHealth(pixelmon *player_pxm, pixelmon *wild_pxm, char hit_pxm);

// print fight/attack moves, based on position of 1st move
void displayFightMenu(pixelmon *player_pxm, int selected_attack);

// make part (where menus appear) of screen black
void eraseMenu();

// put previous attack in white letters/black bg; put selected_attack in black letters/white bg
void updateFightMenu(pixelmon *player_pxm, int selected_attack, int last_selected_attack);

// make part (where messages appear) of screen black
void eraseDisplayArea();

// display custom text to screen
void showMessage(char* message);

// Shows damage and accuracy of selected (highlighted) move
void displayMoveStats(pixelmon *player_pxm, int selected_attack);

// Displays damage and accuracy of new move (deselect old move, select new move)
void updateMoveStats(pixelmon *player_pxm, int selected_attack);

// fxn for the menu solely for attacks
void fightMode(pixelmon *player_pxm, int player_pxm_x, int player_pxm_y,
			   pixelmon *wild_pxm, int wild_pxm_x, int wild_pxm_y,
			   int *selected_attack, int *last_selected_attack, char* message);

// show menu for fight, flee, capture, swap on screen
void displayBattleMenu(const char *options[], const int num_options, int selected_option);

// highlights selected battle menu option
void updateBattleMenu(const char *options[], const int num_options,
					  int selected_option, int last_selected_option);

// displays list of all owned pixelmon to swap from
void displaySwapMenu(int selected_pxm);

// highlights selected pixelmon in swap menu
void updateSwapMenu(int selected_pxm, int last_selected_pxm,
					pixelmon *player_pxm, pixelmon *last_player_pxm);

// display stats and bitmap of selected pixelmon
void updatePixelmon(int player_pxm_x, int player_pxm_y,
				    pixelmon *player_pxm, pixelmon *last_player_pxm,
						bool isEnemy);

// makes pixelmon get xp and level up
void levelUpPixelmon(pixelmon *player_pxm, char *message);

// fxn for swap pixelmon
void swapMode(pixelmon **player_pxm, int player_pxm_x , int player_pxm_y,
		  	  pixelmon **last_player_pxm, int* selected_pxm, int* last_selected_pxm,
			  	char *message);

// fxn with menu for fight, flee, swap, capture
void battleMode(pixelmon *player_pxm, pixelmon *wild_pxm);

#endif
