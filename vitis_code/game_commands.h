/*****************************************************************//**
 * @file game_commands.h
 *
 * @brief commands calls for WORDLE, BOP_IT, REACTION LIGHTS and SOUND games.
 *
 * @author Oliver Jarvis
 * @version v1.0: initial release
 ********************************************************************/

//#ifndef _GAME_COMMANDS_H_INCLUDED
//#define _GAME_COMMANDS_H_INCLUDED

#include "chu_init.h"
#include "gpio_cores.h"
#include "spi_core.h"
#include "ps2_core.h"
#include "sseg_core.h"

/* Game select
 * Upper button selects game
 * Centre button returns game picked
 * Switches determine game
 * Most significant game is leftmost, so if multiple sw are on, leftmost is picked
 */
int game_select(GpoCore *led_p, SsegCore *sseg_p, GpiCore *sw_p, GpiCore *btn_p);

/* wordle game
 * uses acc for randomizer
 * 6 guesses to guess a 5 letter word.
 * Correct letters light LED
 * Letters contained in word make LED flash.
 * Centre button submits answer only when 5 characters selected
 * Space is backspace
 */
void wordle(GpoCore *led_p, SpiCore *spi_p, Ps2Core *ps2_p, GpiCore *btn_p);

/*  Hit 10 buttons is randomly generated order
 *  Starts after 2 seconds.
 *  Order is given by 7sseg
 *  Hitting wrong button/taking time removes score
 */
void buttons(SsegCore *sseg_p, SpiCore *spi_p, GpiCore *btn_p);


/* Switches
 * Hit switches is randomly generated order
 * LED turns on/off for indicated switch
 * Records score and prints to UART
 *
 */
void switches(GpoCore *led_p, SpiCore *spi_p, GpiCore *sw_p, GpiCore *btn_p);

/* switch_button
 * Hit buttons and LEDs in given order
 * LED turns on/off for indicated switch
 * BTN order given by 7sseg
 * Records score and prints to UART
 *
 */
void switch_button(GpoCore *led_p, SpiCore *spi_p, SsegCore *sseg_p, GpiCore *sw_p, GpiCore *btn_p);


/* Memory
 * given an LED order, match LEDs with switches
 * Centre button starts. ALL SW MUST BE DOWN TO START.
 */
void memory (GpoCore *led_p, SpiCore *spi_p, GpiCore *sw_p, GpiCore *btn_p);
