#ifndef PVPBATTLE_H
#define PVPBATTLE_H

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

#include "lcd_image.h"
#include <avr/pgmspace.h> // For PROGMEM
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

/** Writes an int to Serial3, starting from the least-significant
* and finishing with the most significant byte.
*/
void int_to_serial3(int num);

/** Reads an int from Serial3, starting from the least-significant
* and finishing with the most significant byte.
*/
int int_from_serial3();

void pixelmon_to_serial3(pixelmon px);

pixelmon pixelmon_from_serial3();

//server finite state machine to exchange player pixelmon and enemy pixelmon
pixelmon serverFSM( pixelmon player_pxm );

//client finite state machine to exchange player pixelmon and enemy pixelmon
pixelmon clientFSM( pixelmon player_pxm );


#endif
