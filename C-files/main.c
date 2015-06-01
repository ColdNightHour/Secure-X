#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "C:\Users\John\Documents\120B\Project\bit.h"
#include "C:\Users\John\Documents\120B\Project\io.c"
#include "C:\Users\John\Documents\120B\Project\timer.h"
#include "C:\Users\John\Documents\120B\Project\Functions.c"
#include "C:\Users\John\Documents\120B\Project\Security\SM_Functions.h"
#include "C:\Users\John\Documents\120B\Project\keypad.h"

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
	DDRB = 0x00; PORTB = 0xFF; // PORTB set to output, outputs init 0s
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRA = 0xFF; PORTA = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	LCD_init();
	ADC_init();
	Custom_Char();
	unsigned long int tmpGCD = 1;
	unsigned long int GCD = tmpGCD;
	
	unsigned long int SMTick1_calc = 10;
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	
	unsigned long int SMTick2_calc = 100;
	unsigned long int SMTick2_period = SMTick2_calc/GCD;
	
	unsigned long int SMTick3_calc = 100;
	unsigned long int SMTick3_period = SMTick3_calc/GCD;
	
	unsigned long int SMTick4_calc = 100;
	unsigned long int SMTick4_period = SMTick4_calc/GCD;
	
	unsigned long int SMTick5_calc = 100;
	unsigned long int SMTick5_period = SMTick5_calc/GCD;
	
	unsigned long int SMTick6_calc = 100;
	unsigned long int SMTick6_period = SMTick6_calc/GCD;
	
	static task task1, task2, task3, task4, task5, task6;
	task *tasks[] = {&task1, &task2, &task3, &task4, &task5, &task6};
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
	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	unsigned short i; // Scheduler for-loop iterator
	while(1) {
		// Scheduler code
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

	// Error: Program should not exit!
	return 0;
}
