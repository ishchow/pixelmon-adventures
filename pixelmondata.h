#ifndef PIXELMONDATA_H
#define PIXELMONDATA_H

#include <Arduino.h>
#include <avr/pgmspace.h>

// attack names and damage
typedef struct {
    char* name;
    uint16_t dmg;
} fight_move;

// types of pixelmon
typedef struct {
    char *name;
    const uint8_t *bitmap;
    fight_move attacks[4];
} pixelmon_type;

// each individual pixelmon
typedef struct {
    int pixelmon_id;
    int health;
    int level;
    int xp;
} pixelmon;

const int NUM_PIXELMON_TYPES = 2; // total number of pixelmon
const int MAX_OWNED = 6; // max num of pixelmon player can have
// pixelmon.bmps are all 32x32
const int16_t PXM_BMP_WIDTH = 32;
const int16_t PXM_BMP_HEIGHT = 32;

void loadAllPixelmon();

#endif
