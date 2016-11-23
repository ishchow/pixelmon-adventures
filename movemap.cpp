#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

#include "lcd_image.h"

#define SD_CS    5  // Chip select line for SD card
#define TFT_CS   6  // Chip select line for TFT display
#define TFT_DC   7  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)

#define JOY_VERT_ANALOG 0
#define JOY_HORIZ_ANALOG 1
#define JOY_SEL 9
#define JOY_DEADZONE 64

#define TFT_WIDTH 128
#define TFT_HEIGHT 160

#define SPRITE_WIDTH 19
#define SPRITE_HEIGHT 28

#define INV_SPRITE_SPEED 50

#define MILLIS_PER_FRAME 100 //framerate

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
Sd2Card card;

lcd_image_t map_image = { "chess.lcd", 480, 480 };
lcd_image_t sprite = { "sprite.lcd", 19, 28 };

// top_leftY and top_leftX are coordinates of top left corner on tft
int16_t top_leftX = 240;
int16_t top_leftY = 240;

//global variables to hold the centre position of joystick
int g_joyCentreX;
int g_joyCentreY;

//global variables for sprite's x,y position
int g_spriteX = TFT_WIDTH/2;
int g_spriteY = TFT_HEIGHT/2;
//previously drawn position of sprite
int g_prevX = -1;
int g_prevY = -1;

//global var. is 1 when joystick is moved or pressed, 0 otherwise
int update = 0;

//get centre position of joystick and set as default
void calibrateJoyCentre(){
  g_joyCentreY = analogRead(JOY_VERT_ANALOG);
  g_joyCentreX = analogRead(JOY_HORIZ_ANALOG);
  Serial.println("Calibration complete");
  Serial.print("x Calibration: "); Serial.println(g_joyCentreX);
  Serial.print("y Calibration: "); Serial.println(g_joyCentreY);
}

// initialize chip, SD card, joystick
void setup() {
  init();
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735R chip, black or blue tab
  // init SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("OK!");
  digitalWrite(JOY_SEL, HIGH); // enable joystick internal pull-up resistor
  // clear to black
  tft.fillScreen(ST7735_BLACK);
  calibrateJoyCentre();
  update = 1;
  Serial.println("Setup Complete");
}

// redraw entire map (for shifting the map on the screen)
void updateMap(){
  lcd_image_draw(&map_image, &tft,
                 top_leftX, top_leftY, //image
                 0, 0, //screen
                 TFT_WIDTH, TFT_HEIGHT); //how much to draw
                 Serial.print("top left X: ");Serial.println(top_leftX);
                 Serial.print("top left Y: ");Serial.println(top_leftY);
}


// redrawing sprite on the map
void updateScreen() {
  //clear old sprite
  lcd_image_draw(&map_image, &tft,
                 top_leftX + g_prevX, top_leftY + g_prevY,
                 g_prevX, g_prevY,
                 18, 27);
  g_prevX = g_spriteX;
  g_prevY = g_spriteY;
  //draws new sprite
  lcd_image_draw(&sprite, &tft,
                 0, 0, // image
                 g_prevX, g_prevY, // screen
                 18, 27); // amount to draw
  update = 0;
}

// scan joystick and update cursor position
void scanJoystick(){
  int v = analogRead(JOY_VERT_ANALOG);
  int h = analogRead(JOY_HORIZ_ANALOG);
  bool invSelect = digitalRead(JOY_SEL); // HIGH when not pressed, LOW when pressed
    //update g_spriteX & g_spriteY when outside of deadzone
    if (abs(v - g_joyCentreY) > JOY_DEADZONE) { //vertical movement
      //outside of deadzone
      update = 1;
      int unboundedY = g_spriteY + (v - g_joyCentreY)/INV_SPRITE_SPEED;
      g_spriteY = constrain(unboundedY, 0, TFT_HEIGHT - SPRITE_HEIGHT - 1);
      Serial.print("g_spriteY: ");Serial.println(g_spriteY);
      //check if sprite is off screen and not at map edge
      // if 1, shift map; if 0, do nothing
      if ( ((unboundedY >= TFT_HEIGHT - SPRITE_HEIGHT)&&(top_leftY != 480 - TFT_HEIGHT -1)) || ((unboundedY < 0) && (top_leftY != 0)) ) {
        g_spriteY = constrain(TFT_HEIGHT - SPRITE_HEIGHT - g_spriteY, 0, TFT_HEIGHT - SPRITE_HEIGHT - 1);
        // Serial.print("unboundedY: "); Serial.println(unboundedY);
        // Serial.println(abs(unboundedY)/unboundedY);
        top_leftY = constrain(top_leftY + (abs(unboundedY)/unboundedY)*TFT_HEIGHT, 0 , 480 - TFT_HEIGHT -1);
        updateMap();
      }
    }
    if (abs(h - g_joyCentreX) > JOY_DEADZONE) { //horizontal movement
      update = 1;
      //outside of deadzone
      int unboundedX = g_spriteX + (h - g_joyCentreX)/INV_SPRITE_SPEED;
      g_spriteX = constrain(unboundedX, 0, TFT_WIDTH - SPRITE_WIDTH - 1);
      Serial.print("g_spriteX: ");Serial.println(g_spriteX);
      //check if sprite is off screen and not at map edge
      // if 1, shift map; if 0, do nothing
      if ((unboundedX >= TFT_WIDTH - SPRITE_WIDTH)&&(top_leftX!= 480 - TFT_WIDTH -1)|| ((unboundedX < 0) && (top_leftX != 0)) ) {
        g_spriteX = constrain(TFT_WIDTH - SPRITE_WIDTH - g_spriteX, 0, TFT_WIDTH - SPRITE_WIDTH - 1);
        // Serial.println(abs(unboundedX)/unboundedX);
        // Serial.print("unboundedX: "); Serial.println(unboundedX);
        top_leftX = constrain(top_leftX + (abs(unboundedX)/unboundedX)*TFT_WIDTH, 0 , 480 - TFT_WIDTH -1);
        updateMap();
      }
    }
  }

int main() {
  setup();
  updateMap();
  int startTime = millis();
  while (true) {
    scanJoystick();
    if (update == 1) {
      updateScreen();
    }
    //keep constant framerate
    int timeDiff = millis() - startTime; //time elapsed from start of loop until after refresh
    if (timeDiff < MILLIS_PER_FRAME){
      delay(MILLIS_PER_FRAME - timeDiff);
    }
    startTime = millis();
  }

  return 0;
}
