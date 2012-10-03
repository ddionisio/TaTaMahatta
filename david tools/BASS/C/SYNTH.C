/* BASS Simple Synth, copyright (c) 2001-2003 Ian Luck.
=======================================================
Imports: bass.lib, kernel32.lib
*/

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "bass.h"

/* display error messages */
void Error(char *text) 
{
	printf("Error(%d): %s\n",BASS_ErrorGetCode(),text);
	BASS_Free();
	ExitProcess(0);
}


#define PI 3.14159265358979323846
#define TABLESIZE 2048
int sinetable[TABLESIZE];	// sine table
#define KEYS 20
WORD keys[KEYS]={
	'Q','2','W','3','E','R','5','T','6','Y','7','U',
	'I','9','O','0','P',219,187,221
};
#define MAXVOL	4000	// higher value = longer fadeout
int vol[KEYS]={0},pos[KEYS];	// keys' volume & pos


/* stream writer */
DWORD CALLBACK WriteStream(HSTREAM handle, short *buffer, DWORD length, DWORD user)
{
	int n,s;
	DWORD c;
	float f;
	memset(buffer,0,length);
	for (n=0;n<KEYS;n++) {
		if (!vol[n]) continue;
		f=pow(2.0,(n+3)/12.0)*TABLESIZE*440.0/44100.0;
		for (c=0;c<length/4 && vol[n];c++) {
			s=sinetable[(int)((pos[n]+c)*f)&(TABLESIZE-1)]*vol[n]/MAXVOL;
			s+=(int)buffer[c*2];
			if (s>32767) s=32767;
			else if (s<-32768) s=-32768;
			buffer[c*2+1]=buffer[c*2]=s; // left and right channels are the same
			if (vol[n]<MAXVOL) vol[n]--;
		}
		pos[n]+=c; // update key's sine pos
	}
	return length;
}

void main(int argc, char **argv)
{
	BASS_INFO info;
	HSTREAM str;
	char *fxname[9]={"CHORUS","COMPRESSOR","DISTORTION","ECHO",
		"FLANGER","GARGLE","I3DL2REVERB","PARAMEQ","REVERB"};
	HFX fx[9]={0}; // effect handles
	INPUT_RECORD keyin;
	int r;
	float buflen;

	printf("BASS Simple Sinewave Synth\n"
			"--------------------------\n");

	/* check that BASS 1.8 was loaded */
	if (BASS_GetVersion()!=MAKELONG(1,8)) {
		printf("BASS version 1.8 was not loaded\n");
		return;
	}

	/* setup output - 10ms update period, get latency */
	if (!BASS_Init(-1,44100,MAKELONG(BASS_DEVICE_LATENCY,10),0))
		Error("Can't initialize device");
	BASS_Start();

	/* build sine table */
	for (r=0;r<TABLESIZE;r++)
		sinetable[r]=(int)(sin(2.0*PI*(double)r/TABLESIZE)*7000.0);

	info.size=sizeof(info);
	BASS_GetInfo(&info);
	printf("device latency: %dms\n",info.latency);
	printf("device minbuf: %dms\n",info.minbuf);
	printf("ds version: %d (effects %s)\n",info.dsver,info.dsver<8?"disabled":"enabled");

	/* default buffer size = update period + 'minbuf' */
	buflen=BASS_SetBufferLength((10+info.minbuf)/1000.0f);

	/* create a stream, stereo so that effects sound nice */
	str=BASS_StreamCreate(44100,0,(STREAMPROC*)&WriteStream,0);
	printf("press these keys to play:\n\n"
			"  2 3  5 6 7  9 0  =\n"
			" Q W ER T Y UI O P[ ]\n\n"
			"press -/+ to de/increase the buffer\n"
			"press spacebar to quit\n\n");

	if (info.dsver>=8) // DX8 effects available
		printf("press F1-F9 to toggle effects\n\n");

	printf("using a %.1fms buffer\r",buflen*1000.0);

	BASS_StreamPlay(str,0,0);

	while (ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE),&keyin,1,&r)) {
		int key;
		if (keyin.EventType!=KEY_EVENT) continue;
		if (keyin.Event.KeyEvent.wVirtualKeyCode==VK_SPACE) break;
		if (keyin.Event.KeyEvent.bKeyDown && keyin.Event.KeyEvent.wVirtualKeyCode==VK_SUBTRACT) {
			/* recreate stream with smaller buffer */
			BASS_StreamFree(str);
			buflen=BASS_SetBufferLength(buflen-.001f);
			printf("using a %.1fms buffer\t\t\r",buflen*1000.0);
			str=BASS_StreamCreate(44100,0,WriteStream,0);
			/* set effects on the new stream */
			for (r=0;r<9;r++) if (fx[r]) fx[r]=BASS_ChannelSetFX(str,BASS_FX_CHORUS+r);
			BASS_StreamPlay(str,0,0);
		}
		if (keyin.Event.KeyEvent.bKeyDown && keyin.Event.KeyEvent.wVirtualKeyCode==VK_ADD) {
			/* recreate stream with larger buffer */
			BASS_StreamFree(str);
			buflen=BASS_SetBufferLength(buflen+.001f);
			printf("using a %.1fms buffer    \r",buflen*1000.0);
			str=BASS_StreamCreate(44100,0,WriteStream,0);
			/* set effects on the new stream */
			for (r=0;r<9;r++) if (fx[r]) fx[r]=BASS_ChannelSetFX(str,BASS_FX_CHORUS+r);
			BASS_StreamPlay(str,0,0);
		}
		if (keyin.Event.KeyEvent.bKeyDown && keyin.Event.KeyEvent.wVirtualKeyCode>=VK_F1
			&& keyin.Event.KeyEvent.wVirtualKeyCode<=VK_F9) {
			r=keyin.Event.KeyEvent.wVirtualKeyCode-VK_F1;
			if (fx[r]) {
				BASS_ChannelRemoveFX(str,fx[r]);
				fx[r]=0;
				printf("effect %s = OFF\t\t\r",fxname[r]);
			} else {
				/* set the effect, not bothering with parameters (use defaults) */
				if (fx[r]=BASS_ChannelSetFX(str,BASS_FX_CHORUS+r))
					printf("effect %s = ON\t\t\r",fxname[r]);
			}
		}
		for (key=0;key<KEYS;key++)
			if (keyin.Event.KeyEvent.wVirtualKeyCode==keys[key]) break;
		if (key==KEYS) continue;
		if (keyin.Event.KeyEvent.bKeyDown && vol[key]!=MAXVOL) {
			pos[key]=0;
			vol[key]=MAXVOL; // start key
		} else if (!keyin.Event.KeyEvent.bKeyDown && vol[key])
			vol[key]--; // trigger key fadeout
	}

	BASS_Free();
}
