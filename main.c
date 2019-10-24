#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
#include "bit.h"
#include "timer.h"
#include "pwm.h"
#include "tetris.h"
#include "music.h"
typedef unsigned char uc;
//#include <stdio.h>
uc imgFromBlock(uc block)
{
	switch(block)
	{
		case L:
		return 0b00110101;
		case J:
		return 0b01100101;
		case S:
		return 0b00111100;
		case Z:
		return 0b01100110;
		case I:
		return 0b10100101;
		case O:
		return 0b00110110;
		case T:
		return 0b01110100;
	}
	return 0;
}
uc SR(uc img)
{
	static uc pos = 0;
	uc SER		= PORTD & 1;
	uc RCLK		= PORTD & 2;
	uc SRCLK	= PORTD & 4;
	uc SRCLR	= PORTD & 8;
	if(pos == 9)
	{
		RCLK = 2;
		pos ++;
	}
	else if(pos == 10)
	{
		SRCLR = 0;
		pos = 0;
	}
	else
	{
		if(SRCLK)
		{
			RCLK = 0;
			SRCLR = 8;
			SRCLK = 0;
			SER = (img>>pos) & 1;
			pos++;
		}
		else
		{
			SRCLK = 4;
		}
	}
	return SER | RCLK | SRCLK | SRCLR;
}
unsigned char GetKeypadKey() {

	PORTA = 0xEF; // Enable col 4 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTA to stabilize before checking
	if (GetBit(PINA,0)==0) { return('1'); }
	if (GetBit(PINA,1)==0) { return('4'); }
	if (GetBit(PINA,2)==0) { return('7'); }
	if (GetBit(PINA,3)==0) { return('*'); }

	// Check keys in col 2
	PORTA = 0xDF; // Enable col 5 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTA to stabilize before checking
	if (GetBit(PINA,0)==0) { return('2'); }
	if (GetBit(PINA,1)==0) { return('5'); }
	if (GetBit(PINA,2)==0) { return('8'); }
	if (GetBit(PINA,3)==0) { return('0'); }
	// ... *****FINISH*****

	// Check keys in col 3
	PORTA = 0xBF; // Enable col 6 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTA to stabilize before checking
	// ... *****FINISH*****
	if (GetBit(PINA,0)==0) { return('3'); }
	if (GetBit(PINA,1)==0) { return('6'); }
	if (GetBit(PINA,2)==0) { return('9'); }
	if (GetBit(PINA,3)==0) { return('#'); }

	// Check keys in col 4
	// ... *****FINISH*****
	PORTA = 0x7F; // Enable col 7 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTA to stabilize before checking
	if (GetBit(PINA,0)==0) { return('A'); }
	if (GetBit(PINA,1)==0) { return('B'); }
	if (GetBit(PINA,2)==0) { return('C'); }
	if (GetBit(PINA,3)==0) { return('D'); }

	return('\0'); // default value

}

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
//--------End find GCD function ----------------------------------------------


//--------User defined FSMs---------------------------------------------------
//Enumeration of states.

// Monitors button connected to PA0. 
// When button is pressed, shared variable "pause" is toggled.
int SMTick1(int state) {
	static uc x = 0;
	input = 0;
	uc y = GetKeypadKey();
	if(y == 'A')
	input |= 2;
	else if (y=='3')
	input |= 1;
	else if (y=='2')
	input |= 4;
	else if (y=='1')
	input |= 8;
	else if (y=='B')
	input |= 16;
	else if (y=='6')
	input |= 32;
	else if (y == 'D')
	{
		musicstate = NONE;
		resetGame();
		displaystate = 1;
	}
	else if(y == '*')
	pause = !pause;
	x = y;
	return state;
}

//int SMTick2(int state) {
	//static uc x = 0;
	//uc y = !(PINB & 0x01);
	//if(y && !x)
	//{
		//musicstate = NONE;
		//resetGame();
		//displaystate = 1;
	//}
	//x = y;
	//return state;
//}

int SMTick3(int state) {
static uc oldBlock = 0;
if(!displayBlock)
{
	PORTD = (PORTD&0xF0)|SR(0);
}
else
{
	PORTD = (PORTD&0xF0)|SR(imgFromBlock(nextBlock));
}
return state;
}


//Enumeration of states.
/*
enum SM4_States { SM4_display };
	*/

void itos(unsigned short n, char* str, uc bound)
{
	if (n == 0)
	{
		str[0] = '0';
		for(uc i = 1; i < bound; i++)
			str[i] = ' ';
		return;
	}
	uc vec[bound];
	uc veclen = 0;
	for (; n != 0; n /= 10) {
		vec[veclen] = (n % 10) + '0';
		veclen++;
		if (veclen == bound)
		break;
	}
	for(uc i = veclen; i < bound; i++)
		str[i] = ' ';
	for (uc i = 0; i < veclen; i++)
	{
		str[i] = vec[veclen - i -1];
	}
}

void boardToChar()
{
	short board2[16];
	for(uc i = 0; i<16; i++)
	{
		board2[i] = board[i];
	}
	board2[gety(tetromino[0])] |= 1<<(15-getx(tetromino[0]));
	board2[gety(tetromino[1])] |= 1<<(15-getx(tetromino[1]));
	board2[gety(tetromino[2])] |= 1<<(15-getx(tetromino[2]));
	board2[gety(tetromino[3])] |= 1<<(15-getx(tetromino[3]));
	
	uc ch0[8], ch1[8], ch2[8], ch3[8], ch4[8], ch5[8], ch6[8], ch7[8];
	for(uc i = 0; i<8; i++)
	{
		ch0[i] = board2[i]>>11;
		ch1[i] = (board2[i]&0b11111000000)>>6;
		ch2[i] = (board2[i]&0b111110)>>1;
		ch3[i] = (board2[i]&1)<<4|0xE;
		ch4[i] = board2[i+8]>>11;
		ch5[i] = (board2[i+8]&0b11111000000)>>6;
		ch6[i] = (board2[i+8]&0b111110)>>1;
		ch7[i] = (board2[i+8]&1)<<4|0xE;
	}
	writeChar(0,ch0);
	writeChar(1,ch1);
	writeChar(2,ch2);
	writeChar(3,ch3);
	writeChar(4,ch4);
	writeChar(5,ch5);
	writeChar(6,ch6);
	writeChar(7,ch7);
}

void resetGame()
{
	gameover = 0;
	begingame = 0;
	score = 0;
	showstats = 0;
	line = 0;
	level = 1;
	displayBlock = 0;
	gamestate = 0;
	board[0] = 0;board[4] = 0;board[8] = 0;board[12] = 0;
	board[1] = 0;board[5] = 0;board[9] = 0;board[13] = 0;
	board[2] = 0;board[6] = 0;board[10] = 0;board[14] = 0;
	board[3] = 0;board[7] = 0;board[11] = 0;board[15] = 0;
}

// Combine blinking LED outputs from SM2 and SM3, and output on PORTB
unsigned short highscore = 0;
char name[6] = {'A','A','A','A','A','A'};
#define inLR (input&0b11)
#define inUD ((input&0b1100)>>2)
#define inEnter (input&0b10000)
enum dsStates{dsLose, dsTitle,dsTitleCur, dsGame, dsGameInit, dsHighInit, dsHigh, dsScore, dsScoreWait};
uc display(uc state) {
	state = displaystate;
	char str[32];
	static uc cursor = 0;
	static uc currentChar = 0;
	static uc oldUD = 0;
	static uc oldLR = 0;
	static uc oldEnter = 0;
	static short animCounter = 0;
	char scorestr[9];
	// =============================
	//        state transitions
	// =============================
	switch(state)
	{
		case dsTitle:
		LCD_DisplayString(1, "TETRIS  Start \x7F         Score   ");
		animCounter = 0;
		state = dsTitleCur;
		cursor = 0;
		break;
		
		case dsTitleCur:
		if(inEnter && !oldEnter && cursor == 0)
		{
			// Remove line later vv
			//LCD_DisplayString(1,"                                ");
			state = dsGameInit;
			begingame = 1;
		}
		if(inEnter && !oldEnter && cursor == 1)
		{
			//LCD_DisplayString(1,"                                ");
			state = dsScore;
		}
		break;
		
		case dsHighInit:
		state = dsHigh;
		break;
		
		case dsHigh:
		if (inEnter && !oldEnter && currentChar == 6)
		{
			//currentChar = 0;
			state = dsTitle;
			musicstate = NONE;
		}
		break;
		
		case dsScore:
		state = dsScoreWait;
		oldEnter = inEnter;
		break;
		
		case dsScoreWait:
		if(inEnter && !oldEnter) state = dsTitle;
		oldEnter = inEnter;
		break;
		
		case dsGameInit:
		state = dsGame;
		break;
		
		case dsLose:
		if(animCounter == 300)
		{
			if(score > highscore)
			{
				animCounter = 0;
				state = dsHighInit;
			}
			else if(inEnter && !oldEnter)
			{
				resetGame();
				animCounter = 0;
				state = dsTitle;
				musicstate = NONE;
				oldEnter = inEnter;
			}
		}
		break;
		
		
		case dsGame:
		if(gameover)
			state = dsLose;
		break;
		default:
		break;
	}
	// =============================
	//        state actions
	// =============================
	switch(state)
	{
		case dsTitleCur:
			PORTB |= 0b10;
			if(inLR == 1 && cursor == 0)
			{
				LCD_Cursor(15);
				LCD_WriteData(' ');
				LCD_Cursor(31);
				LCD_WriteData('\x7F');
				cursor = 1;
			}
			else if(inLR == 2 && cursor == 1)
			{
				LCD_Cursor(15);
				LCD_WriteData('\x7F');
				LCD_Cursor(31);
				LCD_WriteData(' ');
				cursor = 0;
			}
			oldEnter = inEnter;
			break;
				
		case dsHighInit:
			musicstate = WIN;
			highscore = score;
			name[0] = 'A', name[1] = 'A',name[2] = 'A',name[3] = 'A',name[4] = 'A',name[5] = 'A';
			memcpy(str,"AAAAAA ",7);
			itos(score, scorestr, 9);
			memcpy(str+7, scorestr, 9);
			memcpy(str+16,"^        Enter  ", 16);
			LCD_DisplayString(1, str);
			state = dsHigh;
			resetGame();
			break;
		
		case dsHigh:
		if(inUD == 1 && !oldUD && currentChar != 6)
		{
			if(name[currentChar]=='\x21')
				name[currentChar] = '\xFD';
			else if(name[currentChar]=='\xA1')
				name[currentChar] = '\x7F';
			else
				name[currentChar]--;
			LCD_Cursor(currentChar+1); LCD_WriteData(name[currentChar]);
		}
		else if(inUD == 2 && !oldUD && currentChar != 6)
		{
			if(name[currentChar]=='\xFD')
				name[currentChar] = '\x21';
			else if(name[currentChar]=='\x7F')
				name[currentChar] = '\xA1';
			else
				name[currentChar]++;
			LCD_Cursor(currentChar+1); LCD_WriteData(name[currentChar]);
		}
		else if(inLR == 2 && !oldLR )
		{
			if(currentChar != 0)
			{
				if(currentChar == 6)
					LCD_Cursor(24);
				else
					LCD_Cursor(currentChar+17);
				LCD_WriteData(' ');
				LCD_Cursor(currentChar+16); LCD_WriteData('^');
				currentChar--;
			}
		}
		else if (inLR == 1 && !oldLR )
		{
			LCD_Cursor(currentChar+16);
			LCD_WriteData(' ');
			if(currentChar != 6)
			{
				LCD_Cursor(currentChar+17);
				LCD_WriteData('^');
				currentChar++;
			}
			else
			{
				LCD_Cursor(24);
				LCD_WriteData('>');
			}
		}
		//LCD_Cursor(16); LCD_WriteData(currentChar+'0');
		oldLR = inLR;
		oldUD = inUD;
		oldEnter = inEnter;
		break;		
		
		case dsLose:
		musicstate = DIE;
		switch(animCounter)
		{
			case 0:
			LCD_Cursor(13); LCD_WriteData('G');break;
			case 10:
			LCD_Cursor(14); LCD_WriteData('A');break;
			case 20:
			LCD_Cursor(15); LCD_WriteData('M');break;
			case 30:
			LCD_Cursor(16); LCD_WriteData('E');break;
			case 40:
			LCD_Cursor(29); LCD_WriteData('O');break;
			case 50:
			LCD_Cursor(30); LCD_WriteData('V');break;
			case 60:
			LCD_Cursor(31); LCD_WriteData('E');break;
			case 70:
			LCD_Cursor(32); LCD_WriteData('R');break;
			case 300:
			musicstate = GOVR;
			break;
		}
		if(animCounter < 300)
			animCounter++;
		break;
		
		case dsScore:
		memcpy(str, name, 6);
		str[6] = ' ';
		itos(highscore, scorestr, 9);
		memcpy(str+7, scorestr, 9);
		memcpy(str+16, "          \x7E Back", 16);
		LCD_DisplayString(1,str);
		break;
		
		case dsGameInit:
		musicstate = MUS;
		memcpy(str,"S ", 2);
		itos(score, scorestr, 9);
		memcpy(str+2, scorestr, 9);
		memcpy(str+11, " \0\1\2\3", 5);
		memcpy(str+16, "LV ", 3);
		char levelstr[2];
		itos(level, levelstr, 2);
		memcpy(str+19, levelstr, 2);
		memcpy(str+21, " L ", 3);
		char linestr[3];
		itos(line, linestr, 3);
		memcpy(str+24, linestr, 3);
		memcpy(str+27, " \4\5\6\7", 5);
		LCD_DisplayString(1, str);
		break;
		
		case dsGame:
		if(showstats)
		{
			char scorestr[9];
			itos(score, scorestr, 9);
			char* s = scorestr;
			for(uc i = 3; i < 12; i++){LCD_Cursor(i); LCD_WriteData(*s++);}
			
			char levelstr[2];
			itos(level, levelstr, 2);
			char* lv = levelstr;
			for(uc i = 20; i < 22; i++){LCD_Cursor(i); LCD_WriteData(*lv++);}
			
			char linestr[3];
			itos(line, linestr, 3);
			char*li = linestr;
			for(uc i = 25; i < 28; i++){LCD_Cursor(i); LCD_WriteData(*li++);}
			showstats = 0;
		}
		boardToChar();
		LCD_Cursor(13); LCD_WriteData('\0');
		LCD_Cursor(14); LCD_WriteData('\1');
		LCD_Cursor(15); LCD_WriteData('\2');
		LCD_Cursor(16); LCD_WriteData('\3');
		LCD_Cursor(29); LCD_WriteData('\4');
		LCD_Cursor(30); LCD_WriteData('\5');
		LCD_Cursor(31); LCD_WriteData('\6');
		LCD_Cursor(32); LCD_WriteData('\7');
		break;
		
		default:
		break;
	}
	displaystate = state;
	return state;
}

// --------END User defined FSMs-----------------------------------------------

// Implement scheduler code from PES.
int main()
{
// Set Data Direction Registers
// Buttons PORTA[0-7], set AVR PORTA to pull down logic
DDRA = 0xF0; PORTA = 0x0F;
DDRB = 0xFF; PORTB = 0x00;
DDRC = 0xFF; PORTC = 0x00;
DDRD = 0xFF; PORTD = 0x00;
// . . . etc
LCD_init();
uc bytes[] = {0b01000,  0, 0b10000,  0, 0,0,0,0};
writeChar(0,bytes);
SR(0);
// Period for the tasks
unsigned long int SMTick1_calc = 10;
unsigned long int SMTick2_calc = 10;
unsigned long int SMTick3_calc = 10;
unsigned long int SMTick4_calc = 10;
unsigned long int SMTick5_calc = 10;

//Calculating GCD
unsigned long int tmpGCD = 1;
tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
tmpGCD = findGCD(tmpGCD, SMTick3_calc);
tmpGCD = findGCD(tmpGCD, SMTick4_calc);

//Greatest common divisor for all tasks or smallest time unit for tasks.
//unsigned long int GCD = tmpGCD;
unsigned long int GCD = 10;

//Recalculate GCD periods for scheduler
unsigned long int SMTick1_period = SMTick1_calc/GCD;
unsigned long int SMTick2_period = SMTick2_calc/GCD;
unsigned long int SMTick3_period = SMTick3_calc/GCD;
unsigned long int SMTick4_period = SMTick4_calc/GCD;
unsigned long int SMTick5_period = SMTick5_calc/GCD;

//Declare an array of tasks 
static task task1, task3, task4, task5, task6;
task *tasks[] = {&task1, &task3,&task4, &task5, &task6};
//task *tasks[] = {&task1};
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

// Task 1
task1.state = -1;//Task initial state.
task1.period = SMTick1_period;//Task Period.
task1.elapsedTime = SMTick1_period;//Task current elapsed time.
task1.TickFct = &SMTick1;//Function pointer for the tick.

// Task 2
//task2.state = -1;//Task initial state.
//task2.period = SMTick2_period;//Task Period.
//task2.elapsedTime = SMTick2_period;//Task current elapsed time.
//task2.TickFct = &SMTick2;//Function pointer for the tick.

// Task 3
task3.state = L;//Task initial state.
task3.period = SMTick3_period;//Task Period.
task3.elapsedTime = SMTick3_period; // Task current elasped time.
task3.TickFct = &SMTick3; // Function pointer for the tick.


/*
// Task 4
task4.state = -1;//Task initial state.
task4.period = SMTick4_period;//Task Period.
task4.elapsedTime = SMTick4_period; // Task current elasped time.
task4.TickFct = &SMTick4; // Function pointer for the tick.*/

// Task 4
task4.state = GAMESTART;//Task initial state.
task4.period = SMTick4_period;//Task Period.
task4.elapsedTime = SMTick4_period; // Task current elasped time.
task4.TickFct = &tetris; // Function pointer for the tick.

task5.state = NONE;
task5.period = SMTick5_period;
task5.elapsedTime = SMTick5_period;
task5.TickFct = &sound;

task6.state = dsTitle;
task6.period = 10/GCD;
task6.elapsedTime = 10/GCD;
task6.TickFct = &display;

// Set the timer and turn it on
TimerSet(GCD);
TimerOn();
PWM_on();

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
