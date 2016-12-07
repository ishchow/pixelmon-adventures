#ifndef PIXELMONDATA_H
#define PIXELMONDATA_H

#include <Arduino.h>
#include <avr/pgmspace.h>

// attack names and damage
typedef struct {
    char* name; // names longer than 10 do not display entire string in fight menu
    uint16_t dmg; // Damage: dmg >= 1
    uint8_t acc; // Accuracy: 1 <= acc <= 100
} fight_move;

// types of pixelmon
typedef struct {
    char *name; //names should be 1-10 char long
    const uint8_t *bitmap;
    fight_move attacks[4];
    int rarity;
} pixelmon_type;

// each individual pixelmon
typedef struct {
    int pixelmon_id; // 0 <= pixelmon_id <= NUM_PIXELMON_TYPES - 1
    int health; // 0 <= health <= 9999
    int level; // 0 <= level <= 9999
    int xp; // 0 <= xp <= 9999
} pixelmon;

const int NUM_PIXELMON_TYPES = 9; // total number of pixelmon
const int MAX_OWNED = 6; // max num of pixelmon player can have
// pixelmon.bmps are all 32x32
const int16_t PXM_BMP_WIDTH = 32;
const int16_t PXM_BMP_HEIGHT = 32;
extern pixelmon_type allPixelmon[NUM_PIXELMON_TYPES]; // defined in "pixelmondata.cpp"

void loadAllPixelmon();

#endif
