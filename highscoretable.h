#ifndef HIGHSCORETABLE_H
#define HIGHSCORETABLE_H

#include <Arduino.h>
#include <avr/pgmspace.h> // For PROGMEM and F()
#include "EEPROM.h"

// holds information for each player
typedef struct {
    char name[4];
    int score;
} player;

extern int num_scores;

void clearEEPROM();

// Reads raw bytes from EEPROM and displays in serial monitor
// Only displays the desired number of elements
void displayEEPROM(int num_elements);

// Store number of scores in EEPROM
void storeNumScores();

// Store number of scores in EEPROM
void getNumScores();

// Print highscore table to serial-monitor
void tableToSerial();

// Append player name and score to table
void playerToTable(player* current_player);

// Retrieves player name from serial monitor
void getPlayerName(player *current_player);

// Generates specified number of elements and adds them to highscore table
// Can overwrite the previous table if specified
void generateTable(int num_elements, bool overwrite_table);

void highscoreTableToTFT();

#endif
