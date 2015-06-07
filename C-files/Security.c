#include <avr/io.h>
#include <stdio.h>
#include <string.h>
unsigned short ALARMON = 0;
#include "C:\Users\John\Documents\120B\Project\bit.h"
#include "C:\Users\John\Documents\120B\Project\io.c"
#include "C:\Users\John\Documents\120B\Project\timer.h"
#include "C:\Users\John\Documents\120B\Project\Functions.c"
#include "C:\Users\John\Documents\120B\Project\Security\SM_Functions.h"
#include "C:\Users\John\Documents\120B\Project\keypad.h"
#include "C:\Users\John\Documents\120B\Project\PWM.h"
//--------Find GCD function --------------------------------------------------
unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
}
void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	// in Free Running Mode, a new conversion will trigger
	// whenever the previous conversion completes.
}
//--------End find GCD function ----------------------------------------------

//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

//--------End Task scheduler data structure-----------------------------------


//--------Shared Variables----------------------------------------------------
unsigned char SM2_output = 0x00;
unsigned char SM3_output = 0x00;
unsigned char pause = 0;

//--------End Shared Variables------------------------------------------------

int main(void)
{
	DDRA = 0xFB; PORTA = 0x04;
	DDRB = 0x40; PORTB = 0xBF; // PORTB set to output, outputs init 0s 0xF7
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xFF; PORTD = 0x00;
	LCD_init();
	ADC_init();
	PWM_on();
	Custom_Char();
	unsigned long int tmpGCD = 1;
	unsigned long int GCD = tmpGCD;
	
	unsigned long int SMTick1_calc = 10;
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	
	unsigned long int SMTick2_calc = 100;
	unsigned long int SMTick2_period = SMTick2_calc/GCD;
	
	unsigned long int SMTick3_calc = 100;
	unsigned long int SMTick3_period = SMTick3_calc/GCD;
	
	unsigned long int SMTick4_calc = 10;
	unsigned long int SMTick4_period = SMTick4_calc/GCD;
	
	unsigned long int SMTick5_calc = 100;
	unsigned long int SMTick5_period = SMTick5_calc/GCD;
	
	unsigned long int SMTick6_calc = 100;
	unsigned long int SMTick6_period = SMTick6_calc/GCD;
	
	unsigned long int SMTick7_calc = 10;
	unsigned long int SMTick7_period = SMTick7_calc/GCD;
	
	unsigned long int SMTick8_calc = 100;
	unsigned long int SMTick8_period = SMTick8_calc/GCD;
	
	static task task1, task2, task3, task4, task5, task6, task7, task8;
	task *tasks[] = {&task1, &task2, &task3, &task4, &task5, &task6, &task7, &task8};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	// Task 1
	task1.state = -1;//Task initial state.
	task1.period = SMTick1_period;//Task Period.
	task1.elapsedTime = SMTick1_period;//Task current elapsed time.
	task1.TickFct = &tick_on_off;//Function pointer for the tick.
	// Task 2
	task2.state = -1;//Task initial state.
	task2.period = SMTick2_period;//Task Period.
	task2.elapsedTime = SMTick2_period;//Task current elapsed time.
	task2.TickFct = &tick_menu;//Function pointer for the tick.
	// Task 3
	task3.state = -1;//Task initial state.
	task3.period = SMTick3_period;//Task Period.
	task3.elapsedTime = SMTick3_period;//Task current elapsed time.
	task3.TickFct = &tick_PC_change;//Function pointer for the tick.
	// Task 4
	task4.state = -1;//Task initial state.
	task4.period = SMTick4_period;//Task Period.
	task4.elapsedTime = SMTick4_period;//Task current elapsed time.
	task4.TickFct = &tick_Arm;//Function pointer for the tick.
	// Task 5
	task5.state = -1;//Task initial state.
	task5.period = SMTick5_period;//Task Period.
	task5.elapsedTime = SMTick5_period;//Task current elapsed time.
	task5.TickFct = &tick_Warn;//Function pointer for the tick.
	// Task 6
	task6.state = -1;//Task initial state.
	task6.period = SMTick6_period;//Task Period.
	task6.elapsedTime = SMTick6_period;//Task current elapsed time.
	task6.TickFct = &tick_ITime;//Function pointer for the tick.
	// Task 7
	task7.state = -1;//Task initial state.
	task7.period = SMTick7_period;//Task Period.
	task7.elapsedTime = SMTick7_period;//Task current elapsed time.
	task7.TickFct = &tick_CNT;//Function pointer for the tick.
	// Task 8
	task8.state = -1;//Task initial state.
	task8.period = SMTick8_period;//Task Period.
	task8.elapsedTime = SMTick8_period;//Task current elapsed time.
	task8.TickFct = &tick_SND;//Function pointer for the tick.
	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	unsigned short i; // Scheduler for-loop iterator
	while(1) {
		// Scheduler code
		if(ALARMON)
			set_PWM(3400);
		else 
			set_PWM(0);
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;
}