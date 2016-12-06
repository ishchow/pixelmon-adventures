#include <Arduino.h>
#include "EEPROM.h"

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

void clearEEPROM() {
	for (int i = 0; i < EEPROM.length(); ++i) {
		EEPROM.update(i, 255); // 255 signifies unread
	}
}

int main() {
  init();
  Serial.begin(9600);

  clearEEPROM();
  Serial.println("EEPROM Clear!");
	// Serial.println("EEPROM Contents:");
	// displayEEPROM(EEPROM.length());

  while (true) {}

  return 0;
  Serial.end();
}
