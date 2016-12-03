#include "battlemode.h"

// Dirty hacks to get rid of compiler errors
extern Adafruit_ST7735 tft;
extern pixelmon_type allPixelmon[];
extern pixelmon ownedPixelmon[];
extern int num_pxm_owned;
extern int scanJoystick(int* selection, uint8_t game_mode, uint8_t max_selection);
static const int TFT_WIDTH = 128; // Had to use this cuz TFT_WIDTH & TFT_HEIGHT
static const int TFT_HEIGHT = 160; // were declared as a define.
                                   // Could change the #defines to const int
                                   // or declare in header

// fxn creates a random pixelmon with random stats
// has a while loop to generate pixelmon based on rarity
void generatePixelmon(pixelmon *px) {
	int rarity;
	if (random(101)>=90) {rarity = 2;}
	else {rarity = 1;}
	while (true) {
		px->pixelmon_id = random(NUM_PIXELMON_TYPES);
		if (allPixelmon[px->pixelmon_id].rarity == rarity) {break;}
	}
	px->level = random(1, 11);
	px->health = 100 + (px->level)*10;
	px->xp = 0;
}

// fxn to check if two pixelmon are same
bool pixelmonEqual(pixelmon *px1, pixelmon *px2) {
	if (px1->pixelmon_id != px2->pixelmon_id) return false;
	else if (px1->health != px2->health) return false;
	else if (px1->level != px2->level) return false;
	else if (px1->xp != px2->xp) return false;
	else return true;
}

// Checks if all owned pixelmon have no health, returns true if this is the case
bool allOwnedPixelmonDead() {
	for (int i = 0; i < num_pxm_owned; ++i) {
		if (ownedPixelmon[i].health > 0) return false;
	}
	return true;
}

// Reset health to max for all owned pixelmon
void healAllOwnedPixelmon() {
	for (int i = 0; i < num_pxm_owned; ++i) {
		ownedPixelmon[i].health = 100 + (ownedPixelmon[i].level)*10;
	}
}

// fxn to show pixelmon stats on serial-mon
void printPixelmon(pixelmon *px) {
	Serial.println();
  Serial.print(F("Pixelmon name: ")); Serial.println(allPixelmon[px->pixelmon_id].name);
  Serial.print(F("\tHealth: ")); Serial.println(px->health);
  Serial.print(F("\tLvl: ")); Serial.println(px->level);
  Serial.print(F("\tXP: ")); Serial.println(px->xp);
  Serial.println();
}

// fxn to draw sprite on screen
void drawPixelmon(pixelmon *px, int16_t x, int16_t y, uint16_t bmp_color) {
	tft.drawBitmap(x, y, allPixelmon[px->pixelmon_id].bitmap, PXM_BMP_WIDTH, PXM_BMP_HEIGHT, bmp_color);
	/* arguments for drawBitmap:
	top left corner
	(deref) pointer to bitmap
	size of .bmp
	color
	*/
}

// fxn to overwrite previously drawn pixelmon
void erasePixelmon(int16_t x, int16_t y, uint16_t bg_color) {
	tft.fillRect(x, y, PXM_BMP_WIDTH, PXM_BMP_HEIGHT, bg_color);
}

// fxn to calc. dmg, returns if true if attack is succesful
bool execAttack(pixelmon *attacker, pixelmon *victim, int attack_id) {
	int prev_health = victim->health;
	int hit_probability = random(0, 101);
	if (hit_probability >= 25) {
		victim->health -= allPixelmon[attacker->pixelmon_id].attacks[attack_id].dmg;
		victim->health = constrain(victim->health, 0, prev_health);
		return true;
	}
	return false;
}

// bmp for pixelmon blinks once
void hitAnimation(pixelmon *injured, int16_t injured_x, int16_t injured_y, uint16_t bmp_color, uint16_t bg_color) {
	delay(500);
	erasePixelmon(injured_x, injured_y, bg_color);
	delay(500);
	drawPixelmon(injured, injured_x, injured_y, bmp_color);
}

// draw pixelmon red then erases
void deathAnimation(pixelmon *killed, int16_t killed_x, int16_t killed_y, uint16_t bg_color) {
	drawPixelmon(killed, killed_x, killed_y, ST7735_RED);
	delay(500);
	erasePixelmon(killed_x, killed_y, bg_color);
	delay(500);
}

// draw/redraw pixelmon rapidly side to side
void dodgeAnimation(pixelmon *px, int16_t x, int16_t y, uint16_t bmp_color,
										uint16_t bg_color, char attacked_pxm)
{
		int start_x = x;
		int last_x = x;
		const int DODGE_LENGTH = 10; // Max distance pixelmon moves during animation
		const int MOVE_SPEED = 2; // How many pixels it moves per iteration
		int direction = 1;  // Direction relative to pixelmon
		// Forward: 1, Reverse: -1
		// player dodge
		if (attacked_pxm == 'p') { // Player
			while (true) {
				if (direction == 1) x = constrain(x+MOVE_SPEED, start_x, start_x + DODGE_LENGTH);
				else if (direction == -1) x = constrain(x-MOVE_SPEED, start_x, start_x + DODGE_LENGTH);
				// draw/redraw
				if (x != last_x) {
					erasePixelmon(last_x, y, ST7735_BLACK); // overwrite last sprite
					delay(50);
					drawPixelmon(px, x, y, ST7735_WHITE); // draw new sprite
					last_x = x;
				}
				// start/end position
				if (x == start_x + DODGE_LENGTH) direction = -1; // Max forward distance
				if (x == start_x && direction == -1) break; // Start position
			}
		} else if (attacked_pxm == 'e') { // Enemy
			while (true) {
				if (direction == 1) x = constrain(x-MOVE_SPEED, start_x - DODGE_LENGTH, start_x);
				else if (direction == -1) x = constrain(x+MOVE_SPEED, start_x - DODGE_LENGTH, start_x);
				// draw/redraw
				if (x != last_x) {
					erasePixelmon(last_x, y, ST7735_BLACK); // overwrite last sprite
					delay(50);
					drawPixelmon(px, x, y, ST7735_WHITE); // draw new sprite
					last_x = x;
				}
				// start/end position
				if (x == start_x - DODGE_LENGTH) direction = -1; // Max forward distance
				if (x == start_x && direction == -1) break; // Start position
			}
		}
	}

// Display player pixelmon stats to left side of screen
void displayPlayerPixelmonStats(pixelmon *player_pxm) {
	tft.setTextSize(1);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	tft.setTextWrap(false);
	// Player pixelmon:
	tft.setCursor(0, 33);
	tft.print(allPixelmon[player_pxm->pixelmon_id].name); tft.print(F("\n"));
	tft.print(F("Life: ")); tft.print(player_pxm->health); tft.print(F("\n"));
	tft.print(F("Lvl: ")); tft.print(player_pxm->level); tft.print(F("\n"));
	tft.print(F("EXP: ")); tft.print(player_pxm->xp);
}

// Display wild pixelmon stats to right side of screen
void displayEnemyPixelmonStats(pixelmon *enemy_pxm) {
	tft.setTextSize(1);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	tft.setTextWrap(false);
	// Wild pixelmon:
	tft.setCursor(TFT_WIDTH/2, 33);
	tft.print(allPixelmon[enemy_pxm->pixelmon_id].name);
	tft.setCursor(TFT_WIDTH/2, 33+8);
	tft.print(F("Life: ")); tft.print(enemy_pxm->health);
	tft.setCursor(TFT_WIDTH/2, 33+16);
	tft.print(F("Lvl: ")); tft.print(enemy_pxm->level);
	tft.setCursor(TFT_WIDTH/2, 33+24);
	tft.print(F("EXP: ")); tft.print(enemy_pxm->xp);
}

// display new health on screen
void updateHealth(pixelmon *player_pxm, pixelmon *enemy_pxm, char hit_pxm) {
	tft.setTextSize(1);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	tft.setTextWrap(false);
	if (hit_pxm == 'p') { // Player
		tft.setCursor(7*5,33+8);
		tft.fillRect(7*5, 33+8, TFT_WIDTH/2 - 7*5, 7, ST7735_BLACK); // Clear previous health
		tft.print(player_pxm->health);
	} else if (hit_pxm == 'e') { // Enemy
		tft.setCursor(TFT_WIDTH/2 + 7*5, 33+8);
		// Clear previous health
		tft.fillRect(TFT_WIDTH/2 + 7*5, 33+8, (TFT_WIDTH-1) - (TFT_WIDTH/2 + 7*5), 7, ST7735_BLACK);
		tft.print(enemy_pxm->health);
	}
}

// print fight moves, based on position of 1st move
void displayFightMenu(pixelmon *player_pxm, int selected_attack) {
	const int TXT_SIZE = 2;
	const int FIRST_MOVE = (TFT_HEIGHT-1) - 4*8*TXT_SIZE;
	tft.setTextSize(TXT_SIZE);
	tft.setTextWrap(false);
	tft.setCursor(0, FIRST_MOVE);
	for (int i = 0; i < 4; ++i) {
		if (i != selected_attack) tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
		else tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
		tft.print(allPixelmon[player_pxm->pixelmon_id].attacks[i].name);
		tft.print(F("\n"));
	}
}

// clear fight/battle menus
void eraseMenu() {
	tft.fillRect(0, PXM_BMP_HEIGHT + 4*8 + 1, TFT_WIDTH, TFT_HEIGHT - 33, ST7735_BLACK);
}

// put previous attack in white letters/black bg; put selected_attack in black letters/white bg
void updateFightMenu(pixelmon *player_pxm, int selected_attack, int last_selected_attack) {
	const int TXT_SIZE = 2;
	tft.setTextSize(TXT_SIZE);
	tft.setTextWrap(false);
	const int FIRST_MOVE = (TFT_HEIGHT-1) - 4*8*TXT_SIZE;
	// deselect
	tft.setCursor(0, FIRST_MOVE + last_selected_attack*TXT_SIZE*8);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	tft.print(allPixelmon[player_pxm->pixelmon_id].attacks[last_selected_attack].name);
	// select
	tft.setCursor(0, FIRST_MOVE + selected_attack*TXT_SIZE*8);
	tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
	tft.print(allPixelmon[player_pxm->pixelmon_id].attacks[selected_attack].name);
}

// make part (where messages appear) of screen black
void eraseDisplayArea() {
	tft.fillRect(0, 33 + 6*7 + 1, TFT_WIDTH, 8*3, ST7735_BLACK);
	// arguments: top x, top y, width, height, color for box
}

// display text to screen
void showMessage(char* message) {
	tft.setCursor(0, 33 + 6*7 + 1); // One lines below last pixelmon stat
	tft.setTextWrap(true);
	tft.setTextSize(1);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	tft.print(message);
	delay(1000);
	eraseDisplayArea();
}

// Shows damage and accuracy of selected move
void displayMoveStats(pixelmon *player_pxm, int selected_attack) {
	tft.setTextWrap(false);
	tft.setTextSize(1);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	// damage
	tft.setCursor(0, 33 + 6*7 + 1);
	tft.print(F("Dmg: "));
	tft.print(allPixelmon[player_pxm->pixelmon_id].attacks[selected_attack].dmg);
	// accuracy
	tft.setCursor(TFT_WIDTH/2, 33 + 6*7 + 1);
	tft.print(F("Acc: "));
	tft.print(75);
}

// Displays damage and accuracy of new move
void updateMoveStats(pixelmon *player_pxm, int selected_attack) {
	tft.setTextWrap(false);
	tft.setTextSize(1);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	// Update damage
	tft.setCursor(6*5, 33 + 6*7 + 1);
	// Clear previous text
	tft.fillRect(6*5, 33 + 6*7 + 1, TFT_WIDTH/2 - 6*5, 7, ST7735_BLACK);
	tft.print(allPixelmon[player_pxm->pixelmon_id].attacks[selected_attack].dmg);
	// Update accuracy
	tft.setCursor(TFT_WIDTH/2 + 6*5, 33 + 6*7 + 1);
	// Clear previous text
	tft.fillRect(TFT_WIDTH/2 + 6*5, 33 + 6*7 + 1, (TFT_WIDTH-1) - TFT_WIDTH/2 + 6*5, 7, ST7735_BLACK);
	tft.print(75);
}

// one of two fxns for fighting; this one does attacks
void fightMode(pixelmon *player_pxm, int player_pxm_x, int player_pxm_y,
			   			 pixelmon *enemy_pxm, int enemy_pxm_x, int enemy_pxm_y,
			   	 		 int *selected_attack, int *last_selected_attack, bool pvpMode, char* message)
{
	uint8_t game_mode = 1;
	uint8_t max_sel = 4;
	displayFightMenu(player_pxm, *selected_attack);
	displayMoveStats(player_pxm, *selected_attack);
	while (true) { //selection of attack while loop
		int press = scanJoystick(selected_attack, game_mode, max_sel);
		if (*last_selected_attack != *selected_attack) {
			updateFightMenu(player_pxm, *selected_attack, *last_selected_attack);
			updateMoveStats(player_pxm, *selected_attack);
			*last_selected_attack = *selected_attack;
		}
		if (press == LOW) {
			press = HIGH;
			*last_selected_attack = *selected_attack;
			eraseMenu();
			eraseDisplayArea();
			break;
		}
	}
	//show the attack selected
	sprintf(message, "%s attacks with %s",
	allPixelmon[player_pxm->pixelmon_id].name,
	allPixelmon[player_pxm->pixelmon_id].attacks[*selected_attack].name);
	showMessage(message);
	// do the attack calc.
	bool enemy_pxm_hit = execAttack(player_pxm, enemy_pxm, *selected_attack);
	if (enemy_pxm_hit) {
		showMessage("Attack successful!");
		hitAnimation(enemy_pxm, enemy_pxm_x, enemy_pxm_y, ST7735_WHITE, ST7735_BLACK);
		updateHealth(player_pxm, enemy_pxm, 'e');
	} else {
    // Display slightly different message depending on whether opponent is
    // a computer or a human opponent
		if (pvpMode) {
			sprintf(message, "Enemy %s dodges!", allPixelmon[enemy_pxm->pixelmon_id].name);
		} else {
			sprintf(message, "Wild %s dodges!", allPixelmon[enemy_pxm->pixelmon_id].name);
		}
		showMessage(message);
		dodgeAnimation(enemy_pxm, enemy_pxm_x, enemy_pxm_y, ST7735_WHITE, ST7735_BLACK, 'e');
	}
}

// show menu for fight, flee, capture, swap
void displayBattleMenu(const char *options[], const int num_options, int selected_option) {
	const int TXT_SIZE = 2;
	const int FIRST_ENTRY = (TFT_HEIGHT-1) - num_options*8*TXT_SIZE;
	tft.setTextSize(TXT_SIZE);
	tft.setCursor(0, FIRST_ENTRY);
	tft.setTextWrap(false);
	for (int i = 0; i < num_options; ++i) {
		if (i != selected_option) tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
		else tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
		tft.print(options[i]);
		tft.print(F("\n"));
	}
}

// highlights selected battle menu option
void updateBattleMenu(const char *options[], const int num_options,
					  					int selected_option, int last_selected_option)
{
	const int TXT_SIZE = 2;
	tft.setTextSize(TXT_SIZE);
	tft.setTextWrap(false);
	const int FIRST_ENTRY = (TFT_HEIGHT-1) - num_options*8*TXT_SIZE;
	// deselect
	tft.setCursor(0, FIRST_ENTRY + last_selected_option*TXT_SIZE*8);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	tft.print(options[last_selected_option]);
	// select
	tft.setCursor(0, FIRST_ENTRY + selected_option*TXT_SIZE*8);
	tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
	tft.print(options[selected_option]);
}

// displays list of all owned pixelmon
void displaySwapMenu(int selected_pxm) {
	const int TXT_SIZE = 1;
	const int FIRST_ENTRY = (TFT_HEIGHT-1) - num_pxm_owned*8*TXT_SIZE;
	tft.setTextSize(TXT_SIZE);
	tft.setTextWrap(false);
	tft.setCursor(0, FIRST_ENTRY);
	for (int i = 0; i < num_pxm_owned; ++i) {
		int px_id = ownedPixelmon[i].pixelmon_id;
		if (i != selected_pxm) tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
		else tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
		tft.print(allPixelmon[px_id].name);
		tft.print(F("\n"));
	}
}

// highlights selected pixelmon
void updateSwapMenu(int selected_pxm, int last_selected_pxm,
										pixelmon *player_pxm, pixelmon *last_player_pxm)
{
	const int TXT_SIZE = 1;
	tft.setTextSize(TXT_SIZE);
	tft.setTextWrap(false);
	const int FIRST_ENTRY = (TFT_HEIGHT-1) - num_pxm_owned*8*TXT_SIZE;
	// deselect
	tft.setCursor(0, FIRST_ENTRY + last_selected_pxm*TXT_SIZE*8);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	tft.print(allPixelmon[last_player_pxm->pixelmon_id].name);
	// select
	tft.setCursor(0, FIRST_ENTRY + selected_pxm*TXT_SIZE*8);
	tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
	tft.print(allPixelmon[player_pxm->pixelmon_id].name);
}

// display stats and bitmap of selected pixelmon
void updatePixelmon(int pxm_x, int pxm_y, pixelmon *pxm, pixelmon *last_pxm, bool isEnemy)
{
	int cursor_x = 0;
	if (isEnemy) cursor_x = TFT_WIDTH/2;
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	tft.setTextWrap(false);
	if (pxm->pixelmon_id != last_pxm->pixelmon_id) {
    //draw bitmap of pixelmon
		erasePixelmon(pxm_x, pxm_y, ST7735_BLACK);
		drawPixelmon(pxm, pxm_x, pxm_y, ST7735_WHITE);
		// display pixelmon name
		tft.setCursor(cursor_x, 33);
		tft.fillRect(cursor_x, 33, TFT_WIDTH/2, 8, ST7735_BLACK); // Clear previous name
		tft.print(allPixelmon[pxm->pixelmon_id].name);
	}
	//health
	tft.fillRect(cursor_x + 7*5, 33+8, TFT_WIDTH/2 - 7*5, 8, ST7735_BLACK); // Clear previous life
  tft.setCursor(cursor_x + 7*5, 33+8); // After "Life: "
	tft.print(pxm->health);
	//level
	tft.fillRect(cursor_x + 6*5, 33+16, TFT_WIDTH/2 - 6*5, 8, ST7735_BLACK); // Clear previous level
  tft.setCursor(cursor_x + 6*5, 33+16); // After "Lvl: "
	tft.print(pxm->level);
	//xp
	tft.fillRect(cursor_x + 6*5, 33+24, TFT_WIDTH/2 - 6*5, 8, ST7735_BLACK); // Clear previous xp
  tft.setCursor(cursor_x + 6*5, 33+24); // After "EXP: "
	tft.print(pxm->xp);
}

// makes pixelmon get xp and level up
void levelUpPixelmon(pixelmon *player_pxm, char *message) {
	sprintf(message, "%s leveled up!", allPixelmon[player_pxm->pixelmon_id].name);
	showMessage(message);
	player_pxm->level += 1;
	player_pxm->xp = 0;
	player_pxm->health = 100 + (player_pxm->level)*10;
}

// Swaps current pixelmon with another in player's inventory
// Double pointers necessary in order to update what the parameter
// pixelmon *player_pxm in battle points to
void swapMode(pixelmon **player_pxm, int player_pxm_x , int player_pxm_y,
		  				pixelmon **last_player_pxm, int* selected_pxm, int* last_selected_pxm, char *message)
{
	uint8_t game_mode = 1;
  if ((*player_pxm)->health == 0) { // Redraw dead pixelmon
    drawPixelmon(*player_pxm, player_pxm_x, player_pxm_y, ST7735_WHITE);
  }
	displaySwapMenu(*selected_pxm);
	while (true) {
		int press = scanJoystick(selected_pxm, game_mode, num_pxm_owned);
		if (*last_selected_pxm != *selected_pxm) {
			*player_pxm = &ownedPixelmon[*selected_pxm];
			updateSwapMenu(*selected_pxm, *last_selected_pxm,
			               *player_pxm, *last_player_pxm);
			bool isEnemy = false;
			updatePixelmon(player_pxm_x, player_pxm_y, *player_pxm, *last_player_pxm, isEnemy);
			*last_selected_pxm = *selected_pxm;
			*last_player_pxm = *player_pxm;
		}
		if (press == LOW) {
			press = HIGH;
			*player_pxm = &ownedPixelmon[*selected_pxm];
			*last_selected_pxm = *selected_pxm;
			*last_player_pxm = *player_pxm;
			eraseMenu();
			break;
		}
	}
	sprintf(message, "You choose %s!", allPixelmon[(*player_pxm)->pixelmon_id].name);
	showMessage(message);
}

// complete fxn that uses fightmode and other fxns to conduct entire battle
void battleMode(pixelmon *player_pxm, pixelmon *wild_pxm) {
	uint8_t game_mode = 1;
	int player_pxm_x = 0, player_pxm_y = 0;
	int wild_pxm_x = (TFT_WIDTH - 1) - 32, wild_pxm_y = 0;
	drawPixelmon(player_pxm, player_pxm_x, player_pxm_y, ST7735_WHITE);
	drawPixelmon(wild_pxm, wild_pxm_x, wild_pxm_y, ST7735_WHITE);
	displayPlayerPixelmonStats(player_pxm);
	displayEnemyPixelmonStats(wild_pxm);
	int selected_option = 0;
	int last_selected_option = 0;
	// battle menu choices
	const char *battle_options[] = {"Fight", "Flee", "Swap", "Capture"};
	const int num_battle_options = sizeof(battle_options)/sizeof(battle_options[0]);
	int selected_attack = 0;
	int last_selected_attack = 0;
	//selected pixelmon var.
	int selected_pxm = 0;
	int last_selected_pxm = 0;
	pixelmon *last_player_pxm = player_pxm;
	bool isEnemy = false; // value to set if enemy stats/bitmap is updated
	bool pvpMode = false; // value to set whether "Wild ... dodges" or "Enemy ... dodges"
	// turns var.
	bool player_pxm_turn = true;
	char message[64] = {0};
	bool flee = false;
  bool capture = false;
	int wild_pxm_max_health = wild_pxm->health; // Store original health
  sprintf(message, "You encounter a wild %s!", allPixelmon[wild_pxm->pixelmon_id].name);
	showMessage(message);
	// Select pixelmon before fight
	sprintf(message, "Select a pixelmon!");
	showMessage(message);
	while(!allOwnedPixelmonDead()) { // Pick alive pixelmon
		swapMode(&player_pxm, player_pxm_x, player_pxm_y,
				 		 &last_player_pxm, &selected_pxm, &last_selected_pxm, message);
		 if (player_pxm->health > 0) break;
	}
	//continue battle if one of 4 conditions are met
	while ((player_pxm->health > 0 || !allOwnedPixelmonDead()) && wild_pxm->health > 0 && !flee && !capture) {
		if (player_pxm_turn) { // Player
			displayBattleMenu(battle_options, num_battle_options, selected_option);
			while (true) {
				int press = scanJoystick(&selected_option, game_mode, num_battle_options);
				if (last_selected_option != selected_option) {
					updateBattleMenu(battle_options, num_battle_options,
									 				 selected_option, last_selected_option);
					last_selected_option = selected_option;
				}
				if (press == LOW) {
					press = HIGH;
					last_selected_option = selected_option;
					eraseMenu();
					break;
				}
			}
			// call fxns based on choice made by player
			if (selected_option == 0) { // Fight
				fightMode(player_pxm, player_pxm_x, player_pxm_y,
								  wild_pxm, wild_pxm_x, wild_pxm_y,
								  &selected_attack, &last_selected_attack, pvpMode, message);
			} else if (selected_option == 1) { // Flee
				flee = true;
				showMessage("You fled!");
			} else if (selected_option == 2) { // Swap
				while(!allOwnedPixelmonDead()) {
					swapMode(&player_pxm, player_pxm_x, player_pxm_y,
									 &last_player_pxm, &selected_pxm, &last_selected_pxm, message);
						if (player_pxm->health > 0) break;
					}
			} else if (selected_option == 3 && num_pxm_owned == MAX_OWNED) {
				sprintf(message, "You can only have %d pixelmon!", MAX_OWNED);
				showMessage(message);
			} else if (selected_option == 3 && num_pxm_owned < MAX_OWNED) { // Capture
				sprintf(message, "You throw a pixelcube at wild %s!", allPixelmon[wild_pxm->pixelmon_id].name);
				showMessage(message);
				erasePixelmon(wild_pxm_x, wild_pxm_y, ST7735_BLACK);
				delay(500);
				int capture_threshold = 100 - map(wild_pxm->health, 0, wild_pxm_max_health, 0, 100);
				Serial.print("capture_threshold: "); Serial.println(capture_threshold);
				int capture_probability = random(101);
				Serial.print("capture_probability: "); Serial.println(capture_probability);
				capture = capture_probability <= capture_threshold;
				if (capture) {
					sprintf(message, "You captured wild %s!", allPixelmon[wild_pxm->pixelmon_id].name);
					showMessage(message);
					num_pxm_owned = constrain(num_pxm_owned + 1, 0, MAX_OWNED);
					// Add captured pixelmon to next empty slot
					ownedPixelmon[num_pxm_owned-1] = *wild_pxm;
				} else {
					sprintf(message, "Wild %s escaped!", allPixelmon[wild_pxm->pixelmon_id].name);
					showMessage(message);
					drawPixelmon(wild_pxm, wild_pxm_x, wild_pxm_y, ST7735_MAGENTA);
					delay(500);
					drawPixelmon(wild_pxm, wild_pxm_x, wild_pxm_y, ST7735_WHITE);
				}
			}
			// end player turn
			player_pxm_turn = false;
		} else { // Wild Pokemon turn
			int attack_id = random(4); // Pick random move
			// wild attacks
			sprintf(message, "Wild %s attacks with %s",
							allPixelmon[wild_pxm->pixelmon_id].name,
							allPixelmon[wild_pxm->pixelmon_id].attacks[attack_id].name);
			showMessage(message);
			// calc damage
			bool player_pxm_hit = execAttack(wild_pxm, player_pxm, attack_id);
			if (player_pxm_hit) {
				sprintf(message, "%s is hit!", allPixelmon[player_pxm->pixelmon_id].name);
				showMessage(message);
				hitAnimation(player_pxm, player_pxm_x, player_pxm_y, ST7735_WHITE, ST7735_BLACK);
				updateHealth(player_pxm, wild_pxm, 'p');
			} else {
				sprintf(message, "%s dodges!", allPixelmon[player_pxm->pixelmon_id].name);
				showMessage(message);
				dodgeAnimation(player_pxm, player_pxm_x, player_pxm_y, ST7735_WHITE, ST7735_BLACK, 'p');
			}
			player_pxm_turn = true;
		}
		// actions for when a pixelmon (player or wild) is fainted
		if (wild_pxm->health <= 0) {
			sprintf(message, "Wild %s fainted!", allPixelmon[wild_pxm->pixelmon_id].name);
			showMessage(message);
			deathAnimation(wild_pxm, wild_pxm_x, wild_pxm_y, ST7735_BLACK);
			player_pxm->xp += (wild_pxm->level)*10;
			sprintf(message, "%s gained %d xp", allPixelmon[player_pxm->pixelmon_id].name, 10*wild_pxm->level);
			showMessage(message);
      // Show xp gain wheter or not pixelmon levels up
      updatePixelmon(player_pxm_x, player_pxm_y, player_pxm, last_player_pxm, isEnemy);
			if (player_pxm->xp >= 100) {
				levelUpPixelmon(player_pxm, message);
				bool isEnemy = false;
        // Show stats (most importantly level) after level up
        updatePixelmon(player_pxm_x, player_pxm_y, player_pxm, last_player_pxm, isEnemy);
			}
			eraseMenu();
		} else if (player_pxm->health <= 0) {
			sprintf(message, "%s fainted!", allPixelmon[player_pxm->pixelmon_id].name);
			showMessage(message);
			deathAnimation(player_pxm, player_pxm_x, player_pxm_y, ST7735_BLACK);
			eraseMenu();
			// last_player_pxm->pixelmon_id = -1; // Garbage values for update pixelmon
			while(!allOwnedPixelmonDead()) {
				swapMode(&player_pxm, player_pxm_x, player_pxm_y,
					 	 		 &last_player_pxm, &selected_pxm, &last_selected_pxm, message);
				if (player_pxm->health > 0) break;
			}
		}
	}
}
