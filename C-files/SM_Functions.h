//--------End Shared Variables------------------------------------------------
#include "C:\Users\John\Documents\120B\Project\keypad.h"
#include "C:\Users\John\Documents\120B\Project\bit.h"
#include <stdio.h>
#include <math.h>
//Created by John Andrew Dixon. Used UCR EE120B's libraries, though the code below is fully mine.
unsigned char password[] = "0000";
unsigned char time[] =  "00";
unsigned short tp = 0;
unsigned short pp = 0;
unsigned char out = '\0';
enum startup {start, wel, wwait, pr1, pr1w, pr2, pr2w, pr3, pr3w, p_in, t_in, men_u} states_of;
unsigned short cnt = 0;
unsigned short csr = 11;
unsigned short sreq = 0;
unsigned short sack = 0;
unsigned short switch_ = 0;
const unsigned char Ihour[] = "00";
unsigned char Imin[] = "00";
unsigned char Isec[] = "00";
unsigned char hour[] = "00";
unsigned char min[] = "00";
unsigned char sec[] = "00";
short bool_ = 1;
long time_z = 0;
long time_dz = 0;
long time_i = 0;
long time_di = 0;
long time_w = 0;
/*
Start up tick function which allows the user to enter a Passkey and warning time.
Upon being done, it send a request to the next function to display menu;
*/
void tick_on_off() {
	switch(states_of) {
		case -1:
			states_of = start;
			break;
		case start:
			states_of = wel;
			break;
		case wel:
			states_of = wwait;
			break;
		case wwait:
			if(cnt < 100)
				states_of = wwait;
			else {
				LCD_ClearScreen();
				cnt = 0;
				states_of = pr1;
			}
			break;
		case pr1:
			states_of = pr1w;
			break;
		case pr1w:
			if(cnt < 75)
				states_of = pr1w;
			else {
				LCD_ClearScreen();
				cnt = 0;
				states_of = pr2;
			}
			break;
		case pr2:
			states_of = pr2w;
			break;
		case pr2w:
			if(cnt < 75)
				states_of = pr2w;
			else {
				LCD_ClearScreen();
				cnt = 0;
				states_of = pr3;
			}
			break;
		case pr3:
			states_of = pr3w;
			break;
		case pr3w:
			states_of = p_in;
			break;
		case p_in:
			if(~PINB & 0x08) {
				states_of = p_in;
				time_w = 0;
				password[0] = 0;
				password[1] = 0;
				password[2] = 0;
				password[3] = 0;
				LCD_Cursor(11);
				LCD_WriteData(' ');
				LCD_WriteData(' ');
				LCD_WriteData(' ');
				LCD_WriteData(' ');
				csr = 11;
				cnt = 0;
				switch_ = 0;
				pp = 0; 
				LCD_Cursor(11);
			}
			else if(csr < 15) {
				states_of = p_in;
			}
			else if(~PINB & 0x02) {
				csr = 29;
				states_of = t_in;
				switch_ = 0;
				LCD_Cursor(29);
			}
			else {	
				states_of = p_in;
			}
			break;
		case t_in:
			if(~PINB & 0x08) {
				states_of = t_in;
				time_w = 0;
				time[1] = 0;
				time[0] = 0;
				csr = 29;
				LCD_Cursor(29);
				LCD_WriteData(' ');
				LCD_WriteData(' ');
				cnt = 0;
				switch_ = 0;
				tp = 0; 
				LCD_Cursor(29);
			}
			else if(csr < 31) {
				states_of = t_in;
			}
			else if(~PINB & 0x02){	
				csr = 0;
				LCD_ClearScreen();
				states_of = men_u;
				switch_ = 0;
				unsigned short a = time[1] - '0';
				unsigned short b = time[0] - '0';
				time_w = (b * 10) + a;
			}
			else {
				states_of = t_in;
			}
			break;
		case men_u:
			if(sack == 0) {
				states_of = men_u;
			}
			break;
	}
	switch(states_of) {
		case start:
			break;
		case wel:
			LCD_DisplayString(1, "   Welcome to       Secure-X");
			break;
		case wwait:
			cnt++;
			break;
		case pr1:
			LCD_DisplayString(1, " Specify your 4  digit Passkey");
			break;
		case pr1w:
			cnt++;
			break;
		case pr2:
			LCD_DisplayString(1, "  and optional   alarm warning.");
			break;
		case pr2w:
			cnt++;
			break;
		case pr3:
			LCD_DisplayString(1, "Passkey:        Warning(s):");
			break;
		case pr3w:
			cnt = 0;
			break;
		case p_in:
			out = GetKeypadKey();
			if(out != '\0' && cnt > 15 && !switch_) {
					LCD_Cursor(csr);
					password[pp] = out;
					LCD_WriteData('*');
					if(csr >= 14)
						switch_ = 1;
					csr++;
					pp++;
					cnt = 0;
			}
			cnt++;
			break;
		case t_in:
			out = GetKeypadKey();
			if(out != '\0' && cnt > 15 && !switch_ && (out >= '0' && out <= '9')) {
					LCD_Cursor(csr);
					time[tp] = out;
					LCD_WriteData(out);
					if(csr >= 30) {
						switch_ = 1;
					}
					csr++;
					tp++;
					cnt = 0;
			}
			cnt++;
			break;
		case men_u:
			sreq = 1;
			break;
	}
};
unsigned short PCreq = 0;
unsigned short Areq = 0;
unsigned short Wreq = 0;
unsigned short ITreq = 0;
unsigned short Greq = 0;
unsigned short Gack = 0;
/*
Tick function which displays the menu upon receiving request from the start up function. 
There are four functions this menu tick function interacts with, sending respecting requests to each
and then returning to menu upon getting an acknowledgment. The other four functions are change password, change
warning time, view time with and without an intruder, and arm system.

*/
enum A_menu {wait, p_menu, menu, raise} menu_s;
void tick_menu() {
	switch(menu_s) {
		case -1: 
			menu_s = wait;
			break;
		case wait:
			if(!sreq) 
				menu_s = wait;
			else
				menu_s = p_menu;
			break;
		case p_menu:
			menu_s = menu;
			break;
		case menu:
			if((~PINB & 0x02) && !Greq) {
				menu_s = raise;
				LCD_ClearScreen();
			}
			else if(!Greq){
				LCD_Cursor(csr);
				menu_s = menu;
			}
			else 
				menu_s = raise;
			break;
		case raise:
			if(Gack) {
				menu_s = p_menu;
			}
			else {
				menu_s = raise;
			}
			break;
	}
	switch(menu_s) {
		case wait:
			break;
		case p_menu:
			csr = 1;
			Gack = 0;
			LCD_DisplayString(1, " Passkey| ArmSys Warning| I-Time");
			LCD_Cursor(1);
			LCD_WriteData(0);
			LCD_Cursor(10);
			LCD_WriteData(1);
			LCD_Cursor(17);
			LCD_WriteData(2);
			LCD_Cursor(26);
			LCD_WriteData(3);
			break;
		case menu:
			if(~PINB & 0x01) {
				if(csr == 1) 
					csr = 10;
				else if(csr == 10)
					csr = 17;
				else if(csr == 17)
					csr = 26;
				else if(csr == 26)
					csr = 1;
			}
			else if(~PINB & 0x04) {
				if(csr == 1) 
					csr = 26;
				else if(csr == 10)
					csr = 1;
				else if(csr == 17)
					csr = 10;
				else if(csr == 26)
					csr = 17;
			}
			break;
		case raise:
			if(csr == 1 && !Gack) {
				PCreq = 1;
				Greq = 1;
			}
			else if(csr == 10 && !Gack) {
				Areq = 1;
				Greq = 1;
			}
			else if(csr == 17 && !Gack) {
				Wreq = 1;
				Greq = 1;
			}
			else if(csr == 26 && !Gack) {
				ITreq = 1;
				Greq = 1;
			}
			break;
	}
};
unsigned short same = 1;
unsigned short Hcsr = 23;
unsigned short hpp = 0;
unsigned short wait_ = 0;
enum PC_change {Pwait, P1, Penter, Pcheck, Pnew, PnewA, Pdone} PC_s;
/*
Tick function which is used to change the password. Upon gaining a request from the menu,the function allows the user
to input the old password and change to a new password. After being done it lowers the request and sends out a global
acknowledged signal signifiying the process is over.
*/
void  tick_PC_change() {
	switch(PC_s) {
		case -1:
			PC_s = Pwait;
			Hcsr = 23;
			switch_ = 0;
			pp = 0;
			break;
		case Pwait:
			if(PCreq && !Gack) {
				PC_s = P1;
			}
			else {
				PC_s = Pwait;
			}
			break;
		case P1:
			PC_s = Penter;
			break;
		case Penter:
			if(~PINB & 0x08) {
				PC_s = Pdone;
				Hcsr = 23;
				hpp = 0;
			}
			else if(Hcsr < 26) {
				PC_s = Penter;
			}
			else if(~PINB & 0x02){	
				Hcsr = 0;
				LCD_ClearScreen();
				PC_s = Pcheck;
				switch_ = 0;
			}
			else {
				PC_s = Penter;
			}
			break;
		case Pcheck:
			if(!same) {
				PC_s = P1;
				same = 1;
				LCD_ClearScreen();
			}
			else {
				PC_s  = Pnew;
			}
			break;
		case Pnew:
				PC_s = PnewA;
			break;
		case PnewA:
			if(~PINB & 0x08) {
				PC_s = PnewA;
				Hcsr = 12;
				hpp = 0;
				switch_ = 0;
				LCD_Cursor(12);
				LCD_WriteData(' ');
				LCD_WriteData(' ');
				LCD_WriteData(' ');
				LCD_WriteData(' ');
				password[0] = '0';
				password[1] = '0';
				password[2] = '0';
				password[3] = '0';
				LCD_Cursor(12);
			}
			else if(Hcsr < 15) {
					PC_s = PnewA;
			}
			else if(~PINB & 0x02) {
					LCD_ClearScreen();
					PC_s = Pdone;
			}
			else {
					PC_s = PnewA;
			}
			break;
		case Pdone:
			PC_s = Pwait;
			break;
	}
	switch(PC_s) {
		case Pwait:
			break;
		case P1: 
			LCD_DisplayString(1, " Enter Passkey: ");
			break;
		case Penter:
			out = GetKeypadKey();
			LCD_Cursor(Hcsr);
			if(out != '\0' && !switch_) {
					if(out != password[hpp])
						same = 0;
					LCD_WriteData('*');
					if(Hcsr >= 26)
						switch_ = 1;
					Hcsr++;
					hpp++;
			}
			break;
		case Pcheck:
			if(!same) {
					LCD_DisplayString(1, "Incorrect");
					Hcsr = 23;
					switch_ = 0;
					hpp = 0;
			}
			break;
		case Pnew:
			LCD_DisplayString(1, "Enter new: ");
			Hcsr = 12;
			switch_ = 0;
			hpp = 0;
			break;
		case PnewA:
			out = GetKeypadKey();
			LCD_Cursor(Hcsr);
			if(out != '\0' && !switch_) {
					password[hpp] = out;
					LCD_WriteData('*');
					if(Hcsr >= 15)
						switch_ = 1;
					Hcsr++;
					hpp++;
			}
			break;
		case Pdone:
			wait_ = 0;
			Hcsr = 23;
			same = 1;
			hpp = 0;
			switch_ = 0;
			Greq = 0;
			PCreq = 0;
			Gack = 1;
			
			break;
	}
};
unsigned short tpp = 0;
/*
Tick function which is used to change the user warning time. Upon gaining a request from the menu,the function allows the user
to input the old password and change the current warning time to a new warning time. After being done it lowers the request and sends out a global
acknowledged signal signifiying the process is over.
*/
enum W_change {Wwait, W1, Wenter, Wcheck, WnewW, WnewC, WnewA, Wdone} Warn_s;
void  tick_Warn() {
	switch(Warn_s) {
		case -1:
			Warn_s = Wwait;
			switch_ = 0;
			Hcsr = 23;
			pp = 0;
			break;
		case Wwait:
			if(Wreq && !Gack) {
				Warn_s = W1;
			}
			else {
				Warn_s = Wwait;
			}
			break;
		case W1:
			Warn_s = Wenter;
			break;
		case Wenter:
			if(~PINB & 0x08) {
				Warn_s = Wdone;
				Hcsr = 23;
				hpp = 0;
			}
			else if(Hcsr < 26) {
				Warn_s = Wenter;
			}
			else if(~PINB & 0x02){	
				Hcsr = 0;
				LCD_ClearScreen();
				Warn_s = Wcheck;
				switch_ = 0;
			}
			else {
				Warn_s = Penter;
			}
			break;
		case Wcheck:
			if(!same) {
				Warn_s = W1;
				same = 1;
				LCD_ClearScreen();
			}
			else {
				Warn_s  = WnewW;
				LCD_DisplayString(1, "Current warning time: " );
				LCD_Cursor(23);
				LCD_WriteData(time[tpp]);
				LCD_Cursor(24);
				LCD_WriteData(time[tpp + 1]);
				LCD_Cursor(25);
				LCD_WriteData('s');
				Hcsr = 12;
				switch_ = 0;
				hpp = 0;
			}
			break;
		case WnewW:
			if(wait_ <= 10)
				Warn_s = WnewW;
			else {
				Warn_s = WnewC;
				LCD_DisplayString(1, "Reset? (1/y:0/n)");
			}
			break;
		case WnewC:
			if(out == '1') {
				LCD_ClearScreen();
				LCD_DisplayString(1, "Enter new time: ");
				Warn_s = WnewA;
			}
			else if(out == '0')
				Warn_s = Wdone;
			else
				Warn_s = WnewC;
			break;
		case WnewA:
			if(~PINB & 0x08) {
				Warn_s = WnewA;
				LCD_Cursor(24);
				LCD_WriteData(' ');
				LCD_WriteData(' ');
				Hcsr = 24;
				tpp = 0;
				switch_ = 0;
				LCD_Cursor(24);
				time[0] = '0';
				time[1] = '0';
			}
			else if(Hcsr < 26) {
					Warn_s = WnewA;
			}
			else if(~PINB & 0x02) {
				LCD_ClearScreen();
				Warn_s = Wdone;
				unsigned short a = time[1] - '0';
				unsigned short b = time[0] - '0';
				time_w = (b * 10) + a;
			}
			else {
					Warn_s = WnewA;
			}
			break;
		case Wdone:
			Warn_s = Wwait;
			break;
	}
	switch(Warn_s){
		case Wwait: 
			break;
		case W1:
			LCD_DisplayString(1, " Enter Passkey: ");
			break;
		case Wenter:
			out = GetKeypadKey();
			LCD_Cursor(Hcsr);
			if(out != '\0' && !switch_) {
					if(out != password[hpp])
						same = 0;
					LCD_WriteData('*');
					if(Hcsr >= 26)
						switch_ = 1;
					Hcsr++;
					hpp++;
			}
			break;
		case Wcheck:
			if(!same) {
					LCD_DisplayString(1, "Incorrect");
					Hcsr = 23;
					switch_ = 0;
					hpp = 0;
			}
			break;
		case WnewW:
			Hcsr = 24;
			wait_++;
			break;
		case WnewC:
			out = GetKeypadKey();
			break;
		case WnewA:
			out = GetKeypadKey();
			LCD_Cursor(Hcsr);
			if(out != '\0' && !switch_ && (out >= '0' &&  out <= '9')) {
					time[tpp] = out;
					LCD_WriteData(time[tpp]);
					if(Hcsr >= 25)
						switch_ = 1;
					Hcsr++;
					tpp++;
			}
			break;
		case Wdone:
			Hcsr = 23;
			same = 1;
			hpp = 0;
			tpp = 0;
			switch_ = 0;
			Greq = 0;
			Wreq = 0;
			Gack = 1;
			wait_ = 0;
			break;
	}
};
enum ITime {Iwait, I1, Ienter, Icheck, IInc, IInc2, IInc3, InewW, InewC, InewA, Idone} ITime_s;
/*
Tick function which is used to view the times with and without an intruder. Upon gaining a request from the menu,the function allows the user
to input the old password and view such times with the option of resetting them. After being done it lowers the request and sends out a global
acknowledged signal signifiying the process is over.
*/
void  tick_ITime() {
	switch(ITime_s) {
		case -1:
			ITime_s = Iwait;
			switch_ = 0;
			Hcsr = 23;
			pp = 0;
			break;
			break;
		case Iwait:
			if(ITreq && !Gack) {
				ITime_s = I1;
			}
			else {
				ITime_s = Iwait;
			}
			break;
		case I1:
			ITime_s = Ienter;
			break;
		case Ienter:
			if(~PINB & 0x08) {
				ITime_s = Idone;
				Hcsr = 23;
				hpp = 0;
			}
			else if(Hcsr < 26) {
				ITime_s = Ienter;
			}
			else if(~PINB & 0x02){	
				Hcsr = 0;
				LCD_ClearScreen();
				ITime_s = Icheck;
				switch_ = 0;
			}
			else {
				ITime_s = Ienter;
			}
			break;
		case Icheck:
			if(!same) {
				ITime_s = I1;
				same = 1;
				LCD_ClearScreen();
			}
			else {
				ITime_s  = IInc;
				LCD_DisplayString(1, "Time without an intruder is:" );
				Hcsr = 12;
				switch_ = 0;
				hpp = 0;
				wait_ = 0;
			}
			break;
		case IInc:
			if(wait_ <= 10) {
				ITime_s = IInc;
			}
			else {
				ITime_s = IInc2;
				LCD_DisplayString(1, Sec2Read(time_z, bool_));
				Hcsr = 12;
				switch_ = 0;
				hpp = 0;
				wait_ = 0;
			}
			break;
		case IInc2:
			if(wait_ <= 10)
				ITime_s = IInc2;
			else {
				ITime_s = IInc3;
				LCD_DisplayString(1, "  Time with an  intruder is:");
				Hcsr = 12;
				switch_ = 0;
				hpp = 0;
				wait_ = 0;
			}
			break;
		case IInc3:
			if(wait_ <= 10)
				ITime_s = IInc3;
			else {
				ITime_s = InewW;
				LCD_DisplayString(1, Sec2Read(time_i, bool_));
				Hcsr = 12;
				switch_ = 0;
				hpp = 0;
				wait_ = 0;
			}
			break;
		case InewW:
			if(wait_ <= 10) {
				ITime_s = InewW;
			}
			else {
				ITime_s = InewC;
				LCD_ClearScreen();
				LCD_DisplayString(1, "Reset? (1/y:0/n)");
			}
			break;
		case InewC:
			if(out == '1')
				ITime_s = InewA;
			else if(out == '0')
				ITime_s = Idone;
			else 
				ITime_s = InewC;
			break;
		case InewA:
			ITime_s = Idone;
			break;
		case Idone:
			ITime_s = Iwait;
			break;
	}
	switch(ITime_s) {
		case Iwait:
			break;
		case I1:
			LCD_DisplayString(1, " Enter Passkey: ");
			break;
		case Ienter:
			out = GetKeypadKey();
			LCD_Cursor(Hcsr);
			if(out != '\0' && !switch_) {
					if(out != password[hpp])
						same = 0;
					LCD_WriteData('*');
					if(Hcsr >= 26)
						switch_ = 1;
					Hcsr++;
					hpp++;
			}
			break;
		case Icheck:
			if(!same) {
					LCD_DisplayString(1, "Incorrect");
					Hcsr = 23;
					switch_ = 0;
					hpp = 0;
			}
			break;
		case IInc:
			wait_++;
			break;
		case IInc2:
			wait_++;
			break;
		case IInc3:
			wait_++;
			break;
		case InewW:
			wait_++;
			break;
		case InewC:
			out = GetKeypadKey();
			break;
		case InewA:
			time_z = 0;
			time_z = 0;
			break;
		case Idone:
			Hcsr = 23;
			same = 1;
			hpp = 0;
			tpp = 0;
			switch_ = 0;
			Greq = 0;
			ITreq = 0;
			Gack = 1;
			wait_ = 0;
			break;
	}
};
unsigned short j = 0;
unsigned short CSR = 4;
/*
Tick function which is used to arm the system itself. Upon gaining a request from the menu,the function allows the user
to input the old password and arm the system. There is a 10 second period before being actually armed, where when it is armed it will light an LED.
When armed it will count up for the time without an intruder until the PIR or sound sensor grasps constant input.
After the warning time has passed 
*/
unsigned short Creq = 0;
unsigned short Sreq = 0;
unsigned short T_ack = 0;
unsigned short detectS = 0x00;
unsigned short detectM = 0x00;
unsigned short w_time = 0;
unsigned short turn_off = 0;
unsigned short stop = 0;
unsigned short ASCnt = 1;
enum Arm {Await, AW, Aenter, Acheck, AInc, AnewCT, ADown, AStart, ADone} Arm_s;
void  tick_Arm() {
	switch(Arm_s) {
		case -1:
			Arm_s = Await;
			break;
		case Await:
			if(Areq & !Gack) {
				Arm_s = AW;
				LCD_DisplayString(1, "You will now arm   the system");
				cnt = 0;
			}
			else {
				Arm_s = Await;
			}
			break;
		case AW:
			if(wait_ <= 100)
				Arm_s = AW;
			else {
				Arm_s = Aenter;
				LCD_DisplayString(1, " Enter Passkey: ");
			}
			break;
		case Aenter:
			if(~PINB & 0x08) {
				Arm_s = ADone;
				Hcsr = 23;
				hpp = 0;
			}
			else if(Hcsr < 26) {
				Arm_s = Aenter;
			}
			else if(~PINB & 0x02){	
				Hcsr = 0;
				LCD_ClearScreen();
				Arm_s = Acheck;
				switch_ = 0;
			}
			else {
				Arm_s = Aenter;
			}
			break;
		case Acheck:
			if(!same) {
				Arm_s = AW;
				same = 1;
				wait_ = 0;
			}
			else {
				Arm_s  = AInc;
				LCD_DisplayString(1, "  System  will    activate in");
				wait_ = 0;
			}
			break;
		case AInc:
			if(wait_ <= 100)
				Arm_s = AInc;
			else {
				Arm_s = AnewCT;
				LCD_DisplayString(1, "5");
				wait_ = 0;
				LCD_WriteData(' ');
			}
			break;
		case AnewCT:
			if(wait_ <= 15)
				Arm_s = AnewCT;
			else {
				Arm_s = ADown;
				wait_ = 10;
				cnt = 5;
			}
			break;
		case ADown:
			if(cnt != 0)
				Arm_s = ADown;
			else {
				Arm_s = AStart;
				LCD_ClearScreen();
			}
			break;
		case AStart:
			if(!T_ack)
				Arm_s = AStart;
			else 
				Arm_s = ADone;
			break;
		case ADone:
			Arm_s = Await;
			break;
	}
	switch(Arm_s) {
		case Await:
			break;
		case AW:
			wait_++;
			break;
		case Aenter:
			out = GetKeypadKey();
			LCD_Cursor(Hcsr);
			cnt++;
			if(out != '\0' && !switch_ && cnt >= 15) {
					if(out != password[hpp])
						same = 0;
					LCD_WriteData('*');
					if(Hcsr >= 26)
						switch_ = 1;
					Hcsr++;
					hpp++;
					cnt = 0;
			}
			break;
		case Acheck:
			if(!same) {
					LCD_DisplayString(1, "Incorrect");
					Hcsr = 23;
					switch_ = 0;
					hpp = 0;
			}
			break;
		case AInc:
			wait_++;
			break;
		case AnewCT:
			wait_++;
			break;
		case ADown:
			wait_++; 
			LCD_Cursor(CSR);
			if(wait_ >= 52) {
				cnt--;
				LCD_WriteData(cnt + '0');
				wait_ = 0;
				LCD_WriteData(' ');
				CSR+=2;
			}
			break;
		case AStart:
			detectS = GetBit(~PINB, 4);
			if(!detectM)
				detectM = GetBit(~PINA, 2);
			Creq = 1;
			break;
		case ADone:
			CSR = 4;
			Hcsr = 23;
			same = 1;
			ASCnt = 1;
			hpp = 0;
			tpp = 0;
			switch_ = 0;
			Greq = 0;
			Areq = 0;
			Gack = 1;
			wait_ = 0;
			Creq = 0;
			Sreq = 0;
			T_ack = 0;
			turn_off = 0;
			detectS = 0x00;
			detectM = 0x00;
			w_time = 0;
			cnt = 0;
			break;
	}
};
unsigned short Dis_CNT = 0;
unsigned short Cis_CNT = 0;
enum count {Cwait, Cstart, CInt, CSound, CStop, CDone} CNT_s;
void tick_CNT() {
	switch(CNT_s) {
		case -1:
			CNT_s = Cwait;
			break;
		case Cwait:
			if(!Creq) {
				CNT_s = Cwait;
			}
			else {
				CNT_s = Cstart;
				cnt = 0;
				hpp = 0;
				same = 1;
				LCD_DisplayString(1, Sec2Read(time_z, bool_));
			}
			break;
		case Cstart:
			if(stop >= 4) {
				if(same) {
					CNT_s = CStop;
					LCD_DisplayString(1, "Disarmed ");
					LCD_WriteData(0);
					cnt = 0;
				}
				else {
					hpp = 0;
					stop = 0;
					same = 1;
					CNT_s = Cstart;
				}
			}
			else if(!detectS && !detectM) {
				CNT_s = Cstart;
				bool_ = 1;
			}
			else if(detectS || detectM) {
				LCD_DisplayString(1, "Possible Intruder");
				bool_= 0;
				CNT_s = CInt;
				cnt = 0;
			}
			break;
		case CInt:
			if(stop >= 4) {
				if(same) {
					CNT_s = CStop;
					LCD_DisplayString(1, "Disarmed! ");
					LCD_WriteData(0);
					cnt = 0;
				}
				else {
					hpp = 0;
					stop = 0;
					same = 1;
					CNT_s = CInt;
				}
			}
			else if(!detectM && !detectS) {
				CNT_s = Cstart;
				w_time = 0;
			}
			else if(w_time != time_w)
				CNT_s = CInt;
			else if(w_time == time_w) {
				CNT_s = CSound;
				Sreq = 1;
				bool_ = 1;
				Dis_CNT = 0;
			}
			break;
		case CSound:
			if(stop >= 4) {
				if(same) {
					CNT_s = CStop;
					LCD_DisplayString(1, "Disarmed ");
					LCD_WriteData(0);
					cnt = 0;
					turn_off = 1;
				}
				else {
					hpp = 0;
					stop = 0;
					same = 1;
					CNT_s = CSound;
				}
			}
			else 
				CNT_s = CSound;
			break;
		case CStop:
			if(cnt <= 100)
				CNT_s = CStop;
			else 
				CNT_s = CDone;
			break;
		case CDone:
			CNT_s = Cwait;
			break;
	}
	switch(CNT_s) {
		case Cwait:
			break;
		case Cstart:
			Dis_CNT++;
			if(Dis_CNT >= 50) {
				time_dz++;
				LCD_DisplayString(1, Sec2Read(time_dz, bool_));
				LCD_Cursor(14);
				if(time_dz % 2)
					LCD_WriteData('-');
				else
					LCD_WriteData(1);
				Dis_CNT = 0;
			}
			out = GetKeypadKey();
			cnt++;
			if(out != '\0' && cnt >= 15 && stop < 4) {
				stop++;
				if(out != password[hpp])
					same = 0;
				hpp++;
				cnt = 0;
			}
			break;
		case CInt:
			Cis_CNT++;
			if(Cis_CNT >= 50) {
				w_time++;
				LCD_DisplayString(1, Sec2Read(w_time, bool_));
				Cis_CNT = 0;
				LCD_Cursor(15);
				if(w_time % 2 == 1) {
					LCD_WriteData(' ');
					LCD_WriteData(2);
				}
				else {
					LCD_WriteData(6);
					LCD_WriteData(7);
				}
			}
			out = GetKeypadKey();
			cnt++;
			if(out != '\0' && cnt >= 15 && stop < 4) {
				stop++;
				if(out != password[hpp])
					same = 0;
				hpp++;
				cnt = 0;
			}
			break;
		case CSound:
			Dis_CNT++;
			if(Dis_CNT >= 50) {
				time_di++;
				LCD_DisplayString(1, Sec2Read(time_di, bool_));
				LCD_Cursor(14);
				if(time_di % 2) {
					LCD_WriteData(4);
					LCD_Cursor(15);
					LCD_WriteData(5);
					LCD_Cursor(17);
					LCD_WriteData(' ');
					LCD_Cursor(18);
					LCD_WriteData(' ');
					LCD_Cursor(31);
					LCD_WriteData(6);
					LCD_Cursor(32);
					LCD_WriteData(7);
				}
				else {
					LCD_WriteData(' ');
					LCD_Cursor(15);
					LCD_WriteData(' ');
					LCD_Cursor(17);
					LCD_WriteData(6);
					LCD_Cursor(18);
					LCD_WriteData(7);
					LCD_Cursor(31);
					LCD_WriteData(' ');
					LCD_Cursor(32);
					LCD_WriteData(' ');
				}
				Dis_CNT = 0;
			}
			out = GetKeypadKey();
			cnt++;
			if(out != '\0' && cnt >= 15 && stop < 4) {
				stop++;
				if(out != password[hpp])
					same = 0;
				hpp++;
				cnt = 0;
			}
			break;
		case CStop:
			cnt++;
			Sreq = 0;
			Creq = 0;
			turn_off = 1;
			stop = 0;
			break;
		case CDone:
			T_ack = 1;
			time_i = time_di;
			time_z = time_dz;
			time_dz = 0;
			time_di = 0;
			break;
	}
};
enum sound {Swait, Salarm, Sdone} sound_s;
void tick_SND() {
	switch(sound_s) {
		case -1: 
			sound_s = Swait;
			break;
		case Swait:
			if(!Sreq) {
				sound_s = Swait;
			}
			else {
				sound_s = Salarm;
			}
			break;
		case Salarm:
			if(turn_off) 
				sound_s = Sdone;
			else 
				sound_s = Salarm;
			break;
		case Sdone:
			sound_s = Swait;
			break;
	}
	switch(sound_s) {
		case Swait:
			break;
		case Salarm:
			ALARMON = 1;
			break;
		case Sdone:
			ALARMON = 0;
			Sreq = 0;
			break;
	}
};