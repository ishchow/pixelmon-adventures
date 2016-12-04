#include "pixelmondata.h"

// change NUM_PIXELMON_TYPES in pixelmondata.h when adding or deleting pixelmon
pixelmon_type allPixelmon[NUM_PIXELMON_TYPES];

const uint8_t turtle[] PROGMEM = {
  // 'turtle' bmp
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xfc, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x03, 0xff, 0x80, 0x00, 0x03, 0xff, 0x80, 0x00,
  0x03, 0xf7, 0xc0, 0x00, 0x07, 0xf2, 0xc0, 0x00, 0x03, 0xf2, 0xc0, 0x00, 0x07, 0xf3, 0xc0, 0x00,
  0x07, 0xff, 0xc0, 0x00, 0x01, 0x81, 0x98, 0x00, 0x32, 0x03, 0x74, 0x00, 0x7d, 0x8e, 0x64, 0x00,
  0x3f, 0x87, 0xf8, 0x00, 0x1f, 0x7e, 0xf0, 0x00, 0x02, 0xfe, 0xe0, 0x00, 0x01, 0xfe, 0x01, 0xf8,
  0x01, 0xff, 0xa1, 0xf8, 0x00, 0xbb, 0xa3, 0xac, 0x01, 0x93, 0x93, 0x7c, 0x01, 0xff, 0xd3, 0x78,
  0x06, 0xff, 0xeb, 0xb0, 0x0e, 0x9f, 0xf7, 0xc0, 0x07, 0xf0, 0xf0, 0x00, 0x07, 0x9c, 0xf8, 0x00,
  0x07, 0x80, 0xf8, 0x00, 0x05, 0x80, 0x7c, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t veggiesaur[] PROGMEM = {
  // 'veggiesaur' bmp
  0x00, 0x00, 0x02, 0x80, 0x00, 0x00, 0x05, 0xe0, 0x00, 0x00, 0x1d, 0xa0, 0x00, 0x01, 0xc1, 0x38,
  0x00, 0x04, 0x02, 0x78, 0x00, 0x08, 0x70, 0x78, 0x00, 0x10, 0x80, 0x38, 0x08, 0x11, 0xf8, 0x3c,
  0x04, 0x76, 0x08, 0x7e, 0x00, 0x00, 0x08, 0x7e, 0x00, 0x00, 0x08, 0xff, 0x00, 0x00, 0x0f, 0xff,
  0x00, 0x02, 0x0f, 0xff, 0x00, 0x01, 0x07, 0xff, 0x10, 0x08, 0x85, 0xff, 0x10, 0x1c, 0x85, 0xfe,
  0x00, 0x04, 0x84, 0xfe, 0x80, 0x00, 0x05, 0xfc, 0x80, 0x80, 0x05, 0xf8, 0x00, 0x00, 0x49, 0xd8,
  0x4c, 0x77, 0x80, 0x08, 0x2f, 0xff, 0x02, 0xc4, 0x17, 0xfe, 0x03, 0xb4, 0x01, 0x17, 0x8d, 0xf2,
  0x03, 0xfe, 0x8d, 0x62, 0x19, 0xf8, 0x85, 0x02, 0x11, 0xc0, 0x81, 0x86, 0x01, 0x3f, 0xc1, 0xcc,
  0x01, 0x0f, 0xe1, 0xd4, 0x16, 0x07, 0xaa, 0x78, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t banana[] PROGMEM = {
  // 'banana'
  0x00, 0x03, 0x80, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x06, 0x60, 0x00,
  0x00, 0x04, 0x30, 0x00, 0x00, 0x04, 0x30, 0x00, 0x00, 0x04, 0x18, 0x00, 0x00, 0x04, 0x08, 0x00,
  0x00, 0x1f, 0x8c, 0x00, 0x00, 0x28, 0x44, 0x00, 0x00, 0x3c, 0x44, 0x00, 0x00, 0x28, 0x44, 0x00,
  0x00, 0x1f, 0x94, 0x00, 0x00, 0x08, 0x74, 0x00, 0x00, 0x0f, 0xf4, 0x00, 0x00, 0x07, 0xe4, 0x00,
  0x00, 0x07, 0xe4, 0x00, 0x00, 0x07, 0xc4, 0x00, 0x00, 0x0e, 0x04, 0x00, 0x03, 0x88, 0x0d, 0xc0,
  0x0c, 0xd8, 0x0b, 0x30, 0x0c, 0x50, 0x1a, 0x30, 0x0c, 0xd0, 0x13, 0x30, 0x03, 0xb0, 0x33, 0xc0,
  0x00, 0xa0, 0x76, 0x00, 0x00, 0xf3, 0xdc, 0x00, 0x01, 0xff, 0xf8, 0x00, 0x01, 0xff, 0xf8, 0x00,
  0x03, 0x80, 0x3c, 0x00, 0x0c, 0x71, 0xe2, 0x00, 0x0c, 0x11, 0x82, 0x00, 0x03, 0xf1, 0xfc, 0x00,
};

const uint8_t the_donald[] PROGMEM = {
  // 'the_donald'
  0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x80, 0x01, 0xb0, 0x01, 0x7f, 0x9e, 0x70,
  0x03, 0x7f, 0xde, 0x60, 0x04, 0xff, 0xe0, 0x60, 0x04, 0x10, 0x1f, 0xc0, 0x07, 0x20, 0x02, 0x00,
  0x04, 0x4f, 0x9e, 0x00, 0x06, 0x47, 0x9e, 0x00, 0x07, 0xc1, 0x0a, 0x00, 0x07, 0xc0, 0x00, 0x00,
  0x07, 0x80, 0x00, 0x00, 0x07, 0x00, 0x10, 0x00, 0x07, 0x81, 0xf0, 0x00, 0x03, 0x81, 0xf0, 0x00,
  0x03, 0x81, 0x70, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x8c, 0x08, 0x00, 0x00, 0x43, 0xf0, 0x00,
  0x00, 0x1f, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x3a, 0x80, 0x00, 0x00, 0x7a, 0x80, 0x00,
  0x00, 0x7f, 0xe0, 0x00, 0x00, 0x61, 0xe0, 0x00, 0x00, 0x63, 0xe0, 0x00, 0x00, 0x63, 0xc0, 0x00,
  0x00, 0x3f, 0x80, 0x00, 0x00, 0x39, 0x80, 0x00, 0x00, 0x39, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t pixelcat[] PROGMEM = {
  // 'pixelcat' bmp
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc0, 0xff, 0xfb, 0x39, 0xa0, 0xff, 0xfc, 0xc6, 0x60,
  0x0f, 0x84, 0xc6, 0x68, 0x00, 0x23, 0x3d, 0xa4, 0xff, 0xdb, 0x3c, 0xe4, 0xff, 0xc4, 0xc4, 0x04,
  0x0f, 0x8c, 0xc8, 0x0a, 0xf0, 0x7c, 0xc8, 0x02, 0xff, 0xfb, 0x38, 0x02, 0xff, 0xff, 0x38, 0x00,
  0x0f, 0x8b, 0xff, 0xf8, 0xf0, 0x7c, 0xc1, 0xb0, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t braveman[] PROGMEM = {
  // 'braveman' bmp
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x30, 0x08, 0x00, 0x00, 0x3f, 0xe8, 0x00, 0x00, 0x3f, 0xf8, 0x00, 0x00, 0x3f, 0xf8, 0x00,
  0x00, 0x3f, 0xf8, 0x00, 0x00, 0x31, 0x98, 0x00, 0x00, 0x31, 0x98, 0x00, 0x00, 0x3f, 0xf8, 0x00,
  0x00, 0x3f, 0xf8, 0x00, 0x00, 0x18, 0x30, 0x00, 0x00, 0x1b, 0xb0, 0x00, 0x00, 0x7c, 0x6e, 0x00,
  0x00, 0xe3, 0x8e, 0x00, 0x01, 0xce, 0xe3, 0x80, 0x01, 0x9f, 0xe1, 0x80, 0x01, 0x9d, 0x61, 0x80,
  0x01, 0xf0, 0x0f, 0x80, 0x01, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t voidinvader[] PROGMEM = {
  // 'voidinvader'
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x03, 0x80, 0x01, 0xc0, 0x03, 0x80,
  0x01, 0xc0, 0x03, 0x80, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x30, 0x0c, 0x00, 0x01, 0xff, 0xff, 0x80,
  0x01, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0x80, 0x07, 0xcf, 0xf3, 0xe0, 0x07, 0xcf, 0xf3, 0xe0,
  0x3f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xfc, 0x39, 0xff, 0xff, 0x9c,
  0x39, 0xff, 0xff, 0x9c, 0x39, 0xc0, 0x03, 0x9c, 0x39, 0xc0, 0x03, 0x9c, 0x39, 0xc0, 0x03, 0x9c,
  0x00, 0x3e, 0x7c, 0x00, 0x00, 0x3e, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t superplumber[] PROGMEM = {
  // 'superplumber'
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x70, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00,
  0x01, 0x07, 0xe0, 0x00, 0x02, 0x3f, 0xf0, 0x00, 0x02, 0x7f, 0xf0, 0x00, 0x02, 0x70, 0x60, 0x00,
  0x07, 0xc2, 0xc0, 0x00, 0x04, 0xc2, 0xe0, 0x00, 0x04, 0xf0, 0x10, 0x00, 0x04, 0x44, 0x10, 0x00,
  0x02, 0x1f, 0x38, 0x00, 0x01, 0x83, 0xe0, 0x00, 0x03, 0xc0, 0x80, 0x00, 0x04, 0x7f, 0x00, 0x00,
  0x04, 0x79, 0xa0, 0x00, 0x08, 0x9c, 0xd0, 0x00, 0x08, 0x84, 0xd0, 0x00, 0x09, 0x02, 0x50, 0x00,
  0x09, 0x02, 0x60, 0x00, 0x0c, 0x85, 0xa0, 0x00, 0x07, 0xf9, 0xa0, 0x00, 0x07, 0xff, 0xe0, 0x00,
  0x03, 0xff, 0xf0, 0x00, 0x05, 0xff, 0xc8, 0x00, 0x05, 0xfb, 0x88, 0x00, 0x04, 0xf1, 0x10, 0x00,
  0x06, 0xf9, 0x30, 0x00, 0x02, 0x19, 0x20, 0x00, 0x01, 0xf8, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t richoldguy[] PROGMEM = {
  // 'richoldguy'
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x63, 0x80, 0x00, 0x00, 0xb1, 0x80, 0xe0,
  0x00, 0x7e, 0xd0, 0x40, 0x00, 0x1f, 0xf0, 0xc0, 0x00, 0x1f, 0xe0, 0xc0, 0x00, 0xd8, 0x00, 0x20,
  0x00, 0x24, 0x70, 0x50, 0x00, 0xa4, 0x90, 0x10, 0x00, 0x00, 0xf0, 0xa8, 0x4e, 0x26, 0x07, 0x00,
  0x0f, 0xf0, 0x1f, 0xf8, 0x77, 0xfb, 0xbf, 0x84, 0x03, 0xff, 0xbc, 0x08, 0x00, 0xfe, 0x30, 0x08,
  0x00, 0x3c, 0x20, 0x06, 0x00, 0x38, 0x60, 0x00, 0x00, 0x79, 0x60, 0x02, 0x00, 0xfb, 0x70, 0x00,
  0x00, 0xf4, 0xf0, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x80, 0x88, 0x00, 0x06, 0x04, 0x88, 0x00,
  0x06, 0x18, 0x00, 0x00, 0x01, 0xc0, 0xfc, 0x00, 0x01, 0x80, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

// function assigns data to allPixelmon of the pixelmon_type data type
void loadAllPixelmon() {
  allPixelmon[0].name = "turtle";
  allPixelmon[0].bitmap = turtle;
  allPixelmon[0].attacks[0].name = "headbop";
  allPixelmon[0].attacks[0].dmg = 6; allPixelmon[0].attacks[0].acc = 95;
  allPixelmon[0].attacks[1].name = "spin";
  allPixelmon[0].attacks[1].dmg = 15; allPixelmon[0].attacks[1].acc = 60;
  allPixelmon[0].attacks[2].name = "squirt";
  allPixelmon[0].attacks[2].dmg = 12; allPixelmon[0].attacks[2].acc = 65;
  allPixelmon[0].attacks[3].name = "tail wag";
  allPixelmon[0].attacks[3].dmg = 10; allPixelmon[0].attacks[3].acc = 70;
  allPixelmon[0].rarity = 1;

  allPixelmon[1].name = "veggiesaur";
  allPixelmon[1].bitmap = veggiesaur;
  allPixelmon[1].attacks[0].name = "scream";
  allPixelmon[1].attacks[0].dmg = 500; allPixelmon[1].attacks[0].acc = 100;
  allPixelmon[1].attacks[1].name = "pounce";
  allPixelmon[1].attacks[1].dmg = 11; allPixelmon[1].attacks[1].acc = 65;
  allPixelmon[1].attacks[2].name = "chomp";
  allPixelmon[1].attacks[2].dmg = 12; allPixelmon[1].attacks[2].acc = 50;
  allPixelmon[1].attacks[3].name = "salad toss";
  allPixelmon[1].attacks[3].dmg = 20; allPixelmon[1].attacks[3].acc = 30;
  allPixelmon[1].rarity = 1;

  allPixelmon[2].name = "banana";
  allPixelmon[2].bitmap = banana;
  allPixelmon[2].attacks[0].name = "peanut";
  allPixelmon[2].attacks[0].dmg = 4; allPixelmon[2].attacks[0].acc = 98;
  allPixelmon[2].attacks[1].name = "butter";
  allPixelmon[2].attacks[1].dmg = 12; allPixelmon[2].attacks[1].acc = 60;
  allPixelmon[2].attacks[2].name = "jelly";
  allPixelmon[2].attacks[2].dmg = 15; allPixelmon[2].attacks[2].acc = 50;
  allPixelmon[2].attacks[3].name = "time";
  allPixelmon[2].attacks[3].dmg = 19; allPixelmon[2].attacks[3].acc = 40;
  allPixelmon[2].rarity = 1;

  allPixelmon[3].name = "Donald";
  allPixelmon[3].bitmap = the_donald;
  allPixelmon[3].attacks[0].name = "grope";
  allPixelmon[3].attacks[0].dmg = 2; allPixelmon[3].attacks[0].acc = 98;
  allPixelmon[3].attacks[1].name = "deport";
  allPixelmon[3].attacks[1].dmg = 4; allPixelmon[3].attacks[1].acc = 95;
  allPixelmon[3].attacks[2].name = "build a wall";
  allPixelmon[3].attacks[2].dmg = 20; allPixelmon[3].attacks[2].acc = 50;
  allPixelmon[3].attacks[3].name = "make 'murica great again";
  allPixelmon[3].attacks[3].dmg = 50; allPixelmon[3].attacks[3].acc = 5;
  allPixelmon[3].rarity = 2;

  allPixelmon[4].name = "pixelcat";
  allPixelmon[4].bitmap = pixelcat;
  allPixelmon[4].attacks[0].name = "nyan";
  allPixelmon[4].attacks[0].dmg = 19; allPixelmon[4].attacks[0].acc = 40;
  allPixelmon[4].attacks[1].name = "catscratch";
  allPixelmon[4].attacks[1].dmg = 10; allPixelmon[4].attacks[1].acc = 95;
  allPixelmon[4].attacks[2].name = "rainbow";
  allPixelmon[4].attacks[2].dmg = 9; allPixelmon[4].attacks[2].acc = 20;
  allPixelmon[4].attacks[3].name = "poptart";
  allPixelmon[4].attacks[3].dmg = 15; allPixelmon[4].attacks[3].acc = 50;
  allPixelmon[4].rarity = 2;

  allPixelmon[5].name = "supaplumba";
  allPixelmon[5].bitmap = superplumber;
  allPixelmon[5].attacks[0].name = "jump";
  allPixelmon[5].attacks[0].dmg = 5; allPixelmon[5].attacks[0].acc = 95;
  allPixelmon[5].attacks[1].name = "fireball";
  allPixelmon[5].attacks[1].dmg = 14; allPixelmon[5].attacks[1].acc = 80;
  allPixelmon[5].attacks[2].name = "shrooms";
  allPixelmon[5].attacks[2].dmg = 20; allPixelmon[5].attacks[2].acc = 50;
  allPixelmon[5].attacks[3].name = "kill goomba";
  allPixelmon[5].attacks[3].dmg = 1; allPixelmon[5].attacks[3].acc = 100;
  allPixelmon[5].rarity = 1;

  allPixelmon[6].name = "richoldguy";
  allPixelmon[6].bitmap = richoldguy;
  allPixelmon[6].attacks[0].name = "shoe";
  allPixelmon[6].attacks[0].dmg = 3; allPixelmon[6].attacks[0].acc = 90;
  allPixelmon[6].attacks[1].name = "hat";
  allPixelmon[6].attacks[1].dmg = 4; allPixelmon[6].attacks[1].acc = 85;
  allPixelmon[6].attacks[2].name = "monocle";
  allPixelmon[6].attacks[2].dmg = 10; allPixelmon[6].attacks[2].acc = 75;
  allPixelmon[6].attacks[3].name = "monopoly";
  allPixelmon[6].attacks[3].dmg = 21; allPixelmon[6].attacks[3].acc = 45;
  allPixelmon[6].rarity = 1;

  allPixelmon[7].name = "braveman";
  allPixelmon[7].bitmap = braveman;
  allPixelmon[7].attacks[0].name = "B*Tarang";
  allPixelmon[7].attacks[0].dmg = 21; allPixelmon[7].attacks[0].acc = 90;
  allPixelmon[7].attacks[1].name = "punch";
  allPixelmon[7].attacks[1].dmg = 13; allPixelmon[7].attacks[1].acc = 100;
  allPixelmon[7].attacks[2].name = "I'm B*tman";
  allPixelmon[7].attacks[2].dmg = 100; allPixelmon[7].attacks[2].acc = 50;
  allPixelmon[7].attacks[3].name = "cape";
  allPixelmon[7].attacks[3].dmg = 16; allPixelmon[7].attacks[3].acc = 95;
  allPixelmon[7].rarity = 2;

  allPixelmon[8].name = "invader";
  allPixelmon[8].bitmap = voidinvader;
  allPixelmon[8].attacks[0].name = "laser";
  allPixelmon[8].attacks[0].dmg = 18; allPixelmon[8].attacks[0].acc = 50;
  allPixelmon[8].attacks[1].name = "left";
  allPixelmon[8].attacks[1].dmg = 8; allPixelmon[8].attacks[1].acc = 90;
  allPixelmon[8].attacks[2].name = "right";
  allPixelmon[8].attacks[2].dmg = 8; allPixelmon[8].attacks[2].acc = 90;
  allPixelmon[8].attacks[3].name = "move down";
  allPixelmon[8].attacks[3].dmg = 13; allPixelmon[8].attacks[3].acc = 70;
  allPixelmon[8].rarity = 1;
}
