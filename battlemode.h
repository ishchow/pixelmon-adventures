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

void generatePixelmon(pixelmon *px);

bool pixelmonEqual(pixelmon *px1, pixelmon *px2);

void printPixelmon(pixelmon *px);

void drawPixelmon(pixelmon *px, int16_t x, int16_t y, uint16_t bmp_color);

void erasePixelmon(int16_t x, int16_t y, uint16_t bg_color);

bool execAttack(pixelmon *attacker, pixelmon *victim, int attack_id);

void hitAnimation(pixelmon *injured, int16_t injured_x, int16_t injured_y, uint16_t bmp_color, uint16_t bg_color);

void deathAnimation(pixelmon *killed, int16_t killed_x, int16_t killed_y, uint16_t bg_color);

void dodgeAnimation(pixelmon *px, int16_t x, int16_t y, uint16_t bmp_color,
					uint16_t bg_color, char attacked_pxm);

void displayPixelmonStats(pixelmon *player_pxm, pixelmon *wild_pxm);

void updateHealth(pixelmon *player_pxm, pixelmon *wild_pxm, char hit_pxm);

void displayFightMenu(pixelmon *player_pxm, int selected_attack);

void eraseMenu();

void updateFightMenu(pixelmon *player_pxm, int selected_attack, int last_selected_attack);

void eraseDisplayArea();

void showMessage(char* message);

void displayMoveStats(pixelmon *player_pxm, int selected_attack);

void updateMoveStats(pixelmon *player_pxm, int selected_attack);

void fightMode(pixelmon *player_pxm, int player_pxm_x, int player_pxm_y,
			   pixelmon *wild_pxm, int wild_pxm_x, int wild_pxm_y,
			   int *selected_attack, int *last_selected_attack, char* message);

void displayBattleMenu(const char *options[], int selected_option);

void updateBattleMenu(const char *options[], int selected_option, int last_selected_option);

void battleMode(pixelmon *player_pxm, pixelmon *wild_pxm);

#endif
