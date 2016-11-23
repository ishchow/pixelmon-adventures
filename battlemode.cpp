#include "battlemode.h"

// Dirty hacks to get rid of compiler errors
extern pixelmon_type allPixelmon[];
extern pixelmon ownedPixelmon[];
extern Adafruit_ST7735 tft;
extern int scanJoystick(int* selection, uint8_t game_mode, uint8_t max_selection);
static const int TFT_WIDTH = 128; // Had to use this cuz TFT_WIDTH & TFT_HEIGHT
static const int TFT_HEIGHT = 160; // were declared as a define.
                                   // Could change the #defines to const int
                                   // or declare in header

void generatePixelmon(pixelmon *px) {
		px->pixelmon_id = random(NUM_PIXELMON_TYPES);
		px->health = random(75, 101);
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

void printPixelmon(pixelmon *px) {
    Serial.println();
    Serial.print("Pixelmon name: "); Serial.println(allPixelmon[px->pixelmon_id].name);
    Serial.print("\tHealth: "); Serial.println(px->health);
    Serial.print("\tLvl: "); Serial.println(px->level);
    Serial.print("\tXP: "); Serial.println(px->xp);
    Serial.println();
}

void drawPixelmon(pixelmon *px, int16_t x, int16_t y, uint16_t bmp_color) {
	tft.drawBitmap(x, y, allPixelmon[px->pixelmon_id].bitmap, PXM_BMP_WIDTH, PXM_BMP_HEIGHT, bmp_color);
}

void erasePixelmon(int16_t x, int16_t y, uint16_t bg_color) {
	tft.fillRect(x, y, PXM_BMP_WIDTH, PXM_BMP_HEIGHT, bg_color);
}

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

void hitAnimation(pixelmon *injured, int16_t injured_x, int16_t injured_y, uint16_t bmp_color, uint16_t bg_color) {
	delay(500);
	erasePixelmon(injured_x, injured_y, bg_color);
	delay(500);
	drawPixelmon(injured, injured_x, injured_y, bmp_color);
}

void deathAnimation(pixelmon *killed, int16_t killed_x, int16_t killed_y, uint16_t bg_color) {
	drawPixelmon(killed, killed_x, killed_y, ST7735_RED);
	delay(500);
	erasePixelmon(killed_x, killed_y, bg_color);
	delay(500);
}

void dodgeAnimation(pixelmon *px, int16_t x, int16_t y, uint16_t bmp_color,
					uint16_t bg_color, char attacked_pxm) {
	int start_x = x;
	int last_x = x;
	const int DODGE_LENGTH = 10;
	const int MOVE_SPEED = 2;
	int direction = 1;  // Direction relative to pokemon
						// Forward: 1, Reverse: -1

	if (attacked_pxm == 'p') {
		while (true) {
			if (direction == 1) x = constrain(x+MOVE_SPEED, start_x, start_x + DODGE_LENGTH);
			else if (direction == -1) x = constrain(x-MOVE_SPEED, start_x, start_x + DODGE_LENGTH);

			if (x != last_x) {
				erasePixelmon(last_x, y, ST7735_BLACK);
				delay(50);
				drawPixelmon(px, x, y, ST7735_WHITE);
				last_x = x;
			}

			if (x == start_x + DODGE_LENGTH) direction = -1;
			if (x == start_x && direction == -1) break;
		}
	} else if (attacked_pxm == 'w') {
		while (true) {
			if (direction == 1) x = constrain(x-MOVE_SPEED, start_x - DODGE_LENGTH, start_x);
			else if (direction == -1) x = constrain(x+MOVE_SPEED, start_x - DODGE_LENGTH, start_x);

			if (x != last_x) {
				erasePixelmon(last_x, y, ST7735_BLACK);
				delay(50);
				drawPixelmon(px, x, y, ST7735_WHITE);
				last_x = x;
			}

			if (x == start_x - DODGE_LENGTH) direction = -1;
			if (x == start_x && direction == -1) break;
		}
	}
}

void displayPixelmonStats(pixelmon *player_pxm, pixelmon *wild_pxm) {
	tft.setTextSize(1);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);

	// Player pixelmon:
	tft.setCursor(0, 33);
	tft.print(allPixelmon[player_pxm->pixelmon_id].name); tft.print(F("\n"));
	tft.print(F("Life: ")); tft.print(player_pxm->health); tft.print(F("\n"));
	tft.print(F("Lvl: ")); tft.print(player_pxm->level); tft.print(F("\n"));
	tft.print(F("XP: ")); tft.print(player_pxm->xp);

	// Wild pixelmon:
	tft.setCursor(TFT_WIDTH/2, 33);
	tft.print(allPixelmon[wild_pxm->pixelmon_id].name);
	tft.setCursor(TFT_WIDTH/2, 33+8);
	tft.print(F("Life: ")); tft.print(wild_pxm->health);
	tft.setCursor(TFT_WIDTH/2, 33+16);
	tft.print(F("Lvl: ")); tft.print(wild_pxm->level);
	tft.setCursor(TFT_WIDTH/2, 33+24);
	tft.print(F("XP: ")); tft.print(player_pxm->xp);
}

void updateHealth(pixelmon *player_pxm, pixelmon *wild_pxm, char hit_pxm) {
	tft.setTextSize(1);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	tft.setTextWrap(false);
	if (hit_pxm == 'p') {
		tft.setCursor(0,33+8);
		tft.print(F("Life: "));
		tft.fillRect(7*5, 33+8, 3*5, 7, ST7735_BLACK); // Clear previous health
		tft.print(player_pxm->health);
	} else if (hit_pxm == 'w') {
		tft.setCursor(TFT_WIDTH/2, 33+8);
		tft.print(F("Life: "));
		tft.fillRect(TFT_WIDTH/2 + 7*5, 33+8, 3*5, 7, ST7735_BLACK); // Clear previous health
		tft.print(wild_pxm->health);
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
		tft.print(F("\n"));
	}
}

void eraseMenu() {
	const int TXT_SIZE = 2;
	const int FIRST_ENTRY = (TFT_HEIGHT-1) - 4*8*TXT_SIZE;
	tft.fillRect(0, FIRST_ENTRY, TFT_WIDTH, TFT_HEIGHT - FIRST_ENTRY, ST7735_BLACK);
}

void updateFightMenu(pixelmon *player_pxm, int selected_attack, int last_selected_attack) {
	const int TXT_SIZE = 2;
	tft.setTextSize(TXT_SIZE);
	const int FIRST_MOVE = (TFT_HEIGHT-1) - 4*8*TXT_SIZE;

	tft.setCursor(0, FIRST_MOVE + last_selected_attack*TXT_SIZE*8);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	tft.print(allPixelmon[player_pxm->pixelmon_id].attacks[last_selected_attack].name);

	tft.setCursor(0, FIRST_MOVE + selected_attack*TXT_SIZE*8);
	tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
	tft.print(allPixelmon[player_pxm->pixelmon_id].attacks[selected_attack].name);
}

void eraseDisplayArea() {
	tft.fillRect(0, 33 + 6*7 + 1, TFT_WIDTH, 8*2, ST7735_BLACK);
}

void showMessage(char* message) {
	tft.setCursor(0, 33 + 6*7 + 1);
	tft.setTextWrap(true);
	tft.setTextSize(1);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	tft.print(message);
	delay(1000);
	eraseDisplayArea();
}

void displayMoveStats(pixelmon *player_pxm, int selected_attack) {
	tft.setTextWrap(false);
	tft.setTextSize(1);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);

	tft.setCursor(0, 33 + 6*7 + 1);
	tft.print("Dmg: ");
	tft.print(allPixelmon[player_pxm->pixelmon_id].attacks[selected_attack].dmg);

	tft.setCursor(TFT_WIDTH/2, 33 + 6*7 + 1);
	tft.print("Acc: ");
	tft.print(75);
}

void updateMoveStats(pixelmon *player_pxm, int selected_attack) {
	tft.setTextWrap(false);
	tft.setTextSize(1);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);

	tft.setCursor(6*5, 33 + 6*7 + 1);
	// Clear previous text
	tft.fillRect(6*5, 33 + 6*7 + 1, TFT_WIDTH/2 - 6*5, 7, ST7735_BLACK);
	tft.print(allPixelmon[player_pxm->pixelmon_id].attacks[selected_attack].dmg);


	tft.setCursor(TFT_WIDTH/2 + 6*5, 33 + 6*7 + 1);
	// Clear previous text
	tft.fillRect(TFT_WIDTH/2 + 6*5, 33 + 6*7 + 1, (TFT_WIDTH-1) - TFT_WIDTH/2 + 6*5, 7, ST7735_BLACK);
	tft.print(75);
}

void fightMode(pixelmon *player_pxm, int player_pxm_x, int player_pxm_y,
			   pixelmon *wild_pxm, int wild_pxm_x, int wild_pxm_y,
			   int *selected_attack, int *last_selected_attack, char* message)
{
    uint8_t game_mode = 1;
    uint8_t max_sel = 4;
	displayFightMenu(player_pxm, *selected_attack);
	displayMoveStats(player_pxm, *selected_attack);
	while (true) {
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

	sprintf(message, "%s attacks with %s",
			allPixelmon[player_pxm->pixelmon_id].name,
			allPixelmon[player_pxm->pixelmon_id].attacks[*selected_attack].name);
	showMessage(message);

	bool wild_pxm_hit = execAttack(player_pxm, wild_pxm, *selected_attack);
	if (wild_pxm_hit) {
		showMessage("Attack succesful!");
		hitAnimation(wild_pxm, wild_pxm_x, wild_pxm_y, ST7735_WHITE, ST7735_BLACK);
		updateHealth(player_pxm, wild_pxm, 'w');
	} else {
		sprintf(message, "Wild %s dodges!", allPixelmon[wild_pxm->pixelmon_id].name);
		showMessage(message);
		dodgeAnimation(wild_pxm, wild_pxm_x, wild_pxm_y, ST7735_WHITE, ST7735_BLACK, 'w');
	}
}

void displayBattleMenu(const char *options[], int selected_option) {
	const int TXT_SIZE = 2;
	const int FIRST_ENTRY = (TFT_HEIGHT-1) - 4*8*TXT_SIZE;
	tft.setTextSize(TXT_SIZE);
	tft.setCursor(0, FIRST_ENTRY);
	for (int i = 0; i < 4; ++i) {
		if (i != selected_option) tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
		else tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
		tft.print(options[i]);
		tft.print(F("\n"));
	}
}

void updateBattleMenu(const char *options[], int selected_option, int last_selected_option) {
	const int TXT_SIZE = 2;
	tft.setTextSize(TXT_SIZE);
	const int FIRST_MOVE = (TFT_HEIGHT-1) - 4*8*TXT_SIZE;

	tft.setCursor(0, FIRST_MOVE + last_selected_option*TXT_SIZE*8);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	tft.print(options[last_selected_option]);

	tft.setCursor(0, FIRST_MOVE + selected_option*TXT_SIZE*8);
	tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
	tft.print(options[selected_option]);
}

void battleMode(pixelmon *player_pxm, pixelmon *wild_pxm) {
    uint8_t game_mode = 1;
	int player_pxm_x = 0, player_pxm_y = 0;
	int wild_pxm_x = (TFT_WIDTH - 1) - 32, wild_pxm_y = 0;
	drawPixelmon(player_pxm, player_pxm_x, player_pxm_y, ST7735_WHITE);
	drawPixelmon(wild_pxm, wild_pxm_x, wild_pxm_y, ST7735_WHITE);
	displayPixelmonStats(player_pxm, wild_pxm);

	int selected_option = 0;
	int last_selected_option = 0;

	const char *options[] = {"Fight", "Flee", "Capture", "Swap"};
	int selected_attack = 0;
	int last_selected_attack = 0;

    bool player_pxm_turn = true;
	char message[64] = {0};
	bool flee = false;
    bool capture = false;

	while ((player_pxm->health > 0 && wild_pxm->health > 0) && !flee && !capture) {
		if (player_pxm_turn) { // Player
            uint8_t max_sel = 4;
			displayBattleMenu(options, selected_option);
			while (true) {
				int press = scanJoystick(&selected_option, game_mode, max_sel);
				if (last_selected_option != selected_option) {
					updateBattleMenu(options, selected_option, last_selected_option);
					last_selected_option = selected_option;
				}
				if (press == LOW) {
					press = HIGH;
					last_selected_option = selected_option;
					eraseMenu();
					break;
				}
			}

			if (selected_option == 0) { // Fight
				fightMode(player_pxm, player_pxm_x, player_pxm_y,
						  wild_pxm, wild_pxm_x, wild_pxm_y,
						  &selected_attack, &last_selected_attack, message);
			} else if (selected_option == 1) { // Flee
				flee = true;
				showMessage("You fled!");
			} else if (selected_option == 2) { // Capture
                sprintf(message, "You throw a pokeball at wild %s!", allPixelmon[wild_pxm->pixelmon_id].name);
                showMessage(message);
                erasePixelmon(wild_pxm_x, wild_pxm_y, ST7735_BLACK);
                delay(500);
                capture = random(101) <= 25;
                if (capture) {
                    sprintf(message, "You captured wild %s!", allPixelmon[wild_pxm->pixelmon_id].name);
                    showMessage(message);
                    ownedPixelmon[1] = *wild_pxm;
                } else {
                    sprintf(message, "Wild %s escaped!", allPixelmon[wild_pxm->pixelmon_id].name);
                    showMessage(message);
                    drawPixelmon(wild_pxm, wild_pxm_x, wild_pxm_y, ST7735_MAGENTA);
                    delay(500);
                    drawPixelmon(wild_pxm, wild_pxm_x, wild_pxm_y, ST7735_WHITE);
                }
            } else if (selected_option == 3) { // Swap

            }

			player_pxm_turn = false;
		} else { // Wild Pokemon
			int attack_id = random(4); // Pick random move

			sprintf(message, "Wild %s attacks with %s",
					allPixelmon[wild_pxm->pixelmon_id].name,
					allPixelmon[wild_pxm->pixelmon_id].attacks[attack_id].name);
			showMessage(message);

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

		// wild_pxm->health = 0;

		if (wild_pxm->health <= 0) {
			sprintf(message, "Wild %s fainted!", allPixelmon[wild_pxm->pixelmon_id].name);
			showMessage(message);
			deathAnimation(wild_pxm, wild_pxm_x, wild_pxm_y, ST7735_BLACK);
			eraseMenu();
		} else if (player_pxm->health <= 0) {
			sprintf(message, "%s fainted!", allPixelmon[player_pxm->pixelmon_id].name);
			showMessage(message);
			deathAnimation(player_pxm, player_pxm_x, player_pxm_y, ST7735_BLACK);
			eraseMenu();
		}
	}

}