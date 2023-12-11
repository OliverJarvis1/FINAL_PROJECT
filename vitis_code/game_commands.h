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

int game_select(GpoCore *led_p, SsegCore *sseg_p, GpiCore *sw_p, GpiCore *btn_p);

void wordle(GpoCore *led_p, SpiCore *spi_p, Ps2Core *ps2_p, GpiCore *btn_p);

void buttons(SsegCore *sseg_p, SpiCore *spi_p, GpiCore *btn_p);

void switches(GpoCore *led_p, SpiCore *spi_p, GpiCore *sw_p, GpiCore *btn_p);

void switch_button(GpoCore *led_p, SpiCore *spi_p, SsegCore *sseg_p, GpiCore *sw_p, GpiCore *btn_p);

void memory (GpoCore *led_p, SpiCore *spi_p, GpiCore *sw_p, GpiCore *btn_p);
