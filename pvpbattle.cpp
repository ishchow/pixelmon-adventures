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

/** Writes an int to Serial3, starting from the least-significant
* and finishing with the most significant byte.
*/
void int_to_serial3(int num) {
    Serial3.write((char) (num >> 0));
    Serial3.write((char) (num >> 8));
}

/** Reads an int from Serial3, starting from the least-significant
* and finishing with the most significant byte.
*/
int int_from_serial3() {
    int num = 0;
    num = num | ((uint32_t) Serial3.read()) << 0;
    num = num | ((uint32_t) Serial3.read()) << 8;
    return num;
}

void pixelmon_to_serial3(pixelmon px) {
    int_to_serial3(px.pixelmon_id);
    int_to_serial3(px.health);
    int_to_serial3(px.level);
    int_to_serial3(px.xp);
}

pixelmon pixelmon_from_serial3() {
    pixelmon px;
    px.pixelmon_id = int_from_serial3();
    px.health = int_from_serial3();
    px.level = int_from_serial3();
    px.xp = int_from_serial3();
    return px;
}

pixelmon clientKey(pixelmon player_pxm){
    /*
    If the Arduino is configured as a server, it will fetch the client's public
    key using a Finite State Machine. Lastly, it will return the client's public
    key.
    */
    long timeout = 1000;
    char letter;
    pixelmon opponent_pxm;

    enum State { Listen,    WaitKey1,  WaitAck1,
                 WaitKey2,  WaitAck2,  DataXchange};
    State currentState = Listen;

    // FSM control
    while(1){
        Serial.print("Current state: "); Serial.println(currentState);
        // Wait indefinitely for the first Comm Req
        // This is done in case the client is turned on/reset before the server
        if (currentState == Listen && wait_on_serial3(1, -1)){
            letter = Serial3.read();
            if (letter == 'C') {
                currentState = WaitKey1;
            } else {
                currentState = Listen;
            }
        }
        // Save ckey if it comes through and then send Acknowledge.
        else if (currentState == WaitKey1 && wait_on_serial3(sizeof(pixelmon), timeout)){
            // ckey = uint32_from_serial3();
			opponent_pxm = pixelmon_from_serial3();
            Serial3.write("A");
            // uint32_to_serial3(skey);
			pixelmon_to_serial3(player_pxm);
            currentState = WaitAck1;
        }
        // If Comm Req is received, wait for key
        // Else If Acknowledge is received, start exchange.
        else if (currentState == WaitAck1 && wait_on_serial3(1, timeout)){
            letter = Serial3.read();
            if (letter == 'C') {
               currentState = WaitKey2;
            }
            else if (letter == 'A') {
                currentState = DataXchange;
            }
        }
        // Save ckey again
        else if (currentState == WaitKey2 && wait_on_serial3(sizeof(pixelmon), timeout)){
            // ckey = uint32_from_serial3();
			opponent_pxm = pixelmon_from_serial3();
            currentState = WaitAck2;
        }
        // If Comm Req is received, wait for key
        // Else If Acknowledge is received, start exchange.
        else if (currentState == WaitAck2 && wait_on_serial3(1, timeout)){
            letter = Serial3.read();
            if (letter == 'C') {
                currentState = WaitKey2;
            } else if (letter == 'A'){
                currentState = DataXchange;
            }
        }
        else if (currentState = DataXchange){break;}
        else {
            while (Serial3.available() > 0) {
				Serial3.read();
			}
            currentState = Listen;
        }
    }
    return opponent_pxm;
}
