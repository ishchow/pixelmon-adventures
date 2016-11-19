#ifndef ALLPIXELMON_H
#define ALLPIXELMON_H

#include <Arduino.h>
#include <avr/pgmspace.h>

typedef struct {
    char* name;
    int dmg;
} fight_move;

typedef struct {
    char *name;
    const uint8_t *bitmap;
    int bitmap_width;
    int bitmap_height;
    fight_move attacks[4];
} pixelmon_type;

typedef struct {
    int pixelmon_id;
    int health;
    int level;
    int xp;
} pixelmon;

void loadAllPixelmon();

#endif
