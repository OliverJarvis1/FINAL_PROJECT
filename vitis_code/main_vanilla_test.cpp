/*****************************************************************//**
 * @file main_vanilla_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "spi_core.h"
#include "ps2_core.h"
#include "sseg_core.h"

#include "game_commands.h"
#include "math.h"
#include "string.h"





// instantiate led, spi
GpoCore  led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore  sw(get_slot_addr(BRIDGE_BASE, S3_SW));
SpiCore  spi(get_slot_addr(BRIDGE_BASE, S9_SPI));
GpiCore  btn(get_slot_addr(BRIDGE_BASE, S7_BTN));
Ps2Core  ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));

// main function
// uses game select function (from game_commands.cpp) to select game
// selects game using output and plays it by calling respective function
// all code is in game_commands.cpp

int main() {
	int b;
   while (1) {
	   b = game_select(&led, &sseg, &sw, &btn);
	   if(b == 4) {
		   //wordle - BROKEN, so commented out.
		   //ISSUE: SP2 does not communicate with keyboard
		   //wordle(&led, &spi, &ps2, &btn);
	   } else if (b==3) {
		   //switches
		   switches(&led, &spi, &sw, &btn);
	   } else if (b==2) {
		   //buttons
		   buttons(&sseg, &spi, &btn);

	   } else if (b==1) {
		   //switch and button
		   switch_button(&led, &spi, &sseg, &sw, &btn);
	   } else if (b==0) {
		   //memory
		   memory(&led, &spi, &sw, &btn);

	   }

   } //while
} //main


