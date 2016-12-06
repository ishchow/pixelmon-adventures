#include "highscoretable.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

extern Adafruit_ST7735 tft;

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
// Can overwrite the previous table if desired
void generateTable(int num_elements, bool overwrite_table) {
	if (overwrite_table) { // Overwrite previous table
		clearEEPROM();
		num_scores = 0; // is == -1 after clearing, so must reset
		storeNumScores();
	}
	player some_player;
	const char* names[] = {
		"SAM", "ISH", "VIV", "TAR", "ALI", "CNZ", "SHN", "DIL", "DON", "ARI", "TIM",
		"CAR", "PRO", "ALT"
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

// Swap two players of highscore_table struct
void swapPlayer(player *ptr_player1, player *ptr_player2) {
    player tmp = *ptr_player1;
    *ptr_player1 = *ptr_player2;
    *ptr_player2 = tmp;
}

int partition(player *highscore_table, int len, int pivot_idx) {
  // 1. Put away the pivot by swapping it with the last element of
  //    the array.
  // 2. Search from both ends to out-of-order elements in the subarray
  //    array[0:len-2] by repeating:
  //    2.1 the low-scanner searches for first high element from
  //        the beginning of the array but do not go beyond the
  //        high-scanner's position
  //    2.2 the high-scanner searches for first low element from
  //        the end of the array but do not go beyond the
  //        low-scanner's position
  //    2.3 swap if found a pair of eligible elements, move scanners
  // 3. Put the pivot back to its final position,
  //    return the final position.

  swapPlayer(&highscore_table[pivot_idx], &highscore_table[len-1]);
  pivot_idx = len - 1;

  int low_idx = 0;
  int high_idx = len - 2;

  while (true) {
    // INVARIANT: for 0 <= k <= low_idx; a[k] <= a[pivot_idx]
    //            for high_idx <= j <= len-1: a[j] > a[pivot_idx]
      if (highscore_table[low_idx].score <= highscore_table[pivot_idx].score && low_idx < high_idx) {
          ++low_idx;
      }
      if (highscore_table[high_idx].score > highscore_table[pivot_idx].score && high_idx > low_idx) {
          --high_idx;
      }
      if (highscore_table[low_idx].score > highscore_table[pivot_idx].score &&
          highscore_table[high_idx].score <= highscore_table[pivot_idx].score) {
          swapPlayer(&highscore_table[low_idx], &highscore_table[high_idx]);
          ++low_idx;
          --high_idx;
      }
      if (low_idx >= high_idx) {
          if (highscore_table[high_idx].score > highscore_table[pivot_idx].score) {
              swapPlayer(&highscore_table[pivot_idx], &highscore_table[high_idx]);
              pivot_idx = high_idx;
          } else {
              swapPlayer(&highscore_table[pivot_idx], &highscore_table[high_idx + 1]);
              pivot_idx = high_idx + 1;
          }
          break;
      }
  }

  return pivot_idx;
}

// Sorts the highscore_table in increasing order
void qsort(player *highscore_table, int len) {
    if (len > 1) {
        int pivot_idx = len/2;
        pivot_idx = partition( highscore_table, len, pivot_idx );

        qsort( highscore_table, pivot_idx );
        qsort( highscore_table+pivot_idx+1, len-pivot_idx-1 );
    }
}

// use selection sort to sort the players in highscore_table[]
// in increasing order
// Running time: O(n^2)
void ssort(player *highscore_table, int len) {
  // i is the barrier: we want to sort highscore_table[0], ..., highscore_table[i]
  // Invariant: highscore_table[i+1], ..., highscore_table[n-1] are the n-i-1 largest
  // numbers in the array, and appear in sorted order
  for (int i = len-1; i > 0; --i) {
    int max_index = 0;
    // Invariant: highscore_table[max_index] >= highscore_table[0],
		// highscore_table[1], ..., highscore_table[j-1]
    for (int j = 1; j <= i; ++j) {
      if (highscore_table[j].score > highscore_table[max_index].score) {
        max_index = j;
      }
    }

    // puts the largest value among highscore_table[0], highscore_table[1], ..., highscore_table[i]
    // at the end
    swapPlayer(&highscore_table[i], &highscore_table[max_index]);
  }

  // post-condition: now highscore_table[1], ..., highscore_table[n-1] are the n-1
  // largest items and appear in sorted order
  // so highscore_table[0] <= highscore_table[1] as well and the array is sorted
}

void highscoreTableToTFT() {
	getNumScores();
	player highscore_table[num_scores];
	for (int eeprom_add = sizeof(num_scores), i = 0;
			 eeprom_add < sizeof(num_scores) + num_scores*sizeof(player), i < num_scores;
			 eeprom_add += sizeof(player), ++i)
	{
		EEPROM.get(eeprom_add, highscore_table[i]);
	}
	// Sort in increasing order
	if (num_scores > 20) qsort(highscore_table, num_scores);
	else ssort(highscore_table, num_scores);
	// for(int i = num_scores-1; i >= 0; --i) {
	// 	Serial.print(F("Name: ")); Serial.print(highscore_table[i].name);
	// 	Serial.print(F(" ")); Serial.println(highscore_table[i].score);
	// }
	tft.setTextSize(1);
	tft.setTextWrap(false);
	tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
	tft.setCursor(0, 0);
	tft.print(F("\t\t\t\tTHE VERY BEST\t\t\t\t\n"));
	const int NUM_SCORES_DISPLAYED = 18;
	const int FIRST_ENTRY = 160 - NUM_SCORES_DISPLAYED*8;
	for (int i = num_scores-1, j = 0; i >= min(0,NUM_SCORES_DISPLAYED), j < NUM_SCORES_DISPLAYED;
			 --i, ++j)
  {
		tft.setCursor(0, FIRST_ENTRY + j*8);
		tft.print(highscore_table[i].name);
		tft.setCursor(128/2, FIRST_ENTRY + j*8);
		tft.print(highscore_table[i].score);
	}
}
