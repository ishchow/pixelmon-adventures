#include <Arduino.h>
#include "EEPROM.h"

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

  while (true) {}

  return 0;
  Serial.end();
}
