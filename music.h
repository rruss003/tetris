#include "notes.h"
#pragma once

#define hd 90
#define h 60
#define q 40
#define d 50
#define e 20
typedef unsigned char uc;

uc jumpsnd(uc state)
{
	static uc dur = 0;
	//const uc nn = 25;
	const uc nn = 12;
	const double n1 = as4, n2 = as5;
	const double step = (n2-n1)/nn;
	if(dur == 0)
	switch(state)
	{
		case 0:
		set_PWM(n2);
		//dur = 3;
		dur = 1;
		return ++state; break;
		case 1:
		set_PWM(n1);
		//dur = 5;
		dur = 2;
		return ++state; break;
		break;
		default:
		if(state-2<nn)
		{
			set_PWM(n1+(state-2)*step);
			return ++state;
		}
		else
		{
			set_PWM(0);
			return state;
		}
		break;
	}
	else
	dur--;

	return state;
}
uc mus(uc state, const double* notes, const uc* dur, uc length, uc loop, uc ingame)
{
	static uc duration = 0;
	if(duration == 0)
	{
		set_PWM(notes[state]);
		if(dur)
			duration = ingame?dur[state]/6:dur[state];
		else
			duration = q;
		if(state < length - 1)
		return ++state;
		else if(loop)
		return 0;
	}
	if(duration != 0)
	duration --;
	return state;
}

enum sndState{MUS,WIN,DIE,GOVR,JMP, NONE, MUS2};
uc sound(uc state)
{
	if(pause)
	{
		set_PWM(0);
		return state;
	}
	static const double titlenotes[] = {a5,g5,fs5,g5,g5,f5,e5,f5,f5,e5,ds5,e5,e5,d5,cs5,d5,a5,g5,fs5,g5,as5,a5,ds5,a5,c6,b5,as5,b5,a5,g5,f5,e5};
	static const double winnotes[] = {g5,d5,g5,a5,g5,d5,g5,c6,b5,a5,g5,d5,g5,a5,f5,c5,f5,as5,a5,f5};
	static const uc windur[] = {e,e,e,h,e,e,e,d,q,e,e,e,e,h,e,e,e,h,q,e};
	static const uc winlen = 20;

	static const double gonotes[] = {g5,g4,c5,e5,ds5,g4,b5,b4,a5,c5,e5,a5,g5,c5,d5,e5,a5,c5,f5,a5,gs5,c5,d5,f5,e5,g4,c5,e5,d5,a4,b4,d5};
	static const uc golen = 32;

	static const double dienotes[] = {ds5,d5,cs5,c5,d5,cs5,c5,b4,d5,c5,b4,as4,c5,b4,as4,a4,b4,as4,a4,gs4,fs4,f4,e4,0};
	static const uc diedur[] = {6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,60,6};
	static const uc dielen = 24;

	//static double intronotes[] = {g5,fs5,f5,e5,0,g4,g3};
	//static const double gamenotes[] = {e5,d5,c5,e5,0,c5,d5,e5,0,e5,d5,c5,e5,f5,g5,a5,0,c5,0,c5,a4,g4,c5,0,c5,0,c5,f4,g4,a4,b4,0,c6,0,c6,0,e5,d5,c5,e5,0,c5,d5,e5,0,e5,d5,c5,a5,g5,c6,e5,d5,0,c5,0,c5,d5,e5,c5,0,c5,0,c5,f5,e5,c5,d5,0,c6,g5,0,a5,b5,c6,b5,g5,0,f5,g5,f5,e5,f5,fs5,g5,0,a5,b5,c6,b5,g5,0,ds6,d6,0,c6,g5,0,g5,f5,e5,d5,0};
	//static const uc gamedur[]   = {q, e, e, q, h, e, e, q,1, q, q, q, q, q, q, q,q, q,1, q, q, q, e,1, e,q, q, q, q, q, q, q, q, q, q, q, e, e, e,h, e, e, q,1, q, q, q, q, q, q, e, e,q, q,1, q, q, q, e,1, e,q, q, q, q, q, q,e, q, h,q, q, q, q, d, e,q, q, e, e, q, q, q, q, q, q, q, q, d, e,q,  q, q,q, q, e,1, e, q, q, q,q};
	//static const uc gamelen = 103;
	
	static const double gamenotes2[] = {e5,b4,c5,d5,c5,b4,a4,0,a4,c5,e5,d5,c5,b4,0,b4,c5,d5,e5,c5,a4,0,a4,0,d5,f5,a5,g5,f5,e5,c5,e5,d5,c5,b4,0,b4,c5,d5,e5,c5,a4,0,a4,0,e5,c5,d5,b4,c5,a4,gs4,b4,e5,c5,d5,b4,c5,e5,a5,gs5,b5};
	static const uc gamedur2[]		 = { q, e, e ,q, e, e, q,1, e ,e, q, e, e, q,1, e, e, q, q, q, q,1, q,q, q, e, q, e, e, h, e, q, e, e, q,1, e, e, q, q, q, q,1, q,q, hd,hd,hd,hd,hd,hd,hd,hd,hd,hd,hd,hd,q, q, hd, hd, hd};
	static const uc gamelen2 = 62;
	
	static uc substate = 0, substate2 = 0, prevstate = 0, oldsubstate;
	if(prevstate == JMP)
	{
		substate = oldsubstate;
	}
	else if(musicstate != prevstate)
	{
		substate = 0;
	}
	switch(musicstate)
	{
		//case MUS2:
		//substate = mus(substate, gamenotes, gamedur, gamelen, 1, 1);
		//oldsubstate = substate;
		//break;
		case MUS:
		substate = mus(substate, gamenotes2, gamedur2, gamelen2, 1, 1);
		oldsubstate = substate;
		break;
		case DIE:
		substate = mus(substate, dienotes, diedur, dielen, 0, 0);
		break;
		case GOVR:
		substate = mus(substate, gonotes, 0, golen, 1, 0);
		break;
		case WIN:
		substate = mus(substate, winnotes, windur, winlen, 1, 0);
		break;
		case JMP:
		substate2 = jumpsnd(substate2);
		//if(substate2 == 27)
		if(substate2 == 13)
		{
			substate2 = 0;
			musicstate = MUS;
		}
		break;
		case NONE:
		set_PWM(0);
		break;
	}
	prevstate = musicstate;
	return state;
}