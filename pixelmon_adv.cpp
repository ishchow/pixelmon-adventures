#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>

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

int num_scores;
player current_player;

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

void clearEEPROM() {
	for (int i = 0; i < EEPROM.length(); ++i) {
		EEPROM.update(i, 255); // 255 signifies unread
	}
}

// Reads raw bytes from EEPROM and displays in serial monitor
// Only displays the desired number of elements
void displayEEPROM(int num_elements) {
	num_elements = constrain(num_elements, 0, EEPROM.length());
	char braces[10];
	Serial.println();
	for(int i = 0; i < num_elements; ++i) {
		int val = EEPROM.read(i);
		sprintf(braces, "[%d]", i);
		Serial.print(F("EEPROM")); Serial.print(braces); Serial.print(": ");
		Serial.println(val);
	}
	Serial.println();
}

void getNumScores() {
	int eeprom_start = 0;
	EEPROM.get(eeprom_start, num_scores);
	Serial.println(F("num_scores retrieved!"));
	Serial.print(F("num_scores: ")); Serial.println(num_scores);
}

void storeNumScores() {
	Serial.print(F("num_scores: ")); Serial.println(num_scores);
	int eeprom_start = 0;
	EEPROM.put(eeprom_start, num_scores);
	Serial.println(F("num_scores stored!"));
}

void tableToSerial() {
	getNumScores();
	if (num_scores == -1) return;
	Serial.println(F("High score table: "));
	for (int eeprom_add = sizeof(num_scores); eeprom_add < sizeof(num_scores) + num_scores*sizeof(player);
			 eeprom_add += sizeof(player)) {
		player selected_player;
		EEPROM.get(eeprom_add, selected_player);
		Serial.print(F("Name: ")); Serial.print(selected_player.name);
		Serial.print(F(" ")); Serial.println(selected_player.score);
	}
}

void playerToTable() {
	getNumScores();
	// After last entry in table
	int eeprom_add = sizeof(num_scores) + num_scores*sizeof(player);
	EEPROM.put(eeprom_add, current_player);
	++num_scores;
	storeNumScores();
}

// Retrieves player name from serial monitor
void getPlayerName() {
	Serial.print(F("Enter your name (three char only): "));
	char player_name[4];
	int i=0;
	while (i<3) {
		while (Serial.available()==0) {}
		int letter = Serial.read();
		if (!isalpha(letter)) {
			Serial.println(F("Letters only please!"));
			Serial.print(F("Enter your name (three char only): "));
		}
		else {
			player_name[i] = letter;
			Serial.print((char)letter);
			++i;
		}
	}
	player_name[4] = '\0';
	Serial.println();
	strncpy(current_player.name, player_name, sizeof(current_player.name) - 1);
}


// // show text in EEPROM
// void showEEPROM() {
// 	Serial.print("The current text in EEPROM: ");
// 	for (int i = 0; i < EEPROM.length() && isalnum(EEPROM[i]); ++i) {
// 		Serial.write( EEPROM[i] );
// 	}
// 	Serial.println("");
// }

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
	if (num_scores == -1) { // num_scores == -1 when eeprom is clear
		num_scores = 0;
		storeNumScores();
	}
}

int main() {
	setup();
	// for (int i = 0; i < 10; ++i) {
	// 	current_player.score = random(101);
	// 	getPlayerName();
	// 	playerToTable();
	// }
	// tableToSerial();
	//
	// address = sizeof(num_scores);
	// player current_player;
	// char name1[4] = "SAM";
	// strncpy(current_player.name, name1, sizeof(current_player.name));
	// current_player.score = 999;
	// EEPROM.put(address, current_player);
	//
	// address += sizeof(player);
	// char name2[4] = "ISH";
	// strncpy(current_player.name, name2, sizeof(current_player.name));
	// current_player.score = 150;
	// EEPROM.put(address, current_player);

	// num_scores = 2;

	// num_scores = 5;
	// EEPROM.put(address, num_scores);
	// EEPROM.get(address, num_scores);
	// Serial.print("num_scores: "); Serial.println(num_scores);
	loadAllPixelmon();
	// pixelmon enemy_pxm;
	// generatePixelmon(&enemy_pxm);
	// PVPbattleMode(&ownedPixelmon[0], &enemy_pxm);
	// tft.fillScreen(ST7735_BLACK);

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
	ownedPixelmon[4].pixelmon_id = 1;
	ownedPixelmon[4].health = 1;
	Serial.print(F("num_pxm_owned: ")); Serial.println(num_pxm_owned);

	long startTime = millis();
	while (true) {
		if (allOwnedPixelmonDead()) {
			// Serial.print(player_score);
			// for (int j=0; j<EEPROM.length(); ++j) {
			// 	if (isalnum(EEPROM[j])) {}
			// 	else {
			// 		// EEPROM.put(j,player_score);
			// 		EEPROM[j]='0'+player_score;
			// 		break;
			// 	}
			// }
			Serial.print(F("Current score: ")); Serial.println(current_player.score);
			tft.fillScreen(ST7735_BLACK);
			tft.setTextSize(1);
			tft.setCursor(0,0);
			tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
			tft.print(F("GAME OVER"));
			// showEEPROM();
			break;
		}
		if (encounter_wild_pixelmon) { // battle wild pixelmon in game mode
			pixelmon wd; // Wild pixelmon
			Serial.println(F("Wild: "));
			generatePixelmon(&wd);
			printPixelmon(&wd);
			tft.fillScreen(ST7735_BLACK);
			battleMode(&ownedPixelmon[0], &wd); // Initiate battle b/w player and wild
			encounter_wild_pixelmon = false;
			updateMap();
			updateScreen();
			Serial.print(F("Current score: ")); Serial.print(current_player.score);
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
