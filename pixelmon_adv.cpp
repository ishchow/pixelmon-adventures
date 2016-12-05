#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>

#include "lcd_image.h"
#include <avr/pgmspace.h> // For PROGMEM and F()
#include "pixelmondata.h" //loads pixelmon data types and data
#include "battlemode.h" //pixelmon stats, battlemode, menus
#include "pvpbattle.h" // for pvp pixelmon battles
#include "highscoretable.h" // for high score table
#include "ArduinoExtras.h"

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
	Serial.println(F("Calibration complete"));
	Serial.print(F("x Calibration: ")); Serial.println(g_joyCentreX);
	Serial.print(F("y Calibration: ")); Serial.println(g_joyCentreY);
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
int scanJoystick(int *selection, uint8_t game_mode, uint8_t num_options){
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
				*selection = constrain(*selection+1, 0, num_options-1); //down
			} else {
				delay(100);
				*selection = constrain(*selection-1, 0, num_options-1); //up
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
	Serial.print(F("Initializing SD card..."));
	if (!SD.begin(SD_CS)) {
		Serial.println(F("failed!"));
		return;
	}
	Serial.println(F("OK!"));
	digitalWrite(JOY_SEL, HIGH); // enable joystick internal pull-up resistor
	// clear to black
	tft.fillScreen(ST7735_BLACK);
	calibrateJoyCentre();
	update = true;
	Serial.println(F("Setup Complete"));
	Serial.print(F("Size of pixelmon_type: ")); Serial.println(sizeof(pixelmon_type));
	Serial.print(F("Size of allPixelmon: ")); Serial.println(sizeof(pixelmon_type)*NUM_PIXELMON_TYPES);
	// clearEEPROM();
	getNumScores();
}

int main() {
	setup();
	loadAllPixelmon();
	player current_player;
	// Initialize current player
	current_player.score = 0;
	strncpy(current_player.name, "", sizeof(current_player.name)/sizeof(current_player.name[0]));
	tft.setCursor(0, 64);
	tft.setTextSize(1);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	tft.setTextWrap(true);
	tft.print(F("Before you embark on your journey to become THE VERY BEST, please enter your name in the serial-monitor."));
	getPlayerName(&current_player);
	tft.fillRect(0, 64, TFT_WIDTH, 6*8, ST7735_BLACK); // Clear previous text
	char* message = new char[64];
	sprintf(message, "Good luck on your journey, %s! Your current score is %d.", current_player.name, current_player.score);
	tft.setCursor(0, 64);
	tft.print(message);
	delete[] message;
	delay(2500);

	updateMap();
	int PVPChallenge = 1;
	int PVP_choice = 0;
	int prevPVP_choice = 0;
	int battleConfirm = 0;
	const char *PVPYESNO[] = {"YES", "NO"};

	for (int i = 0; i < MAX_OWNED - 1; ++i) {
		generatePixelmon(&ownedPixelmon[i]);
		num_pxm_owned = i + 1;
		// this line is for testing; causes all pixelmon health except last to go to 0
		if ( i != MAX_OWNED - 2) {
			ownedPixelmon[i].health = 0;
		}
		printPixelmon(&ownedPixelmon[i]);
	}
	ownedPixelmon[4].pixelmon_id = 1; // veggisaur
	// ownedPixelmon[4].health = 1;
	Serial.print(F("num_pxm_owned: ")); Serial.println(num_pxm_owned);

	long startTime = millis();
	while (true) {
		if (allOwnedPixelmonDead()) {
			tft.fillScreen(ST7735_BLACK);
			tft.setTextSize(1);
			tft.setCursor(0,0);
			tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
			tft.print(F("GAME OVER"));
			if (strcmp(current_player.name, "") != 0) { // Player actually enters a name
				playerToTable(&current_player);
			}
			tableToSerial();
			break;
		}
		if (encounter_wild_pixelmon) { // battle wild pixelmon in game mode
			pixelmon wd; // Wild pixelmon
			generatePixelmon(&wd);
			tft.fillScreen(ST7735_BLACK);
			battleMode(&ownedPixelmon[0], &wd, &current_player); // Initiate battle b/w player and wild
			encounter_wild_pixelmon = false;
			updateMap();
			updateScreen();
			Serial.print(F("Current score: ")); Serial.println(current_player.score);
			Serial.print(F("num_pxm_owned: ")); Serial.println(num_pxm_owned);
			for (int i = 0; i < num_pxm_owned; ++i) printPixelmon(&ownedPixelmon[i]);
			startTime = millis();
		} else { // map mode
			uint8_t game_mode = 0;
			PVPChallenge = scanJoystick(NULL, game_mode, NULL);
			if (PVPChallenge == 0) {
				displayPVPChallengeMenu(PVP_choice, PVPYESNO);
				while (true) { //selection pvp choice while loop
					int press = scanJoystick(&PVP_choice, 1, 2);
					if (prevPVP_choice != PVP_choice) {
						updatePVPChallengeMenu(PVP_choice, prevPVP_choice, PVPYESNO);
						prevPVP_choice = PVP_choice;
					}
					if (press == LOW) {
						press = HIGH;
						prevPVP_choice = PVP_choice;
						break;
					}
				}
				if (PVP_choice == 0) { // Player enters PVP Mode
					if (digitalRead(13) == HIGH) {
						battleConfirm = integerServerFSM(-1000);
					}
					else {
						battleConfirm = integerClientFSM(-1000);
					}
					if (battleConfirm == -1000) {
						pixelmon enemy_pxm = {-1, 0, 0, 0};
						// generatePixelmon(&enemy_pxm);
						tft.fillScreen(ST7735_BLACK);
						PVPbattleMode(&ownedPixelmon[0], &enemy_pxm);
						updateMap();
						updateScreen();
						startTime = millis();
					}
					else {
						updateMap();
						updateScreen();
						startTime = millis();
					}
				}
				else {
					if (digitalRead(13) == HIGH) {
						integerServerFSM(-2000);
					}
					else {
						integerClientFSM(-2000);
					}
					updateMap();
					updateScreen();
					startTime = millis();
				}
			}
			if (update) updateScreen();
		}

		//keep constant framerate
		long timeDiff = millis() - startTime; //time elapsed since start of loop or
																					// last battle (pvp or wild) until after refresh
		if (timeDiff < MILLIS_PER_FRAME){
			delay(MILLIS_PER_FRAME - timeDiff);
		}
		startTime = millis();
	}

	Serial3.end();
	Serial.end();
	return 0;
}
