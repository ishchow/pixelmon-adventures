/*
ard_template.cpp

Authors:
Ishaat Chowdhury	Section: EA1
Sam Wu              Section: EA2

Description:

*/

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

#include "lcd_image.h"

// Display pins:
// standard U of A library settings, assuming Atmel Mega SPI pins
#define SD_CS    5 // Chip select line for SD card
#define TFT_CS   6 // Chip select line for TFT display
#define TFT_DC   7 // Data/command line for TFT
#define TFT_RST  8 // Reset Line for TFT (or connect to +5V)

// #define JOY_VERT_ANALOG 0
// #define JOY_HORIZ_ANALOG 1
// #define JOY_DEADZONE 64
// #define JOY_CENTRE 512
// #define JOY_SPEED 5
// #define JOY_SEL 9

// #define MILLIS_PER_FRAME 50 // 20fps
// #define STEPS_PER_PIXEL 64

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
Sd2Card card;

void setup() {
    init();
    Serial.begin(9600);

    Serial.print("Initializing SD card...");
    if (!SD.begin(SD_CS)) {
        Serial.println("failed!");
        return;
    }

    tft.initR(INITR_BLACKTAB);   // initialize a ST7735R chip, black tab

    Serial.println("OK!");
}

int  main() {

    while (true) {

    }

    Serial.end();
    return 0;
}
