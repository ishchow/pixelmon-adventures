#include "pixelmondata.h"

pixelmon_type allPixelmon[NUM_PIXELMON_TYPES];

const uint8_t squirtle[] PROGMEM = {
       // 'squirtle' bmp
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xfc, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x03, 0xff, 0x80, 0x00, 0x03, 0xff, 0x80, 0x00,
   0x03, 0xf7, 0xc0, 0x00, 0x07, 0xf2, 0xc0, 0x00, 0x03, 0xf2, 0xc0, 0x00, 0x07, 0xf3, 0xc0, 0x00,
   0x07, 0xff, 0xc0, 0x00, 0x01, 0x81, 0x98, 0x00, 0x32, 0x03, 0x74, 0x00, 0x7d, 0x8e, 0x64, 0x00,
   0x3f, 0x87, 0xf8, 0x00, 0x1f, 0x7e, 0xf0, 0x00, 0x02, 0xfe, 0xe0, 0x00, 0x01, 0xfe, 0x01, 0xf8,
   0x01, 0xff, 0xa1, 0xf8, 0x00, 0xbb, 0xa3, 0xac, 0x01, 0x93, 0x93, 0x7c, 0x01, 0xff, 0xd3, 0x78,
   0x06, 0xff, 0xeb, 0xb0, 0x0e, 0x9f, 0xf7, 0xc0, 0x07, 0xf0, 0xf0, 0x00, 0x07, 0x9c, 0xf8, 0x00,
   0x07, 0x80, 0xf8, 0x00, 0x05, 0x80, 0x7c, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00,
   };

const uint8_t bulbosaur[] PROGMEM = {
       // 'bulb' bmp
    0x00, 0x00, 0x02, 0x80, 0x00, 0x00, 0x05, 0xe0, 0x00, 0x00, 0x1d, 0xa0, 0x00, 0x01, 0xc1, 0x38,
   	0x00, 0x04, 0x02, 0x78, 0x00, 0x08, 0x70, 0x78, 0x00, 0x10, 0x80, 0x38, 0x08, 0x11, 0xf8, 0x3c,
   	0x04, 0x76, 0x08, 0x7e, 0x00, 0x00, 0x08, 0x7e, 0x00, 0x00, 0x08, 0xff, 0x00, 0x00, 0x0f, 0xff,
   	0x00, 0x02, 0x0f, 0xff, 0x00, 0x01, 0x07, 0xff, 0x10, 0x08, 0x85, 0xff, 0x10, 0x1c, 0x85, 0xfe,
   	0x00, 0x04, 0x84, 0xfe, 0x80, 0x00, 0x05, 0xfc, 0x80, 0x80, 0x05, 0xf8, 0x00, 0x00, 0x49, 0xd8,
   	0x4c, 0x77, 0x80, 0x08, 0x2f, 0xff, 0x02, 0xc4, 0x17, 0xfe, 0x03, 0xb4, 0x01, 0x17, 0x8d, 0xf2,
   	0x03, 0xfe, 0x8d, 0x62, 0x19, 0xf8, 0x85, 0x02, 0x11, 0xc0, 0x81, 0x86, 0x01, 0x3f, 0xc1, 0xcc,
   	0x01, 0x0f, 0xe1, 0xd4, 0x16, 0x07, 0xaa, 0x78, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00
   };

const uint8_t banana [] PROGMEM = {
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

const uint8_t the_donald [] PROGMEM = {
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

// function assigns data to allPixelmon of the pixelmon_type data type
void loadAllPixelmon() {
    allPixelmon[0].name = "squirtle";
    allPixelmon[0].bitmap = squirtle;
    allPixelmon[0].attacks[0].name = "headbutt"; allPixelmon[0].attacks[0].dmg = 6;
    allPixelmon[0].attacks[1].name = "spin"; allPixelmon[0].attacks[1].dmg = 11;
    allPixelmon[0].attacks[2].name = "squirt"; allPixelmon[0].attacks[2].dmg = 12;
    allPixelmon[0].attacks[3].name = "tail wag"; allPixelmon[0].attacks[3].dmg = 13;

    allPixelmon[1].name = "bulbosaur";
    allPixelmon[1].bitmap = bulbosaur;
    allPixelmon[1].attacks[0].name = "scream"; allPixelmon[1].attacks[0].dmg = 600;
    allPixelmon[1].attacks[1].name = "pounce"; allPixelmon[1].attacks[1].dmg = 11;
    allPixelmon[1].attacks[2].name = "chomp"; allPixelmon[1].attacks[2].dmg = 12;
    allPixelmon[1].attacks[3].name = "bomb"; allPixelmon[1].attacks[3].dmg = 13;

    allPixelmon[2].name = "banana";
    allPixelmon[2].bitmap = banana;
    allPixelmon[2].attacks[0].name = "peanut"; allPixelmon[2].attacks[0].dmg = 4;
    allPixelmon[2].attacks[1].name = "butter"; allPixelmon[2].attacks[1].dmg = 12;
    allPixelmon[2].attacks[2].name = "jelly"; allPixelmon[2].attacks[2].dmg = 15;
    allPixelmon[2].attacks[3].name = "time"; allPixelmon[2].attacks[3].dmg = 19;

    allPixelmon[3].name = "the donald";
    allPixelmon[3].bitmap = the_donald;
    allPixelmon[3].attacks[0].name = "grope"; allPixelmon[3].attacks[0].dmg = 2;
    allPixelmon[3].attacks[1].name = "deport"; allPixelmon[3].attacks[1].dmg = 4;
    allPixelmon[3].attacks[2].name = "build a wall"; allPixelmon[3].attacks[2].dmg = 20;
    allPixelmon[3].attacks[3].name = "make america great again"; allPixelmon[3].attacks[3].dmg = 25;
}
