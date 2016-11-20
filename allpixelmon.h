#ifndef ALLPIXELMON_H
#define ALLPIXELMON_H

#include <Arduino.h>
#include <avr/pgmspace.h>

typedef struct {
    char* name;
    int8_t dmg;
} fight_move;

typedef struct {
    char *name;
    const uint8_t *bitmap;
    int8_t bitmap_width;
    int8_t bitmap_height;
    fight_move attacks[4];
} pixelmon_type;

typedef struct {
    int8_t pixelmon_id;
    int8_t health;
    int8_t level;
    int8_t xp;
} pixelmon;

const int NUM_PIXELMON_TYPES = 2;

void loadAllPixelmon();

#endif
