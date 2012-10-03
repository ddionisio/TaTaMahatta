/* BASS 2 stereo channels on 4 speakers example, copyright (c) 2002 Ian Luck.
=============================================================================
Imports: bass.lib, kernel32.lib
*/

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "bass.h"

/* display error messages */
void Error(char *text) 
{
	printf("Error(%d): %s\n",BASS_ErrorGetCode(),text);
	BASS_Free();
	ExitProcess(0);
}

/* decoded data buffering stuff */
typedef struct {
	short *buf;
	DWORD writepos,readposl,readposr; // write/read pointers (in samples)
	DWORD freq;
	HSTREAM dstr,lstr,rstr;	// decode/left/right streams
} BUFSTUFF;

/* check that buffer has sufficient decoded data */
void CheckBuffer(BUFSTUFF *b, DWORD pos)
{
	if (pos>b->writepos) {
		int o=min(b->readposl,b->readposr);
		b->writepos-=o;
		memmove(b->buf,b->buf+o*2,b->writepos*4); // shift over used data
		b->readposl-=o;
		b->readposr-=o;
		pos-=o;
		// read upto requested position
		b->writepos+=BASS_ChannelGetData(b->dstr,b->buf+b->writepos*2,(pos-b->writepos)*4)/4;
	}
}

/* play buffered sample data (left) */
DWORD CALLBACK stream_left(HSTREAM handle, short *buffer, DWORD length, BUFSTUFF *b)
{
	DWORD l;
	length/=2; // bytes -> samples
	CheckBuffer(b,b->readposl+length);
	if (length>b->writepos-b->readposl) length=b->writepos-b->readposl;
	for (l=0;l<length;l++) *buffer++=b->buf[(b->readposl+l)*2];
	b->readposl+=length;
	return length*2;
}

/* play buffered sample data (right) */
DWORD CALLBACK stream_right(HSTREAM handle, short *buffer, DWORD length, BUFSTUFF *b)
{
	DWORD l;
	length/=2;
	CheckBuffer(b,b->readposr+length);
	if (length>b->writepos-b->readposr) length=b->writepos-b->readposr;
	for (l=0;l<length;l++) *buffer++=b->buf[(b->readposr+l)*2+1];
	b->readposr+=length;
	return length*2;
}

void main(int argc, char **argv)
{
	BUFSTUFF b,b2;
	BASS_3DVECTOR p={0,0,0};

	printf("BASS 2 stereo channels on 4 speakers example : MOD/MPx/OGG/WAV\n"
			"--------------------------------------------------------------\n"
			"       Set your soundcard's output to 4 or 5.1 speakers\n");

	/* check that BASS 1.6 was loaded */
	if (BASS_GetVersion()!=MAKELONG(1,6)) {
		printf("BASS version 1.6 was not loaded\n");
		return;
	}

	if (argc!=3) {
		printf("\tusage: 4speaker <file1> <file2>\n");
		return;
	}

	/* setup output - default device, 44100hz, stereo, 16 bits */
	if (!BASS_Init(-1,44100,BASS_DEVICE_3D,0))
		Error("Can't initialize device");

	/* try initializing the 1st (front) file */
	if (!(b.dstr=BASS_StreamCreateFile(FALSE,argv[1],0,0,BASS_STREAM_DECODE)))
		if (!(b.dstr=BASS_MusicLoad(FALSE,argv[1],0,0,BASS_MUSIC_DECODE|BASS_MUSIC_RAMPS)))
			Error("Can't play the 1st file");
	if (BASS_ChannelGetFlags(b.dstr)&BASS_SAMPLE_MONO)
		Error("The 1st stream is mono!");

	/* try initializing the 2nd (rear) file */
	if (!(b2.dstr=BASS_StreamCreateFile(FALSE,argv[2],0,0,BASS_STREAM_DECODE)))
		if (!(b2.dstr=BASS_MusicLoad(FALSE,argv[2],0,0,BASS_MUSIC_DECODE|BASS_MUSIC_RAMPS)))
			Error("Can't play the 2nd file");
	if (BASS_ChannelGetFlags(b2.dstr)&BASS_SAMPLE_MONO)
		Error("The 2nd stream is mono!");

	printf("front : %s\n",argv[1]);
	printf("rear  : %s\n",argv[2]);

	/* Get sample rates and allocate buffers */
	BASS_ChannelGetAttributes(b.dstr,&b.freq,0,0);
	b.buf=malloc(b.freq*4); // 1 sec buffer
	BASS_ChannelGetAttributes(b2.dstr,&b2.freq,0,0);
	b2.buf=malloc(b2.freq*4); // 1 sec buffer

	/* Create streams to play the 1st decoded data, and link them */
	b.lstr=BASS_StreamCreate(b.freq,BASS_SAMPLE_MONO|BASS_SAMPLE_3D,(STREAMPROC*)&stream_left,(DWORD)&b);
	b.rstr=BASS_StreamCreate(b.freq,BASS_SAMPLE_MONO|BASS_SAMPLE_3D,(STREAMPROC*)&stream_right,(DWORD)&b);
	BASS_ChannelSetLink(b.lstr,b.rstr);

	/* Create streams to play the 2nd decoded data, and link them */
	b2.lstr=BASS_StreamCreate(b2.freq,BASS_SAMPLE_MONO|BASS_SAMPLE_3D,(STREAMPROC*)&stream_left,(DWORD)&b2);
	b2.rstr=BASS_StreamCreate(b2.freq,BASS_SAMPLE_MONO|BASS_SAMPLE_3D,(STREAMPROC*)&stream_right,(DWORD)&b2);
	BASS_ChannelSetLink(b2.lstr,b2.rstr);

	/* position the streams */
	p.z=3; // front
	p.x=-1.5; // left
	BASS_ChannelSet3DPosition(b.lstr,&p,0,0);
	p.x=1.5; // right
	BASS_ChannelSet3DPosition(b.rstr,&p,0,0);
	p.z=-3; // rear
	p.x=-1.5; // left
	BASS_ChannelSet3DPosition(b2.lstr,&p,0,0);
	p.x=1.5; // right
	BASS_ChannelSet3DPosition(b2.rstr,&p,0,0);
	BASS_Apply3D();

	BASS_Start();
	/* start it! */
	b.writepos=b.readposl=b.readposr=0;
	BASS_StreamPlay(b.lstr,FALSE,0); // start front
	b2.writepos=b2.readposl=b2.readposr=0;
	BASS_StreamPlay(b2.lstr,FALSE,0); // start rear

	while (!_kbhit() && (BASS_ChannelIsActive(b.lstr) || BASS_ChannelIsActive(b2.lstr))) {
		/* display some stuff and wait a bit */
		printf("pos %09I64d %09I64d - cpu %.1f%%  \r",
			BASS_ChannelGetPosition(b.lstr)*2,BASS_ChannelGetPosition(b2.lstr)*2,BASS_GetCPU());
		Sleep(50);
	}

	BASS_Free();
	free(b.buf);
}
