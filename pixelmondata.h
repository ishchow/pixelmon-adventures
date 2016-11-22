#ifndef PIXELMONDATA_H
#define PIXELMONDATA_H

#include <Arduino.h>
#include <avr/pgmspace.h>

typedef struct {
    char* name;
    uint16_t dmg;
} fight_move;

typedef struct {
    char *name;
    const uint8_t *bitmap;
    fight_move attacks[4];
} pixelmon_type;

typedef struct {
    int pixelmon_id;
    int health;
    int level;
    int xp;
} pixelmon;

const int NUM_PIXELMON_TYPES = 2;
const int MAX_OWNED = 6;
const int16_t PXM_BMP_WIDTH = 32;
const int16_t PXM_BMP_HEIGHT = 32;

void loadAllPixelmon();

#endif
