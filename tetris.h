#include "shared.h"
#include "music.h"
//enum states{TITLE, VIEWHS, SETNAME, GAME};
#define valid(x,y) (((board[y]>>(15-x)) & 0x01) == 0)
#define checkValid(a,b,c,d) (valid(getx(a),gety(a)) && valid(getx(b),gety(b)) && valid(getx(c),gety(c)) && valid(getx(d),gety(d)))
//#define notOob(a,b,c,d) (notoob(getx(a)) && notoob(gety(a)) && notoob(getx(b)) && notoob(gety(b)) && notoob(getx(c)) && notoob(gety(c)) && notoob(getx(d)) && notoob(gety(d)))

#define notOob(a,b,c,d,x,y) (notoob((short)getx(a)+x) && notoob((short)gety(a)+y) && notoob((short)getx(b)+x) && notoob((short)gety(b)+y) && notoob((short)getx(c)+x) && notoob((short)gety(c)+y) && notoob((short)getx(d)+x) && notoob((short)gety(d)+y))
#define oob(x) ((x<0) || (x>15))
#define notoob(x) ((x>-1) && (x<16))
#define min(a, b) ((a<b)?a:b)
#define getx(x) (x>>4)
#define gety(x) (x & 0x0F)
#define CW  1
#define CCW 0
#define inDir (input & 0b000011)
#define inRot ((input & 0b001100)>>2)
#define inDrop ((input & 0b010000)>>4)
#define inHarddrop ((input & 0b100000)>>4)
enum blocks { L, J, S, Z, I, O, T };
uc nextBlock = I;
uc curBlock = I;
uc tetromino[7];
short board[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uc rand2()
{
	static const uc table[127] ={0b000110,0b000101,0b011110,0b011100,0b010100,0b110110,0b101101,0b101011,0b010100,0b110000,0b000100,0b011000,0b011110,0b110100,0b101100,0b001001,0b011010,0b001011,0b000101,0b000110,0b001110,0b100010,0b001101,0b010000,0b001110,0b110011,0b100000,0b001010,0b011101,0b110011,0b110001,0b011001,0b011011,0b011010,0b000010,0b100101,0b110010,0b110110,0b110001,0b100010,0b011011,0b011100,0b011100,0b000000,0b110000,0b010110,0b000011,0b001100,0b101000,0b101110,0b100001,0b011101,0b101000,0b101101,0b000001,0b001000,0b001001,0b000000,0b100000,0b000001,0b011110,0b101010,0b000101,0b100110,0b011101,0b100010,0b110101,0b011011,0b110101,0b011011,0b011001,0b110101,0b010110,0b001101,0b010011,0b010011,0b010010,0b001101,0b110110,0b011101,0b000010,0b100011,0b000000,0b100001,0b010010,0b100001,0b100001,0b010101,0b000010,0b110101,0b011001,0b011000,0b000011,0b000100,0b110110,0b000000,0b010110,0b101001,0b100110,0b010100,0b101100,0b011101,0b100001,0b011110,0b100010,0b010001,0b100011,0b000000,0b000000,0b001110,0b101011,0b010010,0b000011,0b110101,0b011010,0b010001,0b011010,0b011101,0b011101,0b001110,0b010001,0b000101,0b011100,0b110000,0b001011,0b001001,0b011000};
	static uc pos = -1;
	if (pos == 255)
		pos = 0;
	else
		pos++;
	if (pos % 2 == 1)
	{
		return getx(table[pos / 2]);
	}
	return gety(table[pos / 2]);
}

void spawnBlock()
{
	switch (nextBlock)
	{
	case L:
		tetromino[0] = 0x60;
		tetromino[1] = 0x61;
		tetromino[2] = 0x71;
		tetromino[3] = 0x81;
		tetromino[4] = 0x00;
		tetromino[5] = 3;
		tetromino[6] = 1;
		break;
	case J:
		tetromino[0] = 0x80;
		tetromino[1] = 0x81;
		tetromino[2] = 0x71;
		tetromino[3] = 0x61;
		tetromino[4] = 0x10;
		tetromino[5] = 3 << 4;
		tetromino[6] = 1;
		break;
	case S:
		tetromino[0] = 0x80;
		tetromino[1] = 0x70;
		tetromino[2] = 0x71;
		tetromino[3] = 0x61;
		tetromino[4] = 0x20;
		tetromino[5] = 3;
		tetromino[6] = 3;
		break;
	case Z:
		tetromino[0] = 0x60;
		tetromino[1] = 0x70;
		tetromino[2] = 0x71;
		tetromino[3] = 0x81;
		tetromino[4] = 0x30;
		tetromino[5] = 3;
		tetromino[6] = 3;
		break;
	case I:
		tetromino[0] = 0x61;
		tetromino[1] = 0x71;
		tetromino[2] = 0x81;
		tetromino[3] = 0x91;
		tetromino[4] = 0x40;
		tetromino[5] = 3;
		tetromino[6] = 0;
		break;
	case O:
		tetromino[0] = 0x70;
		tetromino[1] = 0x71;
		tetromino[2] = 0x80;
		tetromino[3] = 0x81;
		tetromino[4] = 0x50;
		tetromino[5] = 2;
		tetromino[6] = 1;
		break;
	case T:
		tetromino[0] = 0x70;
		tetromino[1] = 0x61;
		tetromino[2] = 0x71;
		tetromino[3] = 0x81;
		tetromino[4] = 0x60;
		tetromino[5] = (3 << 4) | 1;
		tetromino[6] = 1;
		break;
	}
	curBlock = nextBlock;
	nextBlock = rand2();
	if (!checkValid(tetromino[0], tetromino[1], tetromino[2], tetromino[3]))
		gameover = 1;
}

void harddrop()
{
	uc val0 = 0, val1 = 0, val2 = 0, val3 = 0;
	for (; val0 < 16; val0++) if (oob(gety(tetromino[0]) + (short)val0) || !valid(getx(tetromino[0]), gety(tetromino[0]) + val0)) {break;}
	for (; val1 < 16; val1++) if (oob(gety(tetromino[1]) + (short)val1) || !valid(getx(tetromino[1]), gety(tetromino[1]) + val1)) {break;}
	for (; val2 < 16; val2++) if (oob(gety(tetromino[2]) + (short)val2) || !valid(getx(tetromino[2]), gety(tetromino[2]) + val2)) {break;}
	for (; val3 < 16; val3++) if (oob(gety(tetromino[3]) + (short)val3) || !valid(getx(tetromino[3]), gety(tetromino[3]) + val3)) {break;}
	val0 = min(min(val0, val1), min(val2, val3)) - 1;
	if (val0 == -1 || val0 == 0)
		return;
	tetromino[0] += val0;
	tetromino[1] += val0;
	tetromino[2] += val0;
	tetromino[3] += val0;
	score += val0 * 2;
}

void drop()
{
	uc dist;
	if (level < 16) dist = 1;
	else dist = level - 14;
	tetromino[0] += dist;
	tetromino[1] += dist;
	tetromino[2] += dist;
	tetromino[3] += dist;
}
uc canDrop()
{
	short dist;
	if (level < 16) dist = 1;
	else dist = level - 14;
	//if (!valid(getx(tetromino[0]),gety(tetromino[0]) + dist))
	//	message = "invalid: 0";
	//if (!valid(getx(tetromino[1]), gety(tetromino[1]) + dist))
	//	message = "invalid: 1";
	//if (!valid(getx(tetromino[2]), gety(tetromino[2]) + dist))
	//	message = "invalid: 2";
	//if (!valid(getx(tetromino[3]), gety(tetromino[3]) + dist))
	//{
	//	message = "invalid: 3 tetromino[3] x:" + to_string(getx(tetromino[3])) + ", y:" + to_string(gety(tetromino[3]));
	//	pause = 1;
	//}
	
	return(notOob(tetromino[0], tetromino[1], tetromino[2], tetromino[3], 0, dist) && checkValid(tetromino[0] + dist, tetromino[1] + dist, tetromino[2] + dist, tetromino[3] + dist));
}

void rotate()
{
	uc rotation = 0;
	if (inRot == 1)
		rotation = gety(tetromino[4]) + 1;
	else if (inRot == 2)
		rotation = gety(tetromino[4]) - 1;
	else
		return;

	if (rotation == 4)
		rotation = 0;
	else if (rotation == 255)
		rotation = 3;
	//message += to_string(rotation) + ' ';
	short temp0 = 0, temp1 = 0, temp3 = 0, temp5 = 0, temp6 = 0;
	uc temp2 = tetromino[2];
	uc x = temp2 & 0xF0, y = gety(temp2);
	//perform rotation
	switch (getx(tetromino[4]))
	{
	case L:
		switch (rotation)
		{
		case 0:
			temp0 = (x - 0x10) | (y - 1);
			temp1 = (x - 0x10) | (y);
			temp3 = (x + 0x10) | (y);
			temp5 = 3;
			temp6 = 1;
			break;
		case 1:
			temp0 = (x + 0x10) | (y - 1);
			temp1 = (x) | (y - 1);
			temp3 = (x) | (y + 1);
			temp5 = 3;
			temp6 = 3;
			break;
		case 2:
			temp0 = (x + 0x10) | (y + 1);
			temp1 = (x + 0x10) | (y);
			temp3 = (x - 0x10) | (y);
			temp5 = 3 << 4;
			temp6 = (1 << 4);
			break;
		case 3:
			temp0 = (x - 0x10) | (y + 1);
			temp1 = (x) | (y + 1);
			temp3 = (x) | (y - 1);
			temp5 = 3;
			temp6 = (3 << 4);
			break;
		}
		break;
	case J:
		switch (rotation)
		{
		case 0:
			temp0 = (x + 0x10) | (y - 1);
			temp1 = (x + 0x10) | (y);
			temp3 = (x - 0x10) | (y);
			temp5 = 3 << 4;
			temp6 = 1;
			break;
		case 1:
			temp0 = (x + 0x10) | (y + 1);
			temp1 = (x) | (y + 1);
			temp3 = (x) | (y - 1);
			temp5 = 3 << 4;
			temp6 = (3 << 4);
			break;
		case 2:
			temp0 = (x - 0x10) | (y + 1);
			temp1 = (x - 0x10) | (y);
			temp3 = (x + 0x10) | (y);
			temp5 = 3;
			temp6 = (1 << 4);
			break;
		case 3:
			temp0 = (x) | (y + 1);
			temp1 = (x) | (y - 1);
			temp3 = (x - 0x10) | (y - 1);
			temp5 = 3 << 4;
			temp6 = (1 << 4);
			break;
		}
		break;
	case S:
		switch (rotation)
		{
		case 0:
			temp0 = (x + 0x10) | (y - 1);
			temp1 = (x) | (y - 1);
			temp3 = (x - 0x10) | (y);
			temp5 = 3 << 4;
			temp6 = 3;
			break;
		case 1:
			temp0 = (x + 0x10) | (y + 1);
			temp1 = (x + 0x10) | (y);
			temp3 = (x) | (y - 1);
			temp5 = 3 << 4;
			temp6 = (3 << 4);
			break;
		case 2:
			temp0 = (x - 0x10) | (y + 1);
			temp1 = (x) | (y + 1);
			temp3 = (x + 0x10) | (y);
			temp5 = 3;
			temp6 = (3 << 4);
			break;
		case 3:
			temp0 = (x - 0x10) | (y - 1);
			temp1 = (x - 0x10) | (y);
			temp3 = (x) | (y + 1);
			temp5 = 3;
			temp6 = 3;
			break;
		}
		break;
	case Z:
		switch (rotation)
		{
		case 0:
			temp0 = (x - 0x10) | (y - 1);
			temp1 = (x) | (y - 1);
			temp3 = (x + 0x10) | (y);
			temp5 = 3;
			temp6 = 3;
			break;
		case 1:
			temp0 = (x) | (y + 1);
			temp1 = (x + 0x10) | (y);
			temp3 = (x + 0x10) | (y - 1);
			temp5 = (3 << 4);
			temp6 = (3 << 4);
			break;
		case 2:
			temp0 = (x + 0x10) | (y + 1);
			temp1 = (x) | (y + 1);
			temp3 = (x - 0x10) | (y);
			temp5 = (3 << 4);
			temp6 = (3 << 4);
			break;
		case 3:
			temp0 = (x) | (y - 1);
			temp1 = (x - 0x10) | (y);
			temp3 = (x - 0x10) | (y + 1);
			temp5 = (3 << 4);
			temp6 = 3;
			break;
		}
		break;
	case I:
		switch (rotation)
		{
		case 0:
		case 2:
			temp0 = (x - 0x20) | (y);
			temp1 = (x - 0x10) | (y);
			temp3 = (x + 0x10) | (y);
			temp5 = 3;
			temp6 = 0;
			break;
		case 1:
		case 3:
			temp0 = (x) | (y + 2);
			temp1 = (x) | (y + 1);
			temp3 = (x) | (y - 1);
			temp5 = 0;
			temp6 = 3 << 4;
			break;
		}
		break;
	case O:
		return;
	case T:
		switch (rotation)
		{
		case 0:
			temp0 = (x) | (y - 1);
			temp1 = (x + 0x10) | (y);
			temp3 = (x - 0x10) | (y);
			temp5 = (3 << 4) | 1;
			temp6 = 1;
			break;
		case 1:
			temp0 = (x + 0x10) | (y);
			temp1 = (x) | (y + 1);
			temp3 = (x) | (y - 1);
			temp5 = 3 << 4;
			temp6 = (3 << 4) | 1;
			break;
		case 2:
			temp0 = (x) | (y + 1);
			temp1 = (x + 0x10) | (y);
			temp3 = (x - 0x10) | (y);
			temp5 = (3 << 4) | 1;
			temp6 = (3 << 4);
			break;
		case 3:
			temp0 = (x - 0x10) | (y);
			temp1 = (x) | (y + 1);
			temp3 = (x) | (y - 1);
			temp5 = 1;
			temp6 = (3 << 4) | 1;
			break;
		}
		break;
	}
	if (notOob(temp0, temp1, temp2, temp3, 0, 0) && checkValid(temp0, temp1, temp2, temp3))
	{
		tetromino[0] = temp0;
		tetromino[1] = temp1;
		tetromino[2] = temp2;
		tetromino[3] = temp3;
		tetromino[4] = (tetromino[4] & 0xF0) | rotation;
		tetromino[5] = temp5;
		tetromino[6] = temp6;
	}
	else if (notOob(temp0, temp1, temp2, temp3, 1, 0) && checkValid(temp0 + 0x10, temp1 + 0x10, temp2 + 0x10, temp3 + 0x10))
	{
		tetromino[0] = temp0 + 0x10;
		tetromino[1] = temp1 + 0x10;
		tetromino[2] = temp2 + 0x10;
		tetromino[3] = temp3 + 0x10;
		tetromino[4] = (tetromino[4] & 0xF0) | rotation;
		tetromino[5] = temp5;
		tetromino[6] = temp6;
	}
	else if (notOob(temp0, temp1, temp2, temp3, -1, 0) && checkValid(temp0 - 0x10, temp1 - 0x10, temp2 - 0x10, temp3 - 0x10))
	{
		tetromino[0] = temp0 - 0x10;
		tetromino[1] = temp1 - 0x10;
		tetromino[2] = temp2 - 0x10;
		tetromino[3] = temp3 - 0x10;
		tetromino[4] = (tetromino[4] & 0xF0) | rotation;
		tetromino[5] = temp5;
		tetromino[6] = temp6;
	}
	else if (notOob(temp0, temp1, temp2, temp3, 0, -1) && checkValid(temp0 - 0x01, temp1 - 0x01, temp2 - 0x01, temp3 - 0x01))
	{
		tetromino[0] = temp0 - 0x01;
		tetromino[1] = temp1 - 0x01;
		tetromino[2] = temp2 - 0x01;
		tetromino[3] = temp3 - 0x01;
		tetromino[4] = (tetromino[4] & 0xF0) | rotation;
		tetromino[5] = temp5;
		tetromino[6] = temp6;
	}
	/*else
		return 0; // invalid
	return 1; // valid*/
}
void move()
{
	//if (!notoob(getx(tetromino[3])+(short)1))
		//message += "tetrominoo3: "+to_string(getx(tetromino[3]))+ "tetrominoo2: " + to_string(getx(tetromino[2]))+ "tetrominoo1: " + to_string(getx(tetromino[1]))+ "tetromino0: " + to_string(getx(tetromino[0]));
	if (inDir == 1 && notOob(tetromino[0], tetromino[1], tetromino[2], tetromino[3], 1, 0) && checkValid(tetromino[0]+0x10, tetromino[1] + 0x10, tetromino[2] + 0x10, tetromino[3] + 0x10))
	{
		tetromino[0] += 0x10;
		tetromino[1] += 0x10;
		tetromino[2] += 0x10;
		tetromino[3] += 0x10;
		return;
	}

	if (inDir == 2 && notOob(tetromino[0], tetromino[1], tetromino[2], tetromino[3], -1, 0) && checkValid(tetromino[0] - 0x10, tetromino[1] - 0x10, tetromino[2] - 0x10, tetromino[3] - 0x10))
	{
		tetromino[0] -= 0x10;
		tetromino[1] -= 0x10;
		tetromino[2] -= 0x10;
		tetromino[3] -= 0x10;
	}
}

uc waitOver()
{
	//[1.00000,0.79300,0.61780,0.47273,0.35520,0.26200,0.18968,0.13473,0.09388,0.06415,0.04298,0.02822,0.01815,0.01144,0.00706,0.00426,0.00252,0.00146,0.00082,0.00046]
	static uc delay[15] = { 100, 79, 62, 47, 36, 26, 19, 13, 9, 6, 4, 3, 2, 1, 1 };
	static uc count = 0;
	count++;
	if (level < 16)
	{
		if ((count - 1) == (short)delay[level - 1])
		{
			count = 0;
			return 1;
		}
		else
			return 0;
	}
		
	return 1;
}

void lock()
{
	board[gety(tetromino[0])] |= 1 << (15 - getx(tetromino[0]));
	board[gety(tetromino[1])] |= 1 << (15 - getx(tetromino[1]));
	board[gety(tetromino[2])] |= 1 << (15 - getx(tetromino[2]));
	board[gety(tetromino[3])] |= 1 << (15 - getx(tetromino[3]));
}
void clear()
{
#define numlines (gety(tetromino[gety(tetromino[6])])-gety(tetromino[getx(tetromino[6])]) + 1)
	static uc combo = 0;
	uc cleared = 0, oldlvl = level;
	uc lines[5] = { 255, 255, 255, 255, 255};
	for (uc i = 0; i < numlines; i++)
	{
		// get every line tetromino was added to
		lines[i] = gety(tetromino[getx(tetromino[6])]) + i;
	}
	for (uc i = 0; lines[i] != 255; i++)
	{
		if (board[lines[i]] == -1)
		{
			musicstate = JMP;
			// increase level
			line++;
			cleared++;
			if (line % 10 == 0)
			{
				level++;
			}
			// shift lines down
			for (uc j = lines[i]; j > 0; j--)
			{
				board[j] = board[j - 1];
				if (board[j] == 0)
					break;
			}

		}
	}
	if (cleared)
	{
		if (cleared == 1)
			score += 100 * oldlvl;
		else if (cleared == 2)
			score += 300 * oldlvl;
		else if (cleared == 3)
			score += 500 * oldlvl;
		else if (cleared == 4)
			score += 800 * oldlvl;
		score += combo * 50 * oldlvl;
		combo++;
		showstats = 1;
	}
	else
		combo = 0;
}

enum tetrisStates {GAMESTART, SPAWN, WAIT, FALL, HARDDROP, LOCK, CLEAR, OVER };
uc tetris(uc state)
{
	static uc lines; // to be cleared
	if (pause) return state;
	switch (gamestate)
	{
		case GAMESTART:
		if(begingame) gamestate = SPAWN;
		break;
		case SPAWN:
		gamestate = WAIT;
		break;

		case WAIT:
		if (gameover) {
			gamestate = OVER;
			break;
		}
		if (inHarddrop) gamestate = HARDDROP;
		else if (inDrop || waitOver()) gamestate = FALL;
		break;

		case FALL:
		if (inHarddrop) { gamestate = HARDDROP; break; }
		move();
		rotate();
		gamestate = canDrop() ? WAIT : LOCK;
		break;

		case HARDDROP:
		gamestate = LOCK;
		break;

		case LOCK:
		gamestate = CLEAR;
		break;

		case CLEAR:
		gamestate = SPAWN;
		break;

		default:
		break;
	}
	switch (gamestate)
	{
		case SPAWN:
		spawnBlock();
		displayBlock = 1;
		break;

		case FALL:
		drop();
		break;

		case WAIT:
		move();
		rotate();
		break;

		case LOCK:
		lock();
		break;

		case HARDDROP:
		harddrop();
		break;
		case CLEAR:
		clear();
		break;
		default:
		break;
	}
	return state;
}