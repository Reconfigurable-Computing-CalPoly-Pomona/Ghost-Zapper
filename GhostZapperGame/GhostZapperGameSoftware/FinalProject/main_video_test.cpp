/*****************************************************************//**
 * @file main_video_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "xadc_core.h"
#include "spi_core.h"
#include "i2c_core.h"
#include "ps2_core.h"
#include "vga_core.h"
#include "ctimer_core.h"
#include "sseg_core.h"

#define LEFT 0x00000008
#define RIGHT 0x00000002

static int Xmouse1 = 0, Ymouse1 = 0, Xmouse2 = 0, Ymouse2 = 0, GX1 = 0, GY1 = 0, GX2 = 625, GY2 = 0;


void init(Ps2Core *ps2_p){
	ps2_p -> init();
}

int SinglePlayerMouse(SpriteCore *mouse_p, Ps2Core *ps2_p){
	int lb = 0, rb, X, Y;

	if (ps2_p->get_mouse_activity(&lb, &rb, &X, &Y) == 1){

		ps2_p->get_mouse_activity(&lb, &rb, &X, &Y);
		Xmouse1 += X;
		Ymouse1 -= Y;
		if(Xmouse1 >= 625)
			Xmouse1 = 625;
		if(Xmouse1 <= 0)
			Xmouse1 = 0;
		if(Ymouse1 >= 460)
			Ymouse1 = 460;
		if(Ymouse1 <= 0)
			Ymouse1 = 0;
		mouse_p->move_xy(Xmouse1, Ymouse1);

	}

	   return lb;
}

void mouse_adc(SpriteCore *mouse_p, XadcCore *adc_p, int mouseSel) { //0 or 2
	int Xmouse, Ymouse;

	int Xadc = adc_p -> read_raw(mouseSel);
	int Yadc = adc_p -> read_raw(mouseSel+1);
	Xmouse = ((Xadc>>7) + (Xadc>>9));//&0xfff8; //
	Ymouse = ((Yadc>>8) + (Yadc>>9) + (Yadc>>10) + (Yadc>>11));//&0xfff8; //

	mouse_p->move_xy(Xmouse, Ymouse);

	if(mouseSel == 0){
		Xmouse1 = Xmouse;
		Ymouse1 = Ymouse;
	}

	if(mouseSel == 2){
		Xmouse2 = Xmouse;
		Ymouse2 = Ymouse;
	}
   }

void ps2_check(Ps2Core *ps2_p) {
   int id;
   int lbtn, rbtn, xmov, ymov;
   char ch;
   unsigned long last;

   uart.disp("\n\rPS2 device (1-keyboard / 2-mouse): ");
   id = ps2_p->init();
   uart.disp(id);
   uart.disp("\n\r");
   last = now_ms();
   do {
      if (id == 2) {  // mouse
         if (ps2_p->get_mouse_activity(&lbtn, &rbtn, &xmov, &ymov)) {
            uart.disp("[");
            uart.disp(lbtn);
            uart.disp(", ");
            uart.disp(rbtn);
            uart.disp(", ");
            uart.disp(xmov);
            uart.disp(", ");
            uart.disp(ymov);
            uart.disp("] \r\n");
            last = now_ms();

         }   // end get_mouse_activitiy()
      } else {
         if (ps2_p->get_kb_ch(&ch)) {
            uart.disp(ch);
            uart.disp(" ");
            last = now_ms();
         } // end get_kb_ch()
      }  // end id==2
   } while (now_ms() - last < 5000);
   uart.disp("\n\rExit PS2 test \n\r");

}

int check(int controller){ //checks if specified controller hit a ghost
	int Xmouse, Ymouse;

	switch(controller){
	case(0):Xmouse = Xmouse1;
			Ymouse = Ymouse1;
			break;
	case(2):Xmouse = Xmouse2;
			Ymouse = Ymouse2;
			break;
	default:Xmouse = 0;
			Ymouse = 0;
			break;
	}

	if(((((GX1 - Xmouse) >= 0) && ((GX1 - Xmouse) <= 10))|(((Xmouse - GX1) >= 0) && ((Xmouse - GX1) <= 15)))){ //checks ghost1's X axis
		if(((((GY1 - Ymouse) >= 0) && ((GY1 - Ymouse) <= 10))|(((Ymouse - GY1) >= 0) && ((Ymouse - GY1) <= 15)))){ //checks ghost1's Y axis
			return 5; //[ghost died] [ghost1 alive] [ghost0 dead] = 101
		}
		if(((((GY2 - Ymouse) >= 0) && ((GY2 - Ymouse) <= 10))|(((Ymouse - GY2) >= 0) && ((Ymouse - GY2) <= 10)))){ //checks ghost2's Y axis in case both ghosts were roughly in the same area
			return 6; //[ghost died] [ghost1 dead] [ghost0 alive] = 110
		}
	}

	if(((((GX2 - Xmouse) >= 0) && ((GX2 - Xmouse) <= 10))|(((Xmouse - GX2) >= 0) && ((Xmouse - GX2) <= 15)))){
		if(((((GY1 - Ymouse) >= 0) && ((GY1 - Ymouse) <= 10))|(((Ymouse - GY1) >= 0) && ((Ymouse - GY1) <= 15)))){
			return 5; //[ghost died] [ghost1 alive] [ghost0 dead] = 101
		}
		if(((((GY2 - Ymouse) >= 0) && ((GY2 - Ymouse) <= 10))|(((Ymouse - GY2) >= 0) && ((Ymouse - GY2) <= 15)))){
			return 6; //[ghost died] [ghost1 dead] [ghost0 alive] = 110
		}
	}

	else{ //no ghost dead
		return 0; //0x [no ghost died] [ghost1 alive] [ghost0 alive] = 0
	}

	//return 0;
}

char convert(int x){ //converts integer into character
	char num = 0;
	switch(x){
		   case(0):
				   num = '0';
		   	   	   break;
		   case(1):
				   num = '1';
		   		   break;
		   case(2):
		   		   num = '2';
		   		   break;
		   case(3):
		   		   num = '3';
		   		   break;
		   case(4):
		   		   num = '4';
		   	   	   break;
		   case(5):
		  		   num = '5';
 		   	   	   break;
		   case(6):
   				   num = '6';
		   	   	   break;
		   case(7):
		   		   num = '7';
		   	   	   break;
		   case(8):
				   num = '8';
		   	   	   break;
		   case(9):
		   		   num = '9';
		   		   break;
		   default: num = '0';
		   }
	return num;
}

void ghostGEN (ConstTimerCore *ctimer_p, SpriteCore *ghost1_p, SpriteCore *ghost2_p, int ghostSTAT){
	 //spawns new ghosts in random location
   static double a = 0, b = 0;
	int A = 0, B = 0;
	static int Adir = 1, Bdir = -1;

	uint32_t C = ctimer_p->read();

	a += 0.1;
	b += 0.1;

	if(a >= 10){
		A = 1;
		a = 0;
	}
	if(b >= 10){
		B = 1;
		b = 0;
	}

	if(GX1 >= 625){
		GX1 = 625;
		Adir = -1;
	}
	if(GX1 <= 0) {
		GX1 = 0;
		Adir = 1;
	}


	if(GX2 >= 625){
		GX2 = 625;
		Bdir = -1;
	}
	if(GX2 <= 0){
		GX2 = 0;
		Bdir = 1;
	}
	A *= Adir;
	B *= Bdir;

	switch(ghostSTAT){
		case(0): //both alive
				GX1+=A;
				GX2+=B;
				ghost1_p->move_xy(GX1, GY1);
				ghost2_p->move_xy(GX2, GY2);
				break;
		case(3): //both dead
				GX1 = 625*(C & 0x00000001);
				GY1 = C;//
				sleep_ms(5);
				C = ctimer_p->read();
				GX2 = 625*(C & 0x00000001);
				GY2 = C;
				ghost1_p->move_xy(GX1, GY1);
				ghost2_p->move_xy(GX2, GY2);
				break;
		case(6): //ghost2 dead
				GX1+=A;
				GX2 = 625*(C & 0x00000001);
				GY2 = C;//
				ghost1_p->move_xy(GX1, GY1);
				ghost2_p->move_xy(GX2, GY2);
				break;
		case(5): //ghost1 dead
				GX2+=B;
				GY1 = C;
				GX1 = 625*(C & 0x00000001);
				ghost1_p->move_xy(GX1, GY1);
				ghost2_p->move_xy(GX2, GY2);
				break;
		default: break;
	}

}

void ssegFlash(SsegCore *sseg_p){
	   sseg_p->write_1ptn(sseg_p->d2l(16), 7);
	   sseg_p->write_1ptn(sseg_p->d2l(12), 6);
	   sseg_p->write_1ptn(sseg_p->d2l(1), 5);
	   sseg_p->write_1ptn(sseg_p->d2l(25), 4);
	   sseg_p->write_1ptn(sseg_p->d2l(7), 3);
	   sseg_p->write_1ptn(sseg_p->d2l(1), 2);
	   sseg_p->write_1ptn(sseg_p->d2l(13), 1);
	   sseg_p->write_1ptn(sseg_p->d2l(5), 0);
}

void ssegOFF(SsegCore *sseg_p){
	for(int i = 0; i < 8; i++){
	   sseg_p->set_dp(0x00);
	   sseg_p->write_1ptn(sseg_p->h2s(0xff), i);
	}
}

void game(	GpiCore *sw_p, DebounceCore *db_p, SpriteCore *CrossHair1_p, SpriteCore *CrossHair2_p,
			SpriteCore *ghost1_p, SpriteCore *ghost2_p, Ps2Core *ps2_p, XadcCore *adc_p, ConstTimerCore *ctimer_p,
			OsdCore *osd_p, SsegCore *sseg_p) {

	int ghostSTAT = 3, score1 = 0, score2 = 0, btn, bntLB = 0; //holds life of two ghosts in binary (0 if alive, 1 if Dead)
	//uint64_t time = 0, timeINITIAL;
	char P1S10, P1S1, P2S10, P2S1;
	ghost1_p->wr_ctrl(0x0c);
	ghost2_p->wr_ctrl(0x14);
	ssegOFF(sseg_p);
	 osd_p->set_color(0xF00, 0x001);//RED
	 osd_p->wr_char(38, 20, 'P', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(16), 7);
	 sleep_ms(100);
	 osd_p->set_color(0xF80, 0x001);//ORANGE
	 osd_p->wr_char(39, 20, 'L', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(12), 6);
	 sleep_ms(100);
	 osd_p->set_color(0xFF0, 0x001);//YELLOW
	 osd_p->wr_char(40, 20, 'A', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(1), 5);
	 sleep_ms(100);
	 osd_p->set_color(0x0F0, 0x001);//GREEN
	 osd_p->wr_char(41, 20, 'Y', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(25), 4);
	 sleep_ms(100);
	 osd_p->set_color(0x0F8, 0x001);//TEAL
	 osd_p->wr_char(43, 20, 'G', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(7), 3);
	 sleep_ms(100);
	 osd_p->set_color(0x0FF, 0x001);//BRIGHTBLUE
	 osd_p->wr_char(44, 20, 'A', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(1), 2);
	 sleep_ms(100);
	 osd_p->set_color(0x00F, 0x001);//BLUE
	 osd_p->wr_char(45, 20, 'M', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(13), 1);
	 sleep_ms(100);
	 osd_p->set_color(0x80F, 0x001);//BRIGHTPURPLE
	 osd_p->wr_char(46, 20, 'E', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(5), 0);
	 sleep_ms(100);
	 osd_p->set_color(0xF0F, 0x001);//PURPLE
	 osd_p->wr_char(47, 20, '?', 0);
	 sleep_ms(1500);

	 btn = db_p->read();

	 while (btn == 0){
     btn = db_p->read();
	 osd_p->set_color(0xFFF, 0x001);//WHITE
	 osd_p->wr_char(37, 21, 'P', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(16), 7);
	 osd_p->wr_char(38, 21, 'R', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(12), 6);
	 osd_p->wr_char(39, 21, 'E', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(1), 5);
	 osd_p->wr_char(40, 21, 'S', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(25), 4);
	 osd_p->wr_char(41, 21, 'S', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(7), 3);
	 osd_p->wr_char(44, 21, 'S', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(1), 2);
	 osd_p->wr_char(45, 21, 'T', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(13), 1);
	 osd_p->wr_char(46, 21, 'A', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(5), 0);
	 osd_p->wr_char(47, 21, 'R', 0);
	 osd_p->wr_char(48, 21, 'T', 0);
	 btn = db_p->read();
	 sleep_ms(175);
	 btn = db_p->read();
	 sleep_ms(175);
	 ssegOFF(sseg_p);
	 btn = db_p->read();
	 sleep_ms(175);
	 btn = db_p->read();
	 sleep_ms(175);
	 btn = db_p->read();
	 }

	 osd_p->clr_screen();
	//welcome screen
	//wait for button
	//start!


	while((score1 <= 9)&(score2 <= 9)){// || (score2 <= 10)){

		ghostGEN(ctimer_p, ghost1_p, ghost2_p, ghostSTAT);
		ghostSTAT = 0;

		if( sw_p->read() == 0x0001){ //checks if singleplayer mouse
			bntLB = SinglePlayerMouse(CrossHair2_p, ps2_p);
			if(bntLB == 1){		//if player1 pressed button
							ghostSTAT = check(0);
							score1 += (ghostSTAT >> 2);
							sseg_p->write_1ptn(sseg_p->h2s(score1), 0);
			}
		}
		else{						//else two player controller input
			   mouse_adc(CrossHair1_p, adc_p, 0);
			   mouse_adc(CrossHair2_p, adc_p, 2);
			   btn = db_p->read();
			   btn = btn & 0x0000000F;
			if(btn == RIGHT){		//if player1 pressed button
				ghostSTAT = check(0); //player1
				score1 += ghostSTAT >> 2;
				sseg_p->write_1ptn(sseg_p->h2s(score1), 0);
			}
			if(btn == LEFT){		//if player2 pressed button
				ghostSTAT = check(2); //player2
				score2 += ghostSTAT >> 2;
				sseg_p->write_1ptn(sseg_p->h2s(score2), 4);
			}
		}


	}

	P1S10 = convert(score1/10);
	P1S1 = convert(score1-((score1/10)*10));
	P2S10 = convert(score2/10);
	P2S1 = convert(score2-((score2/10)*10));

	 osd_p->set_color(0xF00, 0x001);//RED
	 osd_p->wr_char(38, 20, 'G', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(7), 7);
	 sleep_ms(300);
	 osd_p->set_color(0xF80, 0x001);//ORANGE
	 osd_p->wr_char(39, 20, 'A', 0);
	   sseg_p->write_1ptn(sseg_p->d2l(1), 6);
	 sleep_ms(300);
	 osd_p->set_color(0xFF0, 0x001);//YELLOW
	 osd_p->wr_char(40, 20, 'M', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(13), 5);
	 sleep_ms(300);
	 osd_p->set_color(0x0F0, 0x001);//GREEN
	 osd_p->wr_char(41, 20, 'E', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(5), 4);
	 sleep_ms(300);
	 osd_p->set_color(0x0F8, 0x001);//TEAL
	 osd_p->wr_char(43, 20, 'O', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(15), 3);
	 sleep_ms(300);
	 osd_p->set_color(0x0FF, 0x001);//BRIGHTBLUE
	 osd_p->wr_char(44, 20, 'V', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(22), 2);
	 sleep_ms(300);
	 osd_p->set_color(0x00F, 0x001);//BLUE
	 osd_p->wr_char(45, 20, 'E', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(5), 1);
	 sleep_ms(300);
	 osd_p->set_color(0x80F, 0x001);//BRIGHTPURPLE
	 osd_p->wr_char(46, 20, 'R', 0);
	 sseg_p->write_1ptn(sseg_p->d2l(18), 0);
	 sleep_ms(300);
	 osd_p->set_color(0xF0F, 0x001);//PURPLE
	 osd_p->wr_char(47, 20, '!', 0);
	 sleep_ms(300);

	 osd_p->set_color(0xFFF, 0x001);//WHITE
	 osd_p->wr_char(39, 21, 'S', 0);
	 osd_p->wr_char(40, 21, 'C', 0);
	 osd_p->wr_char(41, 21, 'O', 0);
	 osd_p->wr_char(42, 21, 'R', 0);
	 osd_p->wr_char(43, 21, 'E', 0);
	 osd_p->wr_char(44, 21, '1', 0);
	 osd_p->wr_char(46, 21, P1S10, 1);
	 osd_p->wr_char(47, 21, P1S1, 1);

	 osd_p->wr_char(39, 22, 'S', 0);
	 osd_p->wr_char(40, 22, 'C', 0);
	 osd_p->wr_char(41, 22, 'O', 0);
	 osd_p->wr_char(42, 22, 'R', 0);
	 osd_p->wr_char(43, 22, 'E', 0);
	 osd_p->wr_char(44, 22, '2', 0);
	 osd_p->wr_char(46, 22, P2S10, 1);
	 osd_p->wr_char(47, 22, P2S1, 1);

	//display score
	sleep_ms(5000);
	sleep_ms(5000);
	ssegOFF(sseg_p);
	score1 = score2 = 0;
	osd_p->clr_screen();
}

// external core instantiation
ConstTimerCore ctimer(get_slot_addr(BRIDGE_BASE, S4_USER));
Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
FrameCore frame(FRAME_BASE);

SpriteCore ghost1(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
SpriteCore ghost2(get_sprite_addr(BRIDGE_BASE, V5_USER5), 1024);
SpriteCore CrossHair1(get_sprite_addr(BRIDGE_BASE, V1_MOUSE), 1024);
SpriteCore CrossHair2(get_sprite_addr(BRIDGE_BASE, V4_USER4), 1024);

OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));
DebounceCore btn(get_slot_addr(BRIDGE_BASE, S7_BTN));
SpiCore spi(get_slot_addr(BRIDGE_BASE, S9_SPI));
I2cCore adt7420(get_slot_addr(BRIDGE_BASE, S10_I2C));

DebounceCore db(get_slot_addr(BRIDGE_BASE, S7_BTN));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));

int main() {
	init(&ps2);
	while (1) {
		game(&sw, &db, &CrossHair1, &CrossHair2, &ghost1, &ghost2, &ps2, &adc, &ctimer, &osd, &sseg);
   } // while
} //main
