/* BASS Simple Console Test, copyright (c) 1999-2003 Ian Luck.
==============================================================
Imports: bass.lib, kernel32.lib, winmm.lib
*/

#include <windows.h>
#include <mmsystem.h>
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

DWORD starttime;

/* looping synchronizer, resets the clock */
void CALLBACK LoopSync(HSYNC handle, DWORD channel, DWORD data, DWORD user)
{
	starttime=timeGetTime();
}

void main(int argc, char **argv)
{
	HMUSIC mod;
	HSTREAM str;
	DWORD act,time,level;
	QWORD pos;
	int a;
	printf("Simple console mode BASS example : MOD/MPx/OGG/WAV player\n"
			"---------------------------------------------------------\n");

	/* check that BASS 1.8 was loaded */
	if (BASS_GetVersion()!=MAKELONG(1,8)) {
		printf("BASS version 1.8 was not loaded\n");
		return;
	}

	if (argc!=2) {
		printf("\tusage: contest <file>\n");
		return;
	}

	/* setup output - default device, 44100hz, stereo, 16 bits */
	if (!BASS_Init(-1,44100,0,0))
		Error("Can't initialize device");

	/* try streaming the file/url */
	if ((str=BASS_StreamCreateFile(FALSE,argv[1],0,0,0))
		|| (str=BASS_StreamCreateURL(argv[1],0,0,0))) {
		/* set a synchronizer for when the stream reaches the end */
		BASS_ChannelSetSync(str,BASS_SYNC_END,0,&LoopSync,0);
		pos=BASS_StreamGetLength(str);
		if (BASS_StreamGetFilePosition(str,BASS_FILEPOS_DOWNLOAD)!=-1) {
			/* streaming from the internet */
			if (pos)
				printf("streaming internet file [%I64d bytes]",pos);
			else
				printf("streaming internet file");
		} else
			printf("streaming file [%I64d bytes]",pos);
	} else {
		/* try loading the MOD (with looping, sensitive ramping, and calculate the duration) */
		if (!(mod=BASS_MusicLoad(FALSE,argv[1],0,0,BASS_MUSIC_LOOP|BASS_MUSIC_RAMPS|BASS_MUSIC_CALCLEN)))
			/* not a MOD either */
			Error("Can't play the file");
		/* set a synchronizer for when the MOD reaches the end */
		BASS_ChannelSetSync(mod,BASS_SYNC_END,0,&LoopSync,0);
		/* count channels */
		for (a=0;BASS_MusicGetChannelVol(mod,a)!=-1;a++);
		printf("playing MOD music \"%s\" [%d chans, %d orders]",BASS_MusicGetName(mod),a,BASS_MusicGetLength(mod,FALSE));
		pos=BASS_MusicGetLength(mod,TRUE);
	}

	/* display the time length */
	if (pos) {
		time=(DWORD)BASS_ChannelBytes2Seconds(str?str:mod,pos);
		printf(" %d:%02d\n",time/60,time%60);
	} else /* no time length available */
		printf("\n");

	BASS_Start();
	if (str)
		BASS_StreamPlay(str,0,BASS_SAMPLE_LOOP);
	else
		BASS_MusicPlay(mod);
	starttime=timeGetTime();

	/* NOTE: some compilers don't support _kbhit */
	while (!_kbhit() && (act=BASS_ChannelIsActive(str?str:mod))) {
		/* display some stuff and wait a bit */
		time=timeGetTime()-starttime;
		level=BASS_ChannelGetLevel(str?str:mod);
		pos=BASS_ChannelGetPosition(str?str:mod);
		if (str)
			printf("pos %09I64d",pos);
		else
			printf("pos %03d:%03d",LOWORD(pos),HIWORD(pos));
		printf(" - time %d:%02d - L ",time/60000,(time/1000)%60);
		if (act==BASS_ACTIVE_STALLED) {
			/* internet file stream playback has stalled */
			printf("-- buffering : %05d --",
				BASS_StreamGetFilePosition(str,BASS_FILEPOS_DOWNLOAD)-BASS_StreamGetFilePosition(str,BASS_FILEPOS_DECODE));
		} else {
			for (a=93;a;a=a*2/3) putchar(LOWORD(level)>=a?'*':'-');
			putchar(' ');
			for (a=1;a<128;a+=a-(a>>1)) putchar(HIWORD(level)>=a?'*':'-');
		}
		printf(" R - cpu %.2f%%  \r",BASS_GetCPU());
		Sleep(50);
	}
	printf("                                                                         \r");

	/* wind the frequency down... */
	BASS_ChannelSlideAttributes(str?str:mod,1000,-1,-101,500);
	Sleep(300);
	/* ...and fade-out to avoid a "click" */
	BASS_ChannelSlideAttributes(str?str:mod,-1,-2,-101,200);
	while (BASS_ChannelIsSliding(str?str:mod)) Sleep(1);

	BASS_Free();
}
