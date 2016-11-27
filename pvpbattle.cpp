#include "pvpbattle.h"

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

/** Waits for a certain number of bytes on Serial3 or timeout
* @param nbytes: the number of bytes we want
* @param timeout: timeout period (ms); specifying a negative number
*                turns off timeouts (the function waits indefinitely
*                if timeouts are turned off).
* @return True if the required number of bytes have arrived.
*/
bool wait_on_serial3( uint8_t nbytes, long timeout ) {
    unsigned long deadline = millis() + timeout;//wraparound not a problem
    while (Serial3.available()<nbytes && (timeout<0 || millis()<deadline))
    {
    delay(1); // be nice, no busy loop
    }
    return Serial3.available()>=nbytes;
}

// send an int to other arduino beginning with LSB
// must first typecast num to uint due to how the bitshift works for neg. nums
void int_to_serial3(int num) {
    Serial3.write((char) ( (unsigned int) num >> 0));
    Serial3.write((char) ( (unsigned int) num >> 8));
}

// read an int from other arduino starting with LSB
int int_from_serial3() {
    int num = 0;
    num = num | ((int) Serial3.read()) << 0;
    num = num | ((int) Serial3.read()) << 8;
    return num;
}

// call int_to_serial3 4 times to send all pixelmon data
void pixelmon_to_serial3(pixelmon px) {
    int_to_serial3(px.pixelmon_id);
    int_to_serial3(px.health);
    int_to_serial3(px.level);
    int_to_serial3(px.xp);
}

// call int_from_serial3 4 times to get all pixelmon data
pixelmon pixelmon_from_serial3() {
    pixelmon px;
    px.pixelmon_id = int_from_serial3();
    px.health = int_from_serial3();
    px.level = int_from_serial3();
    px.xp = int_from_serial3();
    return px;
}

//server finite state machine to exchange player pixelmon and enemy pixelmon
pixelmon pixelmonServerFSM( pixelmon player_pxm ) {
  enum State { LISTEN = 1, WAITKEY1, WAITACK1, WAITKEY2, WAITACK2, DATAEXC };
  State curr_state = LISTEN;
  // declare variables
  bool ontime;
  long timeout = 1000;
  pixelmon enemy_pxm;
  char getChar;
  while (true) {
    // LISTEN state (1)
    if (curr_state == LISTEN && Serial3.read() == 'C') {
      curr_state = WAITKEY1;
    }
    // WAITKEY1 state (2)
    else if (curr_state == WAITKEY1) {
      ontime = wait_on_serial3(sizeof(pixelmon),timeout);
      if (ontime == false) {
        curr_state = LISTEN;
      }
      else {
        enemy_pxm = pixelmon_from_serial3();
        Serial3.write('A');
        pixelmon_to_serial3(player_pxm);
        curr_state = WAITACK1;
      }
    }
    // WAITACK1 state (3)
    else if (curr_state == WAITACK1) {
      ontime = wait_on_serial3(1,timeout);
      getChar = Serial3.read();
      if (ontime == false) {
        curr_state = LISTEN;
      }
      else if (getChar == 'C') {
        curr_state = WAITKEY2;
      }
      else if (getChar == 'A') {
        curr_state = DATAEXC;
      }
    }
    // WAITKEY2 state (4)
    else if (curr_state == WAITKEY2) {
      ontime = wait_on_serial3(sizeof(pixelmon),timeout);
      if (ontime == false) {
        curr_state = LISTEN;
      }
      else {
        enemy_pxm = pixelmon_from_serial3();
        curr_state = WAITACK2;
      }
    }
    // WAITACK2 state (5)
    else if (curr_state == WAITACK2) {
      ontime = wait_on_serial3(1,timeout);
      getChar = Serial3.read();
      if (ontime == false) {
        curr_state = LISTEN;
      }
      else if (getChar == 'C') {
        curr_state = WAITKEY2;
      }
      else if (getChar == 'A') {
        curr_state = DATAEXC;
      }
    }
    // DATAEXC state
    else if (curr_state == DATAEXC) {
      return enemy_pxm;
    }
  }
}

//client finite state machine to exchange player pixelmon and enemy pixelmon
pixelmon pixelmonClientFSM( pixelmon player_pxm ) {
  enum State { START = 1, WAITACK, DATAEXC };
  State curr_state = START;
  // declare variables
  bool ontime;
  long timeout = 1000;
  pixelmon enemy_pxm;
  while (true) {
    Serial.print("pixcurrstate: ");Serial.println(curr_state);
    // START state
    if (curr_state == START) {
      Serial3.write('C');
      pixelmon_to_serial3(player_pxm);
      curr_state = WAITACK;
    }
    // WAITACK state
    else if (curr_state = WAITACK) {
      ontime = wait_on_serial3(1+sizeof(pixelmon),timeout);
      if (ontime == false) {
        curr_state = START;
      }
      // DATAEXC state
      else if ((char)Serial3.read() == 'A') {
        enemy_pxm = pixelmon_from_serial3();
        Serial3.write('A');
        curr_state = DATAEXC;
        return enemy_pxm;
      }
    }
  }
}

//server finite state machine to exchange player attack id and enemy hp
int integerServerFSM( int integerSent ) {
  enum State { LISTEN = 1, WAITKEY1, WAITACK1, WAITKEY2, WAITACK2, DATAEXC };
  State curr_state = LISTEN;
  // declare variables
  bool ontime;
  long timeout = 1000;
  int integerReceived = 0;
  char getChar;
  while (true) {
    // LISTEN state (1)
    if (curr_state == LISTEN && Serial3.read() == 'C') {
      curr_state = WAITKEY1;
    }
    // WAITKEY1 state (2)
    else if (curr_state == WAITKEY1) {
      ontime = wait_on_serial3(sizeof(int),timeout);
      if (ontime == false) {
        curr_state = LISTEN;
      }
      else {
        integerReceived = int_from_serial3();
        Serial3.write('A');
        int_to_serial3(integerSent);
        curr_state = WAITACK1;
      }
    }
    // WAITACK1 state (3)
    else if (curr_state == WAITACK1) {
      ontime = wait_on_serial3(1,timeout);
      getChar = Serial3.read();
      if (ontime == false) {
        curr_state = LISTEN;
      }
      else if (getChar == 'C') {
        curr_state = WAITKEY2;
      }
      else if (getChar == 'A') {
        curr_state = DATAEXC;
      }
    }
    // WAITKEY2 state (4)
    else if (curr_state == WAITKEY2) {
      ontime = wait_on_serial3(sizeof(int),timeout);
      if (ontime == false) {
        curr_state = LISTEN;
      }
      else {
        integerReceived = int_from_serial3();
        curr_state = WAITACK2;
      }
    }
    // WAITACK2 state (5)
    else if (curr_state == WAITACK2) {
      ontime = wait_on_serial3(1,timeout);
      getChar = Serial3.read();
      if (ontime == false) {
        curr_state = LISTEN;
      }
      else if (getChar == 'C') {
        curr_state = WAITKEY2;
      }
      else if (getChar == 'A') {
        curr_state = DATAEXC;
      }
    }
    // DATAEXC state
    else if (curr_state == DATAEXC) {
      return integerReceived;
    }
  }
}

//client finite state machine to exchange player attack id and enemy hp
int integerClientFSM( int integerSent ) {
  enum State { START = 1, WAITACK, DATAEXC };
  State curr_state = START;
  // declare variables
  bool ontime;
  long timeout = 1000;
  int integerReceived = 0;
  while (true) {
    Serial.print("intcurrstate: ");Serial.println(curr_state);
    // START state
    if (curr_state == START) {
      Serial3.write('C');
      int_to_serial3(integerSent);
      curr_state = WAITACK;
    }
    // WAITACK state
    else if (curr_state = WAITACK) {
      ontime = wait_on_serial3(1+sizeof(int),timeout);
      if (ontime == false) {
        curr_state = START;
      }
      // DATAEXC state
      else if ((char)Serial3.read() == 'A') {
        integerReceived = int_from_serial3();
        Serial3.write('A');
        curr_state = DATAEXC;
        return integerReceived;
      }
    }
  }
}

// complete fxn that uses fightmode and other fxns to conduct a pvp battle
void PVPbattleMode(pixelmon *player_pxm, pixelmon *wild_pxm) {
  uint8_t game_mode = 1;
  int player_pxm_x = 0, player_pxm_y = 0;
  int wild_pxm_x = (TFT_WIDTH - 1) - 32, wild_pxm_y = 0;
  drawPixelmon(player_pxm, player_pxm_x, player_pxm_y, ST7735_WHITE);
  displayPlayerPixelmonStats(player_pxm);
  int selected_option = 0;
  int last_selected_option = 0;
  // battle menu choices
  const char *battle_options[] = {"Fight", "Swap"};
  const int num_battle_options = sizeof(battle_options)/sizeof(battle_options[0]);
  int selected_attack = 0;
  int last_selected_attack = 0;
  int enemy_selected_attack = 0;
  //selected pixelmon var.
  int player_pxm_temp_health = 0;
  int selected_pxm = 0;
  int last_selected_pxm = 0;
  pixelmon *last_player_pxm = player_pxm;
  pixelmon *last_wild_pxm = wild_pxm;
  // turns var.
  bool player_pxm_turn = true;
  char message[64] = {0};
  bool flee = false;
  bool capture = false;
  bool isEnemy = true; // Set for when updating enemy pixelmon
  // Select pixelmon before fight
  sprintf(message, "Please select a pixelmon!");
  showMessage(message);
  while(!allOwnedPixelmonDead()) { // Pick alive pixelmon
    swapMode(&player_pxm, player_pxm_x, player_pxm_y,
      &last_player_pxm, &selected_pxm, &last_selected_pxm, message);
      if (player_pxm->health > 0) break;
    }
    if (digitalRead(13) == HIGH) {
        last_wild_pxm = wild_pxm;
        *wild_pxm = pixelmonServerFSM(*player_pxm);
        player_pxm_turn = false;
    } else {
        last_wild_pxm = wild_pxm;
        *wild_pxm = pixelmonClientFSM(*player_pxm);
        player_pxm_turn = true;
    }
    displayEnemyPixelmonStats(wild_pxm);
    updatePixelmon(wild_pxm_x, wild_pxm_y, wild_pxm, last_wild_pxm, isEnemy);
    //continue battle if one of 2 conditions are met
    while ((player_pxm->health > 0 || !allOwnedPixelmonDead()) && wild_pxm->health > 0) {
      if (player_pxm_turn) { // Player
        uint8_t max_sel = 2;
        displayBattleMenu(battle_options, num_battle_options, selected_option);
        while (true) {
          int press = scanJoystick(&selected_option, game_mode, max_sel);
          if (last_selected_option != selected_option) {
            updateBattleMenu(battle_options, num_battle_options, selected_option, last_selected_option);
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
            &selected_attack, &last_selected_attack, message);
            if (digitalRead(13) == HIGH) {
              integerServerFSM(-100);
              integerServerFSM(selected_attack);
              integerServerFSM(wild_pxm->health);
            }
            else {
              integerClientFSM(-100);
              integerClientFSM(selected_attack);
              integerClientFSM(wild_pxm->health);
            }
          } else if (selected_option == 1) { // Swap
            while(!allOwnedPixelmonDead()) {
              swapMode(&player_pxm, player_pxm_x, player_pxm_y,
                &last_player_pxm, &selected_pxm, &last_selected_pxm, message);
                if (player_pxm->health > 0) break;
              }
              if (digitalRead(13) == HIGH) {
                integerServerFSM(-200);
                last_wild_pxm = wild_pxm;
            		*wild_pxm = pixelmonServerFSM(*player_pxm);
            	} else {
                integerClientFSM(-200);
                last_wild_pxm = wild_pxm;
            		*wild_pxm = pixelmonClientFSM(*player_pxm);
            	}
              updatePixelmon(wild_pxm_x, wild_pxm_y, wild_pxm, last_wild_pxm, isEnemy);
            }
            player_pxm_turn = false;
          }
		else { // Enemy Pixelmon turn
      int incoming_enemy_choice = 0;
      int attack_id = 0;
      //get enemy choice fight or swap
      if (digitalRead(13) == HIGH) {
        incoming_enemy_choice = integerServerFSM(-1);
      }
      else {
        incoming_enemy_choice = integerClientFSM(-1);
      }
      //if enemy choose to swap
      if (incoming_enemy_choice == -200) {
        if (digitalRead(13) == HIGH) {
            last_wild_pxm = wild_pxm;
            *wild_pxm = pixelmonServerFSM(*player_pxm);
        } else {
            last_wild_pxm = wild_pxm;
            *wild_pxm = pixelmonClientFSM(*player_pxm);
        }
        continue;
      }
      // if enemy attack
      if (digitalRead(13) == HIGH) {
        attack_id = integerServerFSM(-1);
      }
      else {
        attack_id = integerClientFSM(-1);
      }
			sprintf(message, "Enemy %s attacks with %s",
					allPixelmon[wild_pxm->pixelmon_id].name,
					allPixelmon[wild_pxm->pixelmon_id].attacks[attack_id].name);
			showMessage(message);
			// calc damage
      if (digitalRead(13) == HIGH) {
        player_pxm_temp_health = integerServerFSM(-1);
      }
      else {
        player_pxm_temp_health = integerClientFSM(-1);
      }
			if (player_pxm_temp_health < player_pxm->health) {
        player_pxm->health = player_pxm_temp_health;
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
		// actions for when a pixelmon (player or enemy) is fainted
		if (wild_pxm->health <= 0) {
			sprintf(message, "Enemy %s fainted!", allPixelmon[wild_pxm->pixelmon_id].name);
			showMessage(message);
			deathAnimation(wild_pxm, wild_pxm_x, wild_pxm_y, ST7735_BLACK);
			player_pxm->xp += (wild_pxm->level)*10;
			sprintf(message, "%s gained %d xp", allPixelmon[player_pxm->pixelmon_id].name, 10*wild_pxm->level);
			showMessage(message);
			if (player_pxm->xp >= 100) {levelUpPixelmon(player_pxm, message);}
      updatePixelmon(player_pxm_x, player_pxm_y, player_pxm, last_player_pxm, !isEnemy);
			eraseMenu();
      if (digitalRead(13) == HIGH) {
          last_wild_pxm = wild_pxm;
          *wild_pxm = pixelmonServerFSM(*player_pxm);
      } else {
          last_wild_pxm = wild_pxm;
          *wild_pxm = pixelmonClientFSM(*player_pxm);
      }
      updatePixelmon(wild_pxm_x, wild_pxm_y, wild_pxm, last_wild_pxm, isEnemy);
		} else if (player_pxm->health <= 0) {
			sprintf(message, "%s fainted!", allPixelmon[player_pxm->pixelmon_id].name);
			showMessage(message);
			deathAnimation(player_pxm, player_pxm_x, player_pxm_y, ST7735_BLACK);
			eraseMenu();
      // swapping out fainted pixelmon
			while(!allOwnedPixelmonDead()) {
				swapMode(&player_pxm, player_pxm_x, player_pxm_y,
					 	 &last_player_pxm, &selected_pxm, &last_selected_pxm, message);
				if (player_pxm->health > 0) break;
			}
      if (digitalRead(13) == HIGH) {
        last_wild_pxm = wild_pxm;
        *wild_pxm = pixelmonServerFSM(*player_pxm);
      } else {
        last_wild_pxm = wild_pxm;
        *wild_pxm = pixelmonClientFSM(*player_pxm);
      }
      updatePixelmon(wild_pxm_x, wild_pxm_y, wild_pxm, last_wild_pxm, isEnemy);
		}
	}
}

// menu to ask if player wants PVP
void displayPVPChallengeMenu() {
  const int TXT_SIZE = 2;
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(TXT_SIZE);
	tft.setTextWrap(false);
	tft.setCursor(18, 0);
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.print(("PVP MODE"));
  tft.setCursor(50, 70);
  tft.print(("YES"));
  tft.setCursor(50, 70 + TXT_SIZE*8);
  tft.print(("NO"));
}

// highlight selection in challenge menu
void updatePVPChallengeMenu(int PVP_choice, int prevPVP_choice, const char *PVPYESNO[]) {
  const int TXT_SIZE = 2;
	tft.setTextSize(TXT_SIZE);
	tft.setTextWrap(false);
	// deselect
	tft.setCursor(50, 70 + prevPVP_choice*TXT_SIZE*8);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.print(PVPYESNO[prevPVP_choice]);
	// select
	tft.setCursor(50, 70 + PVP_choice*TXT_SIZE*8);
	tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
  tft.print(PVPYESNO[PVP_choice]);
}
