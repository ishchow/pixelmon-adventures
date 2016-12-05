#include "highscoretable.h"

int num_scores = 0;

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

void storeNumScores() {
	Serial.print(F("num_scores: ")); Serial.println(num_scores);
	int eeprom_start = 0;
	EEPROM.put(eeprom_start, num_scores);
	Serial.println(F("num_scores stored!"));
}

void getNumScores() {
	int eeprom_start = 0;
	EEPROM.get(eeprom_start, num_scores);
	if (num_scores == -1) { // num_scores == -1 when eeprom is clear
		num_scores = 0;
		storeNumScores();
	}
	Serial.println(F("num_scores retrieved!"));
	Serial.print(F("num_scores: ")); Serial.println(num_scores);
}


// Print highscore table to serial-monitor
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

// Append player name and score to table
void playerToTable(player* current_player) {
	getNumScores();
	// After last entry in table
	int eeprom_add = sizeof(num_scores) + num_scores*sizeof(player);
	EEPROM.put(eeprom_add, *current_player);
	++num_scores;
	storeNumScores();
}

// Retrieves player name from serial monitor
void getPlayerName(player *current_player) {
	const int NAME_LENGTH = sizeof(current_player->name)/sizeof(current_player->name[0]);
	Serial.print(F("Enter your name (three letters only): "));
	char player_name[NAME_LENGTH];
	int i=0;
	while (i<3) {
		while (Serial.available()==0) {}
		int letter = Serial.read();
		// if (!isalpha(letter)) {
		// 	Serial.println(F("Letters only please!"));
		// 	Serial.print(F("Enter your name (three char only): "));
		// }
		if(isalpha(letter)) {
			letter = toupper(letter);
			player_name[i] = letter;
			Serial.print((char)letter);
			++i;
		}
	}
	player_name[NAME_LENGTH-1] = '\0';
	Serial.println();
	strncpy(current_player->name, player_name, NAME_LENGTH);
	// Serial.print("Player name: "); Serial.println(current_player->name);
}

// Generates specified number of elements and adds them to highscore table
// Can overwrite the previous table if specified
void generateTable(int num_elements, bool overwrite_table) {
	if (overwrite_table) { // Overwrite previous table
		clearEEPROM();
		num_scores = 0;
		storeNumScores();
	}
	player some_player;
	const char* names[] = {
		"SAM", "ISH", "VIV", "TAR", "ALI", "CNZ", "SWN", "DIL", "DON", "ARI", "TIM"
	};
	const int NUM_NAMES = sizeof(names)/sizeof(names[0]);
	const int NAME_LENGTH = sizeof(some_player.name)/sizeof(some_player.name[0]);
	for (int i = 0; i < num_elements; ++i) {
		some_player.score = random(101);
		int name_id = random(NUM_NAMES);
		strncpy(some_player.name, names[name_id], NAME_LENGTH);
		playerToTable(&some_player);
	}
}
