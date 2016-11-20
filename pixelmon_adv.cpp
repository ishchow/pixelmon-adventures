#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

#include "lcd_image.h"
#include <avr/pgmspace.h> // For PROGMEM
#include "allpixelmon.h"

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
#define STEPS_PER_PIXEL 64

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

//global var. is 1 when joystick is moved or pressed, 0 otherwise
int update = 0;

//stores mode: map=0, fight=1
bool game_mode = 0;
bool encounter_wild_pixelmon = 0;

extern pixelmon_type allPixelmon[];

const int MAX_OWNED = 6;
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

			//change between game modes
			void change_game_mode() {
				game_mode = !game_mode;
				update = 1;
				Serial.print("Game Mode: ");
				Serial.println(game_mode);
			}

			// scan joystick and update cursor position
			int scanJoystick(int* selection){
				int v = analogRead(JOY_VERT_ANALOG);
				int h = analogRead(JOY_HORIZ_ANALOG);
				int select = digitalRead(JOY_SEL); // HIGH when not pressed, LOW when pressed
				//update g_spriteX & g_spriteY when outside of deadzone

				if (game_mode == 0) { // map mode = 0
					if (abs(v - g_joyCentreY) > JOY_DEADZONE) { //vertical movement
						//outside of deadzone
						update = 1;
						if (random(0,100)>80) {encounter_wild_pixelmon = 1;}
						int newY = g_spriteY + (v - g_joyCentreY)/INV_SPRITE_SPEED;
						g_spriteY = constrain(newY, 0, TFT_HEIGHT - SPRITE_HEIGHT - 1);
						//check if sprite is off screen and not at map edge
						// if 1, shift map; if 0, do nothing
						if ( ((newY >= TFT_HEIGHT)&& (top_leftY!= MAP_DIMENSIONS - TFT_HEIGHT -1)) || ((newY < 0) && (top_leftY != 0)) ) {
							g_spriteY = constrain(TFT_HEIGHT - g_spriteY, 0, TFT_HEIGHT - SPRITE_HEIGHT - 1);
							// Serial.print("newY: "); Serial.println(newY);
							// Serial.println(abs(newY)/newY);
							top_leftY = constrain(top_leftY + (abs(newY)/newY)*TFT_HEIGHT, 0 , MAP_DIMENSIONS - TFT_HEIGHT -1);
							updateMap();
						}
					}
					if (abs(h - g_joyCentreX) > JOY_DEADZONE) { //horizontal movement
						update = 1;
						if (random(0,100)>80) {encounter_wild_pixelmon = 1;}
						//outside of deadzone
						int newX = g_spriteX + (h - g_joyCentreX)/INV_SPRITE_SPEED;
						g_spriteX = constrain(newX, 0, TFT_WIDTH - SPRITE_WIDTH);
						//check if sprite is off screen and not at map edge
						// if 1, shift map; if 0, do nothing
						if ((newX >= TFT_WIDTH )&&(top_leftX!= MAP_DIMENSIONS - TFT_WIDTH -1)|| ((newX < 0) && (top_leftX != 0)) ) {
							g_spriteX = constrain(TFT_WIDTH - g_spriteX, 0, TFT_WIDTH - SPRITE_WIDTH - 1);
							// Serial.println(abs(newX)/newX);
							// Serial.print("newX: "); Serial.println(newX);
							top_leftX = constrain(top_leftX + (abs(newX)/newX)*TFT_WIDTH, 0 , MAP_DIMENSIONS - TFT_WIDTH -1);
							updateMap();
						}
					}
				}
				else { // fight mode = 1
					if (abs(v - g_joyCentreY) > JOY_DEADZONE) {
						int delta = v - g_joyCentreY;
						if (delta > 0) {
							*selection = constrain(*selection+1, 0, 3);
						} else {
							*selection = constrain(*selection-1, 0, 3);
						}
					}
				}
				return select;
			}

			void generatePixelmon(pixelmon *px) {
				px->pixelmon_id = random(NUM_PIXELMON_TYPES);
				px->health = random(75, 100);
				px->level = random(1, 11);
				px->xp = 0;
			}

			bool pixelmonEqual(pixelmon *px1, pixelmon *px2) {
				if (px1->pixelmon_id != px2->pixelmon_id) return false;
				else if (px1->health != px2->health) return false;
				else if (px1->level != px2->level) return false;
				else if (px1->xp != px2->xp) return false;
				else return true;
			}

			void drawPixelmon(pixelmon *px, int16_t x, int16_t y, uint16_t bmp_color) {
				tft.drawBitmap(x, y, allPixelmon[px->pixelmon_id].bitmap,
					allPixelmon[px->pixelmon_id].bitmap_width,
					allPixelmon[px->pixelmon_id].bitmap_height, bmp_color);
				}

				void erasePixelmon(pixelmon *px, int16_t x, int16_t y, uint16_t bg_color) {
					tft.fillRect(x, y,
						allPixelmon[px->pixelmon_id].bitmap_width,
						allPixelmon[px->pixelmon_id].bitmap_height, bg_color);
					}

					bool execAttack(pixelmon *attacker, pixelmon *victim, int attack_id) {
						int prev_health = victim->health;
						int hit_probability = random(0, 101);
						if (hit_probability > 25) {
							victim->health -= allPixelmon[attacker->pixelmon_id].attacks[attack_id].dmg;
						}
						if (victim->health < prev_health) return true;
						else return false;
					}

					void hitAnimation(pixelmon *injured, int16_t injured_x, int16_t injured_y, uint16_t bmp_color, uint16_t bg_color) {
						delay(500);
						erasePixelmon(injured, injured_x, injured_y, bg_color);
						delay(500);
						drawPixelmon(injured, injured_x, injured_y, bmp_color);
					}

					void deathAnimation(pixelmon *killed, int16_t killed_x, int16_t killed_y, uint16_t bg_color) {
						drawPixelmon(killed, killed_x, killed_y, ST7735_RED);
						delay(500);
						erasePixelmon(killed, killed_x, killed_y, bg_color);
						delay(500);
					}

					void dodgeAnimation(pixelmon *px, int16_t x, int16_t y, uint16_t bmp_color,
						uint16_t bg_color, char attacked) {
							int start_x = x;
							int last_x = x;
							const int DODGE_LENGTH = 10;
							const int MOVE_SPEED = 2;
							int direction = 1;  // Direction relative to pokemon
							// Forward: 1, Reverse: -1

							if (attacked == 'p') {
								while (true) {
									if (direction == 1) x = constrain(x+MOVE_SPEED, start_x, start_x + DODGE_LENGTH);
									else if (direction == -1) x = constrain(x-MOVE_SPEED, start_x, start_x + DODGE_LENGTH);

									if (x != last_x) {
										erasePixelmon(px, last_x, y, ST7735_BLACK);
										delay(50);
										drawPixelmon(px, x, y, ST7735_WHITE);
										last_x = x;
									}

									if (x == start_x + DODGE_LENGTH) direction = -1;
									if (x == start_x && direction == -1) break;
								}
							} else if (attacked == 'w') {
								while (true) {
									if (direction == 1) x = constrain(x-MOVE_SPEED, start_x - DODGE_LENGTH, start_x);
									else if (direction == -1) x = constrain(x+MOVE_SPEED, start_x - DODGE_LENGTH, start_x);

									if (x != last_x) {
										erasePixelmon(px, last_x, y, ST7735_BLACK);
										delay(50);
										drawPixelmon(px, x, y, ST7735_WHITE);
										last_x = x;
									}

									if (x == start_x - DODGE_LENGTH) direction = -1;
									if (x == start_x && direction == -1) break;
								}
							}
							delay(500);
						}

						void displayStats(pixelmon *player_pxm, pixelmon *wild_pxm) {
							tft.setTextSize(1);
							tft.setTextColor(ST7735_WHITE, ST7735_BLACK);

							// Player pixelmon:
							tft.setCursor(0, 33);
							tft.print(allPixelmon[player_pxm->pixelmon_id].name); tft.print("\n");
							tft.print(F("Health: ")); tft.print(player_pxm->health); tft.print("\n");
							tft.print(F("Lvl: ")); tft.print(player_pxm->level); tft.print("\n");
							tft.print(F("XP: ")); tft.print(player_pxm->xp);

							// Wild pixelmon:
							tft.setCursor(TFT_WIDTH/2, 33);
							tft.print(allPixelmon[wild_pxm->pixelmon_id].name);
							tft.setCursor(TFT_WIDTH/2, 33+8);
							tft.print(F("Health: ")); tft.print(wild_pxm->health);
							tft.setCursor(TFT_WIDTH/2, 33+16);
							tft.print(F("Lvl: ")); tft.print(wild_pxm->level);
						}

						void updateHealth(pixelmon *player_pxm, pixelmon *wild_pxm, char hit) {
							tft.setTextSize(1);
							tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
							tft.setTextWrap(false);
							if (hit == 'p') {
								tft.setCursor(0,33+8);
								tft.print(F("Health: ")); tft.print(player_pxm->health);
							} else if (hit == 'w') {
								tft.setCursor(TFT_WIDTH/2, 33+8);
								tft.print(F("Health: ")); tft.print(wild_pxm->health);
							}
						}

						void displayFightMenu(pixelmon *player_pxm, int selected_attack) {
							const int TXT_SIZE = 2;
							const int FIRST_MOVE = (TFT_HEIGHT-1) - 4*8*TXT_SIZE;
							tft.setTextSize(TXT_SIZE);
							tft.setCursor(0, FIRST_MOVE);
							for (int i = 0; i < 4; ++i) {
								if (i != selected_attack) tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
								else tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
								tft.print(allPixelmon[player_pxm->pixelmon_id].attacks[i].name);
								tft.print("\n");
							}
						}

						void eraseMenu() {
							const int TXT_SIZE = 2;
							const int FIRST_ENTRY = (TFT_HEIGHT-1) - 4*8*TXT_SIZE;
							tft.fillRect(0, FIRST_ENTRY, TFT_WIDTH, TFT_HEIGHT - FIRST_ENTRY, ST7735_BLACK);
						}

						void updateFightMenu(pixelmon *player_pxm, int *selected_attack, int *last_selected_attack) {
							const int TXT_SIZE = 2;
							const int FIRST_MOVE = (TFT_HEIGHT-1) - 4*8*TXT_SIZE;
							tft.setCursor(0, FIRST_MOVE + (*last_selected_attack)*TXT_SIZE*8);
							tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
							tft.print(allPixelmon[player_pxm->pixelmon_id].attacks[*last_selected_attack].name);

							tft.setCursor(0, FIRST_MOVE + (*selected_attack)*TXT_SIZE*8);
							tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
							tft.print(allPixelmon[player_pxm->pixelmon_id].attacks[*selected_attack].name);
						}

						void battleMode(pixelmon *player_pxm, pixelmon *wild_pxm) {
							int player_pxm_x = 0, player_pxm_y = 0;
							int wild_pxm_x = (TFT_WIDTH - 1) - 32, wild_pxm_y = 0;
							drawPixelmon(player_pxm, player_pxm_x, player_pxm_y, ST7735_WHITE);
							drawPixelmon(wild_pxm, wild_pxm_x, wild_pxm_y, ST7735_WHITE);

							// char *options[] = {"Fight", "Run", "Swap", "Capture"};
							displayStats(player_pxm, wild_pxm);

							bool player_pxm_turn = true;
							int selected_attack = 0;
							int last_selected_attack = 0;
							while (player_pxm->health > 0 && wild_pxm->health > 0) {
								if (player_pxm_turn) { // Player
									displayFightMenu(player_pxm, selected_attack);
									while (true) {
										int press = scanJoystick(&selected_attack);
										if (last_selected_attack != selected_attack) {
											updateFightMenu(player_pxm, &selected_attack, &last_selected_attack);
											last_selected_attack = selected_attack;
										}
										if (press == LOW) {
											press = HIGH;
											last_selected_attack = selected_attack;
											eraseMenu();
											break;
										}
									}
									Serial.print(F("Player attacks with: "));
									Serial.println(allPixelmon[player_pxm->pixelmon_id].attacks[selected_attack].name);
									Serial.println();
									bool wild_pxm_hit = execAttack(player_pxm, wild_pxm, selected_attack);
									if (wild_pxm_hit) {
										hitAnimation(wild_pxm, wild_pxm_x, wild_pxm_y, ST7735_WHITE, ST7735_BLACK);
										updateHealth(player_pxm, wild_pxm, 'w');
									}
									else dodgeAnimation(wild_pxm, wild_pxm_x, wild_pxm_y, ST7735_WHITE, ST7735_BLACK, 'w');
									player_pxm_turn = false;
									delay(500);
								} else { // Wild Pokemon
									int attack_id = random(4);
									Serial.print(F("Wild attacks with: "));
									Serial.println(allPixelmon[wild_pxm->pixelmon_id].attacks[attack_id].name);
									Serial.println();
									bool player_pxm_hit = execAttack(wild_pxm, player_pxm, attack_id);
									if (player_pxm_hit) {
										hitAnimation(player_pxm, player_pxm_x, player_pxm_y, ST7735_WHITE, ST7735_BLACK);
										updateHealth(player_pxm, wild_pxm, 'p');
									}
									else dodgeAnimation(player_pxm, player_pxm_x, player_pxm_y, ST7735_WHITE, ST7735_BLACK, 'p');
									player_pxm_turn = true;
									// delay(500);
								}

								if (wild_pxm->health <= 0) {
									Serial.println(F("Winner: ")); Serial.println(F("Player\n"));
									deathAnimation(wild_pxm, wild_pxm_x, wild_pxm_y, ST7735_BLACK);
									eraseMenu();
									change_game_mode();
								} else if (player_pxm->health <= 0) {
									Serial.println(F("Winner: ")); Serial.println(F("Wild\n"));
									deathAnimation(player_pxm, player_pxm_x, player_pxm_y, ST7735_BLACK);
									eraseMenu();
									change_game_mode();
								}
							}
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

						int main() {
							setup();
							loadAllPixelmon();
							pixelmon pl; // Player
							generatePixelmon(&pl);
							pixelmon wd; // Wild
							generatePixelmon(&wd);

							updateMap();

							int startTime = millis();
							while (true) {
								if (encounter_wild_pixelmon == 1) {
									change_game_mode();
								}
								scanJoystick(0);
								if (update == 1 && game_mode == 0) {
									updateScreen();
								}
								else if (game_mode == 1) {
									tft.fillScreen(ST7735_BLACK);
									battleMode(&pl, &wd);
								}
								//keep constant framerate
								int timeDiff = millis() - startTime; //time elapsed from start of loop until after refresh
								if (timeDiff < MILLIS_PER_FRAME){
									delay(MILLIS_PER_FRAME - timeDiff);
								}
								startTime = millis();
							}

							Serial.end();
							return 0;
						}
