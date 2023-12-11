/*****************************************************************//**
 * @file game_commands.cpp
 *
 * @brief code for WORDLE, BOP_IT, REACTION LIGHTS and SOUND games.
 *
 * @author Oliver Jarvis
 * @version v1.0: initial release
 ********************************************************************/

#include "game_commands.h"
#include "gpio_cores.h"
#include "spi_core.h"
#include "ps2_core.h"
#include "sseg_core.h"
#include "math.h"
#include "cstdlib"









int game_select(GpoCore *led_p, SsegCore *sseg_p, GpiCore *sw_p, GpiCore *btn_p) {

	int output = 10;
	uint32_t s, b;
	bool loop = true;
	bool game_selected = false;

	for (int i=0; i<8; i++) {
		sseg_p->write_1ptn(0xff,i);
	}
	led_p->write(0);

	while (loop) {
		s = sw_p->read();
		b = btn_p->read();
		if ((b%32)>= 16) {
			//btn_c pressed, starts game if game selected
			if (game_selected) {
				loop = false;
			}
		}
		if ((b%16)>=8) {
			//btn_l pressed
		}
		if ((b%8)>=4) {
			//btn_d pressed
		}
		if ((b%4)>=2) {
			//btn_r pressed
		}
		if ((b%2)==1) {
			//btn_u pressed, game select
			if ((s%32)>= 16) {
				//switch 4, WORDLE
				if (output != 4) {
					uart.disp("Game Selected: WORDLE\n\r");
				}
				output = 4;
				led_p->write(0,0);
				led_p->write(0,1);
				led_p->write(0,2);
				led_p->write(0,3);
				led_p->write(1,output);
				game_selected = true;
			}
			else if ((s%16)>=8) {
				//switch 3, SWITCHES
				if(output != 3) {
					uart.disp("Game Selected: SWITCHES\n\r");
				}
				output = 3;
				led_p->write(0,0);
				led_p->write(0,1);
				led_p->write(0,2);
				led_p->write(1,output);
				led_p->write(0,4);
				game_selected = true;
			}
			else if ((s%8)>=4) {
				//switch 2, REACTION
				if (output != 2) {
					uart.disp("Game Selected: BUTTONS\n\r");
				}
				output = 2;
				led_p->write(0,0);
				led_p->write(0,1);
				led_p->write(1,output);
				led_p->write(0,3);
				led_p->write(0,4);
				game_selected = true;
			}
			else if ((s%4)>=2) {
				//switch 1, BUTTONS & SWITCHES
				if (output != 1) {
					uart.disp("Game Selected: BUTTONS & SWITCHES\n\r");
				}
				output = 1;
				led_p->write(0,0);
				led_p->write(1,output);
				led_p->write(0,2);
				led_p->write(0,3);
				led_p->write(0,4);
				led_p->write(0,5);
				game_selected = true;
			}
			else if ((s%2)==1) {
				//switch 0, MEMORY
				if(output != 0) {
					uart.disp("Game Selected: MEMORY\n\r");
				}
				output = 0;
				led_p->write(1,output);
				led_p->write(0,1);
				led_p->write(0,2);
				led_p->write(0,3);
				led_p->write(0,4);
				led_p->write(0,5);
				game_selected = true;
			}
		}
	}
	led_p->write(0);
	return output;
}















void wordle(GpoCore *led_p, SpiCore *spi_p, Ps2Core *ps2_p, GpiCore *btn_p) {
	// setup
	const uint8_t RD_CMD = 0x0b;
	const uint8_t PART_ID_REG = 0x02;

	const uint8_t DATA_REG = 0x08;

	float x, y, z;
	int id;

	spi_p->set_freq(400000);
	spi_p->set_mode(0, 0);

	// check part id
	spi_p->assert_ss(0);    // activate
	spi_p->transfer(RD_CMD);  // for read operation
	spi_p->transfer(PART_ID_REG);  // part id address
	id = (int) spi_p->transfer(0x00);
	spi_p->deassert_ss(0);

	// read 8-bit x/y/z g values once
	spi_p->assert_ss(0);    // activate
	spi_p->transfer(RD_CMD);  // for read operation
	spi_p->transfer(DATA_REG);  //
	x = spi_p->transfer(0x00);
	y = spi_p->transfer(0x00);
	z = spi_p->transfer(0x00);
	spi_p->deassert_ss(0);



	srand(round(x*y*z));

	// setup ps2 keyboard
	id = ps2_p->init();

	// game
	bool loop = true;
	bool led_loop = true;
	int b;
	int final_guesses = 0;
	int curr_char = 0;
	int correct = 0;
	int close = 0;
	int led_display = 0;
	int led_flash = 0;
	char input;
	char word[6];
	char guess[6];

	switch(rand()%4) {

	case 0:
		// Acute
		word[0] = 'a';
		word[1] = 'c';
		word[2] = 'u';
		word[3] = 't';
		word[4] = 'e';
		break;
	case 1:
		// Weave
		word[0] = 'w';
		word[1] = 'e';
		word[2] = 'a';
		word[3] = 'v';
		word[4] = 'e';
		break;
	case 2:
		// Paper
		word[0] = 'p';
		word[1] = 'a';
		word[2] = 'p';
		word[3] = 'e';
		word[4] = 'r';
		break;
	case 3:
		// Zebra
		word[0] = 'z';
		word[1] = 'e';
		word[2] = 'b';
		word[3] = 'r';
		word[4] = 'a';
		break;
	}
	uart.disp("Enter word. Space for backspace, centre button to enter.\n\r");
	for(int i = 0; i<6; i+=0) {
		// loop for 6 word guesses
		sleep_ms(50);
		led_p->write(led_display);
		if(led_loop) {
			led_display = led_display - led_flash;
			led_loop = !led_loop;
		} else {
			led_display = led_display + led_flash;
			led_loop = !led_loop;
		}

		if(ps2_p->get_kb_ch(&input)) {
			uart.disp(input);
			// get word
			if (input == ' ') {
				if(curr_char > 0) {
					curr_char -= 1;
				}
			} else {
				if(curr_char < 5) {
					guess[curr_char] = input;
					curr_char += 1;
				}
			}
		}
		b = btn_p->read();
		if ((curr_char == 5) && (b%32 == 16)){
			// check word
			for(int j=0; j<5; j++) {
				if(guess[j] == word[j]) {
					// letter is correct
					correct += pow(2,j);
				} else {
					// letter is in word
					if ((guess[j] == word [0]) ||
						(guess[j] == word [1]) ||
						(guess[j] == word [2]) ||
						(guess[j] == word [3]) ||
						(guess[j] == word [4])) {
						close += pow(2,j);
					}
				}
			}
			curr_char = 0;
			led_flash = close;
			led_display = correct + led_flash;
			correct = 0;
			close = 0;
			if (correct == 31) {
				// guess is correct
				loop = false;
				final_guesses = i;
				i = 6;
			} else {
				// guess is incorrect
				i += 1;
			}
		}
	}
}


















void buttons(SsegCore *sseg_p, SpiCore *spi_p, GpiCore *btn_p) {

	// setup
	const uint8_t RD_CMD = 0x0b;
	const uint8_t PART_ID_REG = 0x02;

    const uint8_t DATA_REG = 0x08;

   float x, y, z;
   int id;

   spi_p->set_freq(400000);
   spi_p->set_mode(0, 0);

   // check part id
   spi_p->assert_ss(0);    // activate
   spi_p->transfer(RD_CMD);  // for read operation
   spi_p->transfer(PART_ID_REG);  // part id address
   id = (int) spi_p->transfer(0x00);
   spi_p->deassert_ss(0);

   // read 8-bit x/y/z g values once
   spi_p->assert_ss(0);    // activate
   spi_p->transfer(RD_CMD);  // for read operation
   spi_p->transfer(DATA_REG);  //
   x = spi_p->transfer(0x00);
   y = spi_p->transfer(0x00);
   z = spi_p->transfer(0x00);
   spi_p->deassert_ss(0);

   srand(round(x*y*z));

   sleep_ms(500);

   // game
   int score = 100000;
   bool check = false;
   bool loop = true;
   bool loop2 = true;
   uint32_t b;
   int n_btn = 6;
   int p_btn = 6;
   int btn_pressed = 6;
   while(loop) {

	   for(int i=0; i<10; i++) {
		   while (n_btn == p_btn) {
			   n_btn = rand() % 5;
		   }
		   p_btn = n_btn;
		   if(n_btn == 4) {
			   sseg_p->write_1ptn(0b11000110,0);
		   } else if (n_btn == 3) {
			   sseg_p->write_1ptn(0b11000111,0);
		   } else if (n_btn == 2) {
			   sseg_p->write_1ptn(0b10100001,0);
		   } else if (n_btn == 1) {
			   sseg_p->write_1ptn(0b11001110,0);
		   } else if (n_btn == 0) {
			   sseg_p->write_1ptn(0b11000001,0);
		   }
		   sleep_ms(200);
		   while(n_btn != btn_pressed) {
			   b = btn_p->read();
			   if (b== 16) {
				   //btn_c pressed,
				   btn_pressed = 4;
				   check = true;
			   }
			   else if (b==8) {
				   //btn_l pressed
				   btn_pressed = 3;
				   check = true;
			   }
			   else if (b==4) {
				   //btn_d pressed
				   btn_pressed = 2;
				   check = true;
			   }
			   else if (b==2) {
				   //btn_r pressed
				   btn_pressed = 1;
				   check = true;
			   }
			   else if (b==1) {
				   //btn_u pressed
				   btn_pressed = 0;
				   check = true;
			   }
			   if(check && (n_btn != btn_pressed)) {
				   score -= 50;
				   check = false;
			   }
			   sleep_ms(1);
			   score -= 3;
		   }
	   }

	   if(score < 0) {
		   score = 0;
	   }

	   uart.disp("final score: ");
	   uart.disp(score);
	   uart.disp("/100000\n\r");
	   sleep_ms(3000);
	   uart.disp("Hit centre button to try again or top button to go to game select\n\r");
	   loop2 = true;
	   while (loop2) {
		   b = btn_p->read();
		   if (b== 16) {
			   //btn_c pressed,
			   loop2 = false;
			   score = 100000;
			   sleep_ms(1000);
		   }
		   else if (b==1) {
			   //btn_u pressed
			   loop2 = loop = false;
		   }
	   }
   }
}













void switches(GpoCore *led_p, SpiCore *spi_p, GpiCore *sw_p, GpiCore *btn_p) {
	// setup
	const uint8_t RD_CMD = 0x0b;
	const uint8_t PART_ID_REG = 0x02;

	const uint8_t DATA_REG = 0x08;

   float x, y, z;
   int id;

   spi_p->set_freq(400000);
   spi_p->set_mode(0, 0);

   // check part id
   spi_p->assert_ss(0);    // activate
   spi_p->transfer(RD_CMD);  // for read operation
   spi_p->transfer(PART_ID_REG);  // part id address
   id = (int) spi_p->transfer(0x00);
   spi_p->deassert_ss(0);

   // read 8-bit x/y/z g values once
   spi_p->assert_ss(0);    // activate
   spi_p->transfer(RD_CMD);  // for read operation
   spi_p->transfer(DATA_REG);  //
   x = spi_p->transfer(0x00);
   y = spi_p->transfer(0x00);
   z = spi_p->transfer(0x00);
   spi_p->deassert_ss(0);

   srand(round(x*y*z));

   sleep_ms(500);

   //game
   int led_on = 0;
   int wins = 0;
   int b;
   bool loop2 = true;
   bool loop = true;
   int test;
   int add_sub;
   int sw_input;
   while (loop) {
	   test = (rand()%16);
	   add_sub = pow(2,test);
	   if ((led_on % (int)(pow(2,test+1))) >= add_sub) {
		   // if true, then led is on, so subtract to turn led off
		   led_on -= add_sub;
	   } else {
		   // if false, then led is off, so subtract to turn led on
		   led_on += add_sub;
	   }
	   led_p->write(led_on);
	   sleep_ms(1000);

	   sw_input = sw_p->read();

	   if (sw_input == led_on) {
		   wins += 1;
	   } else {
		   uart.disp("You lost! Total: ");
		   uart.disp(wins);
		   uart.disp(".\n\r");
		   uart.disp("Press centre button to try again or top button to go to game select\n\r");
		   loop2 = true;
		   while (loop2) {
			   b = btn_p->read();
			   if (b== 16) {
				   //btn_c pressed,
				   loop2 = false;
				   wins = 0;
				   led_on = 0;
				   sleep_ms(1000);
			   }
			   else if (b==1) {
				   //btn_u pressed
				   loop2 = loop = false;
			   }
		   }
	   }
   }
}



void switch_button(GpoCore *led_p, SpiCore *spi_p, SsegCore *sseg_p, GpiCore *sw_p, GpiCore *btn_p) {
	// setup
	const uint8_t RD_CMD = 0x0b;
	const uint8_t PART_ID_REG = 0x02;

	const uint8_t DATA_REG = 0x08;

   float x, y, z;
   int id;

   spi_p->set_freq(400000);
   spi_p->set_mode(0, 0);

   // check part id
   spi_p->assert_ss(0);    // activate
   spi_p->transfer(RD_CMD);  // for read operation
   spi_p->transfer(PART_ID_REG);  // part id address
   id = (int) spi_p->transfer(0x00);
   spi_p->deassert_ss(0);

   // read 8-bit x/y/z g values once
   spi_p->assert_ss(0);    // activate
   spi_p->transfer(RD_CMD);  // for read operation
   spi_p->transfer(DATA_REG);  //
   x = spi_p->transfer(0x00);
   y = spi_p->transfer(0x00);
   z = spi_p->transfer(0x00);
   spi_p->deassert_ss(0);

   srand(round(x*y*z));

   sleep_ms(500);

   //game
	int led_on = 0;
	int wins = 0;
	bool loop2 = true;
	bool loop = true;
	int test;
	int add_sub;
	int sw_input;
	uint32_t b;
	int n_btn = 6;
	int p_btn = 6;
	int btn_pressed = 6;

   while (loop) {
	   switch(rand()%2){
	   case 0:
		   test = (rand()%16);
		   add_sub = pow(2,test);
		   if ((led_on % (int)(pow(2,test+1))) >= add_sub) {
			   // if true, then led is on, so subtract to turn led off
			   led_on -= add_sub;
		   } else {
			   // if false, then led is off, so subtract to turn led on
			   led_on += add_sub;
		   }
		   led_p->write(led_on);
		   sleep_ms(1000);

		   sw_input = sw_p->read();
		   break;
	   case 1:
		   while (n_btn == p_btn) {
			   n_btn = rand() % 5;
		   }
		   p_btn = n_btn;
		   if(n_btn == 4) {
			   sseg_p->write_1ptn(0b11000110,0);
		   } else if (n_btn == 3) {
			   sseg_p->write_1ptn(0b11000111,0);
		   } else if (n_btn == 2) {
			   sseg_p->write_1ptn(0b10100001,0);
		   } else if (n_btn == 1) {
			   sseg_p->write_1ptn(0b11001110,0);
		   } else if (n_btn == 0) {
			   sseg_p->write_1ptn(0b11000001,0);
		   }
		   sleep_ms(1000);
		   b = btn_p->read();
		   if (b==16) {
			   //btn_c pressed,
			   btn_pressed = 4;
		   }
		   else if (b==8) {
			   //btn_l pressed
			   btn_pressed = 3;
		   }
		   else if (b==4) {
			   //btn_d pressed
			   btn_pressed = 2;
		   }
		   else if (b==2) {
			   //btn_r pressed
			   btn_pressed = 1;
		   }
		   else if (b==1) {
			   //btn_u pressed
			   btn_pressed = 0;
		   }
		   break;
	   }
	   if((sw_input == led_on) && (btn_pressed == n_btn)) {
		   wins += 1;
	   } else {
		   uart.disp("You lost! Total: ");
		   uart.disp(wins);
		   uart.disp(".\n\r");
		   uart.disp("Press centre button to try again or top button to go to game select\n\r");
		   loop2 = true;
		   while (loop2) {
			   b = btn_p->read();
			   if (b== 16) {
				   //btn_c pressed,
				   loop2 = false;
				   wins = 0;
				   sw_input = 0;
				   btn_pressed = 6;
				   led_on = 0;
				   led_p->write(0);
				   sseg_p->write_1ptn(0b11111111,0);
				   n_btn = 6;
				   p_btn = 6;
				   sleep_ms(1000);
			   }
			   else if (b==1) {
				   //btn_u pressed
				   loop2 = loop = false;
			   }
		   }
	   }
   }
}




void memory (GpoCore *led_p, SpiCore *spi_p, GpiCore *sw_p, GpiCore *btn_p) {
	// setup
	const uint8_t RD_CMD = 0x0b;
	const uint8_t PART_ID_REG = 0x02;

	const uint8_t DATA_REG = 0x08;

   float x, y, z;
   int id;

   spi_p->set_freq(400000);
   spi_p->set_mode(0, 0);

   // check part id
   spi_p->assert_ss(0);    // activate
   spi_p->transfer(RD_CMD);  // for read operation
   spi_p->transfer(PART_ID_REG);  // part id address
   id = (int) spi_p->transfer(0x00);
   spi_p->deassert_ss(0);

   // read 8-bit x/y/z g values once
   spi_p->assert_ss(0);    // activate
   spi_p->transfer(RD_CMD);  // for read operation
   spi_p->transfer(DATA_REG);  //
   x = spi_p->transfer(0x00);
   y = spi_p->transfer(0x00);
   z = spi_p->transfer(0x00);
   spi_p->deassert_ss(0);

   srand(round(x*y*z));

   sleep_ms(2000);

   //game
   uint32_t led_on = 0;
   uint32_t sw_on = 0;
   uint32_t b;
   bool win = false;
   bool loop2 = true;
   bool loop = true;
   bool start_loop = true;
   bool game_loop = true;

   while(loop) {
	   start_loop = true;
	   led_on = rand() % 65535; // (2^16-1), randomly determines which leds will be on
	   led_p->write(led_on);
	   uart.disp("Turn OFF all switches! Press centre button to start.\n\r");
	   while(start_loop) {
		   b = btn_p->read();
		   if (b== 16) {
			   //btn_c pressed,
			   start_loop = false;
			   led_p->write(0);
			   sw_on = sw_p->read();
			   if(sw_on != 0) {
				   game_loop = false;
				   win = false;
			   } else {
				   game_loop = true;
			   }
			   sleep_ms(1500);
		   }
	   }

	   uart.disp("Press centre button to submit answer\n\r");
	   while(game_loop) {
		   sw_on = sw_p->read();
		   led_p->write(sw_on);
	   	   b = btn_p->read();
	   	   if (b== 16) {
	   		   //btn_c pressed,
	   		   led_p->write(led_on);
	   		   game_loop = false;
	   	   }
	   }
	   if (sw_on == led_on) {
		   win = true;
	   } else {
		   win = false;
	   }
	   if (win) {
		   uart.disp("Congrats, you win!\n\r");
	   } else {
		   uart.disp("L.\n\r");
	   }
	   sleep_ms(1500);
	   uart.disp("Press centre button to try again or top button to go to game select\n\r");
	   loop2 = true;
	   while (loop2) {
		   b = btn_p->read();
		   if (b== 16) {
			   //btn_c pressed,
			   loop2 = false;
			   led_p->write(0);
			   sleep_ms(1000);
		   }
		   else if (b==1) {
			   //btn_u pressed
			   loop2 = loop = false;
			   sleep_ms(1000);
		   }
	   }
   }
}
