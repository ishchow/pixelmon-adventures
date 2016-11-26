#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

#include "lcd_image.h"
#include <avr/pgmspace.h> // For PROGMEM
#include "pixelmondata.h" //loads pixelmon data types and data
#include "battlemode.h" //pixelmon stats, battlemode, menus
#include "pvpbattle.h" // for pvp pixelmon battles

// Display pins:
// standard U of A library settings, assuming Atmel Mega SPI pins
#define SD_CS    5 // Chip select line for SD card
#define TFT_CS   6 // Chip select line for TFT display
#define TFT_DC   7 // Data/command line for TFT
#define TFT_RST  8 // Reset Line for TFT (or connect to +5V)

#define JOY_VERT_ANALOG 0
#define JOY_HORIZ_ANALOG 1
#define JOY_SEL 9
#define JOY_DEADZONE 64

#define MILLIS_PER_FRAME 50 // 20fps

#define TFT_WIDTH 128
#define TFT_HEIGHT 160

#define SPRITE_WIDTH 19
#define SPRITE_HEIGHT 28
#define MAP_DIMENSIONS 480

#define INV_SPRITE_SPEED 50

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
Sd2Card card;

lcd_image_t map_image = { "chess.lcd", 480, 480 };
lcd_image_t sprite = { "sprite.lcd", 19, 28 };

// top_leftY and top_leftX are coordinates of top left corner on tft
int16_t top_leftX = MAP_DIMENSIONS/2;
int16_t top_leftY = MAP_DIMENSIONS/2;

//global variables to hold the centre position of joystick, will be calibrated
int g_joyCentreX;
int g_joyCentreY;

//global variables for sprite's x,y position
int g_spriteX = TFT_WIDTH/2;
int g_spriteY = TFT_HEIGHT/2;

//previously drawn position of sprite
int g_prevX = -1;
int g_prevY = -1;

//global var. is true when joystick is moved or pressed, false otherwise
bool update = false;

// If true, used to enter battle mode
bool encounter_wild_pixelmon = 0;

extern pixelmon_type allPixelmon[];

int num_pxm_owned = 1; // 1 <= pxm_owned <= MAX_OWNED
pixelmon ownedPixelmon[MAX_OWNED];

//get centre position of joystick and set as default
void calibrateJoyCentre(){
	g_joyCentreY = analogRead(JOY_VERT_ANALOG);
	g_joyCentreX = analogRead(JOY_HORIZ_ANALOG);
	Serial.println("Calibration complete");
	Serial.print("x Calibration: "); Serial.println(g_joyCentreX);
	Serial.print("y Calibration: "); Serial.println(g_joyCentreY);
}

// redraw entire map (for shifting the map on the screen)
void updateMap(){
	lcd_image_draw(&map_image, &tft,
		top_leftX, top_leftY, //image
		0, 0, //screen
		TFT_WIDTH, TFT_HEIGHT); //how much to draw
}

// redrawing sprite on the map
void updateScreen() {
	//clear old sprite
	lcd_image_draw(&map_image, &tft,
		top_leftX + g_prevX, top_leftY + g_prevY, // image
		g_prevX, g_prevY, // screen
		SPRITE_WIDTH, SPRITE_HEIGHT); //amount drawn
		// new coord. of sprite
		g_prevX = g_spriteX;
		g_prevY = g_spriteY;
		//draws new sprite
		lcd_image_draw(&sprite, &tft,
			0, 0, // image
			g_prevX, g_prevY, // screen
			SPRITE_WIDTH-1, SPRITE_HEIGHT-1); // amount to draw
		update = false; //stop drawing
}

// scan joystick and update cursor position
int scanJoystick(int *selection, uint8_t game_mode, uint8_t max_selection){
	int v = analogRead(JOY_VERT_ANALOG);
	int h = analogRead(JOY_HORIZ_ANALOG);
	int select = digitalRead(JOY_SEL); // HIGH when not pressed, LOW when pressed
	//update g_spriteX & g_spriteY when outside of deadzone

	if (game_mode == 0) { // map mode = 0
		if (abs(v - g_joyCentreY) > JOY_DEADZONE) { //vertical movement
			//outside of deadzone
			update = true;
			if (random(0,100)>90) {encounter_wild_pixelmon = 1;}
			int newY = g_spriteY + (v - g_joyCentreY)/INV_SPRITE_SPEED;
			g_spriteY = constrain(newY, 0, TFT_HEIGHT - SPRITE_HEIGHT - 1);
			//check if sprite is off screen and not at map edge
			// if 1, shift map; if 0, do nothing
			if ( ((newY >= TFT_HEIGHT - SPRITE_HEIGHT)&& (top_leftY!= MAP_DIMENSIONS - TFT_HEIGHT -1)) || ((newY < 0) && (top_leftY != 0)) ) {
				g_spriteY = constrain(TFT_HEIGHT - SPRITE_HEIGHT - g_spriteY, 0, TFT_HEIGHT - SPRITE_HEIGHT - 1);
				// Serial.print("newY: "); Serial.println(newY);
				// Serial.println(abs(newY)/newY);
				top_leftY = constrain(top_leftY + (abs(newY)/newY)*TFT_HEIGHT, 0 , MAP_DIMENSIONS - TFT_HEIGHT -1);
				updateMap();
			}
		}
		if (abs(h - g_joyCentreX) > JOY_DEADZONE) { //horizontal movement
			update = true;
			if (random(0,100)>90) {encounter_wild_pixelmon = 1;}
			//outside of deadzone
			int newX = g_spriteX + (h - g_joyCentreX)/INV_SPRITE_SPEED;
			g_spriteX = constrain(newX, 0, TFT_WIDTH - SPRITE_WIDTH);
			//check if sprite is off screen and not at map edge
			// if 1, shift map; if 0, do nothing
			if ((newX >= TFT_WIDTH - SPRITE_WIDTH )&&(top_leftX!= MAP_DIMENSIONS - TFT_WIDTH -1)|| ((newX < 0) && (top_leftX != 0)) ) {
				g_spriteX = constrain(TFT_WIDTH - SPRITE_WIDTH - g_spriteX, 0, TFT_WIDTH - SPRITE_WIDTH - 1);
				// Serial.println(abs(newX)/newX);
				// Serial.print("newX: "); Serial.println(newX);
				top_leftX = constrain(top_leftX + (abs(newX)/newX)*TFT_WIDTH, 0 , MAP_DIMENSIONS - TFT_WIDTH -1);
				updateMap();
			}
		}
	}
	else { // battle mode = 1
		if (abs(v - g_joyCentreY) > JOY_DEADZONE) {
			int delta = v - g_joyCentreY;
			if (delta > 0) {
				delay(100);
				*selection = constrain(*selection+1, 0, max_selection-1); //down
			} else {
				delay(100);
				*selection = constrain(*selection-1, 0, max_selection-1); //up
			}
		}
	}
	return select;
}

// initialize chip, SD card, joystick
void setup() {
	init();
	Serial.begin(9600);
	Serial3.begin(9600);
	randomSeed(analogRead(7)); // for better random numbers
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
	update = true;
	Serial.println("Setup Complete");
	Serial.print("Size of pixelmon_type: "); Serial.println(sizeof(pixelmon_type));
	Serial.print("Size of pixelmon: "); Serial.println(sizeof(pixelmon));
}

int main() {
	setup();
	loadAllPixelmon();

	for (int i = 0; i < MAX_OWNED - 1; ++i) {
		generatePixelmon(&ownedPixelmon[i]);
		num_pxm_owned = i + 1;
		// // this line is odd; causes monster health to go to 0
		// if ( i != MAX_OWNED - 2) {
		// 	ownedPixelmon[i].health = 0;
		// }
		printPixelmon(&ownedPixelmon[i]);
	}
	Serial.print("num_pxm_owned: "); Serial.println(num_pxm_owned);

	pixelmon enemy_pxm;
	generatePixelmon(&enemy_pxm);
	PVPbattleMode(&ownedPixelmon[0], &enemy_pxm);
	tft.fillScreen(ST7735_BLACK);
	while (true) {}

	updateMap();

	for (int i = 0; i < MAX_OWNED - 1; ++i) {
		generatePixelmon(&ownedPixelmon[i]);
		num_pxm_owned = i + 1;
		// this line is odd; causes monster health to go to 0
		if ( i != MAX_OWNED - 2) {
			ownedPixelmon[i].health = 0;
		}
		printPixelmon(&ownedPixelmon[i]);
	}
	Serial.print("num_pxm_owned: "); Serial.println(num_pxm_owned);

	int startTime = millis();
	while (true) {
		if (encounter_wild_pixelmon) {
			pixelmon wd; // Wild
			generatePixelmon(&wd);

			tft.fillScreen(ST7735_BLACK);
			battleMode(&ownedPixelmon[0], &wd);
			encounter_wild_pixelmon = false;
			updateMap();
			updateScreen();
			Serial.print("num_pxm_owned: "); Serial.println(num_pxm_owned);
			for (int i = 0; i < num_pxm_owned; ++i) printPixelmon(&ownedPixelmon[i]);
			delay(50); // Prevent debouncing?
			if (allOwnedPixelmonDead()) healAllOwnedPixelmon();
		} else {
			uint8_t game_mode = 0;
			scanJoystick(NULL, game_mode, NULL);
			if (update) updateScreen();
		}

		//keep constant framerate
		int timeDiff = millis() - startTime; //time elapsed from start of loop until after refresh
		if (timeDiff < MILLIS_PER_FRAME){
			delay(MILLIS_PER_FRAME - timeDiff);
		}
		startTime = millis();
	}

	Serial3.end();
	Serial.end();
	return 0;
}
