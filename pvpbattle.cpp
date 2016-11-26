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
        // enemy_pxm.pixelmon_id = int_from_serial3();
        // enemy_pxm.health = int_from_serial3();
        // enemy_pxm.level = int_from_serial3();
        // enemy_pxm.xp = int_from_serial3();
        Serial3.write('A');
        pixelmon_to_serial3(player_pxm);
        // int_to_serial3(player_pxm.pixelmon_id);
        // int_to_serial3(player_pxm.health);
        // int_to_serial3(player_pxm.level);
        // int_to_serial3(player_pxm.xp);
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
        // enemy_pxm.pixelmon_id = int_from_serial3();
        // enemy_pxm.health = int_from_serial3();
        // enemy_pxm.level = int_from_serial3();
        // enemy_pxm.xp = int_from_serial3();
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
    // START state
    if (curr_state == START) {
      Serial3.write('C');
      pixelmon_to_serial3(player_pxm);
      // int_to_serial3(player_pxm.pixelmon_id);
      // int_to_serial3(player_pxm.health);
      // int_to_serial3(player_pxm.level);
      // int_to_serial3(player_pxm.xp);
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
        // enemy_pxm.pixelmon_id = int_from_serial3();
        // enemy_pxm.health = int_from_serial3();
        // enemy_pxm.level = int_from_serial3();
        // enemy_pxm.xp = int_from_serial3();
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
