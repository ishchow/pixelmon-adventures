#include "pixelmondata.h"

pixelmon_type allPixelmon[NUM_PIXELMON_TYPES];

const uint8_t squirtle[] PROGMEM = {
       // 'squirtle'
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
       // 'bulb'
    0x00, 0x00, 0x02, 0x80, 0x00, 0x00, 0x05, 0xe0, 0x00, 0x00, 0x1d, 0xa0, 0x00, 0x01, 0xc1, 0x38,
   	0x00, 0x04, 0x02, 0x78, 0x00, 0x08, 0x70, 0x78, 0x00, 0x10, 0x80, 0x38, 0x08, 0x11, 0xf8, 0x3c,
   	0x04, 0x76, 0x08, 0x7e, 0x00, 0x00, 0x08, 0x7e, 0x00, 0x00, 0x08, 0xff, 0x00, 0x00, 0x0f, 0xff,
   	0x00, 0x02, 0x0f, 0xff, 0x00, 0x01, 0x07, 0xff, 0x10, 0x08, 0x85, 0xff, 0x10, 0x1c, 0x85, 0xfe,
   	0x00, 0x04, 0x84, 0xfe, 0x80, 0x00, 0x05, 0xfc, 0x80, 0x80, 0x05, 0xf8, 0x00, 0x00, 0x49, 0xd8,
   	0x4c, 0x77, 0x80, 0x08, 0x2f, 0xff, 0x02, 0xc4, 0x17, 0xfe, 0x03, 0xb4, 0x01, 0x17, 0x8d, 0xf2,
   	0x03, 0xfe, 0x8d, 0x62, 0x19, 0xf8, 0x85, 0x02, 0x11, 0xc0, 0x81, 0x86, 0x01, 0x3f, 0xc1, 0xcc,
   	0x01, 0x0f, 0xe1, 0xd4, 0x16, 0x07, 0xaa, 0x78, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00
   };

void loadAllPixelmon() {
    allPixelmon[0].name = "squirtle";
    allPixelmon[0].bitmap = squirtle;
    allPixelmon[0].attacks[0].name = "scratch"; allPixelmon[0].attacks[0].dmg = 6;
    allPixelmon[0].attacks[1].name = "bite"; allPixelmon[0].attacks[1].dmg = 11;
    allPixelmon[0].attacks[2].name = "water gun"; allPixelmon[0].attacks[2].dmg = 12;
    allPixelmon[0].attacks[3].name = "cute charm"; allPixelmon[0].attacks[3].dmg = 13;

    allPixelmon[1].name = "bulbosaur";
    allPixelmon[1].bitmap = bulbosaur;
    allPixelmon[1].attacks[0].name = "tackle"; allPixelmon[1].attacks[0].dmg = 6;
    allPixelmon[1].attacks[1].name = "kick"; allPixelmon[1].attacks[1].dmg = 11;
    allPixelmon[1].attacks[2].name = "vine whip"; allPixelmon[1].attacks[2].dmg = 12;
    allPixelmon[1].attacks[3].name = "leaf shoot"; allPixelmon[1].attacks[3].dmg = 13;
}
