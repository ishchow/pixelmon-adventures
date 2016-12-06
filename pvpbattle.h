#ifndef PVPBATTLE_H
#define PVPBATTLE_H

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <avr/pgmspace.h> // For PROGMEM and F()
#include "pixelmondata.h"
#include "battlemode.h"

/** Waits for a certain number of bytes on Serial3 or timeout
* @param nbytes: the number of bytes we want
* @param timeout: timeout period (ms); specifying a negative number
*                turns off timeouts (the function waits indefinitely
*                if timeouts are turned off).
* @return True if the required number of bytes have arrived.
*/
bool wait_on_serial3( uint8_t nbytes, long timeout );

// send an int to other arduino beginning with LSB
// must first typecast num to uint due to how the bitshift works for neg. nums
void int_to_serial3(int num);

// read an int from other arduino starting with LSB
int int_from_serial3();

// call int_from_serial3 4 times to send all pixelmon data
void pixelmon_to_serial3(pixelmon px);

// call int_from_serial3 4 times to get all pixelmon data
pixelmon pixelmon_from_serial3();

//server finite state machine to exchange player pixelmon and enemy pixelmon
pixelmon pixelmonServerFSM( pixelmon player_pxm );

//client finite state machine to exchange player pixelmon and enemy pixelmon
pixelmon pixelmonClientFSM( pixelmon player_pxm );

//server finite state machine to exchange player attack id and enemy hp
int integerServerFSM( int integerSent );

//client finite state machine to exchange player attack id and enemy hp
int integerClientFSM( int integerSent );

// complete fxn that uses fightmode and other fxns to conduct a pvp battle
void PVPbattleMode(pixelmon *player_pxm, pixelmon *enemy_pxm, player *current_player));

// menu to ask if player wants PVP
void displayPVPChallengeMenu(int PVP_choice, const char *PVPYESNO[]);

// highlight selection in challenge menu
void updatePVPChallengeMenu(int PVP_choice, int prevPVP_choice, const char *PVPYESNO[]);

#endif
