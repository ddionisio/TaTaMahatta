/* BASS Simple Console WAVE Writer, copyright (c) 2002-2003 Ian Luck.
=====================================================================
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

void main(int argc, char **argv)
{
	DWORD chan,flags,p;
	QWORD pos;
	BOOL str=0;
	FILE *fp;
	char buf[20000];
	WAVEFORMATEX wf;

	printf("BASS WAVE writer example : MOD/MPx/OGG -> BASS.WAV\n"
			"--------------------------------------------------\n");

	/* check that BASS 1.8 was loaded */
	if (BASS_GetVersion()!=MAKELONG(1,8)) {
		printf("BASS version 1.8 was not loaded\n");
		return;
	}

	if (argc!=2) {
		printf("\tusage: writewav <file>\n");
		return;
	}

	/* setup output - "no sound" device, 44100hz, stereo, 16 bits */
	if (!BASS_Init(-2,44100,BASS_DEVICE_NOTHREAD,0))
		Error("Can't initialize device");

	/* try streaming the file/url */
	if ((chan=BASS_StreamCreateFile(FALSE,argv[1],0,0,BASS_STREAM_DECODE))
		|| (chan=BASS_StreamCreateURL(argv[1],0,BASS_STREAM_DECODE,0))) {
		pos=BASS_StreamGetLength(chan);
		printf("streaming file [%I64d bytes]",pos);
		str=TRUE;
	} else {
		/* try loading the MOD (with sensitive ramping, and calculate the duration) */
		if (!(chan=BASS_MusicLoad(FALSE,argv[1],0,0,BASS_MUSIC_DECODE|BASS_MUSIC_RAMP|BASS_MUSIC_CALCLEN)))
			/* not a MOD either */
			Error("Can't play the file");
		printf("playing MOD music \"%s\" [%d orders]",BASS_MusicGetName(chan),BASS_MusicGetLength(chan,FALSE));
		pos=BASS_MusicGetLength(chan,TRUE);
	}

	/* display the time length */
	if (pos) {
		p=(DWORD)BASS_ChannelBytes2Seconds(chan,pos);
		printf(" %d:%02d\n",p/60,p%60);
	} else /* no time length available */
		printf("\n");

	if (!(fp=fopen("BASS.WAV","wb"))) Error("Can't create file");
	printf("writing to BASS.WAV file... press a key to stop\n");

	/* write WAV header */
	flags=BASS_ChannelGetFlags(chan);
	wf.wFormatTag=1;
	wf.nChannels=flags&BASS_SAMPLE_MONO?1:2;
	wf.wBitsPerSample=flags&BASS_SAMPLE_8BITS?8:16;
	wf.nBlockAlign=wf.nChannels*wf.wBitsPerSample/8;
	BASS_ChannelGetAttributes(chan,&wf.nSamplesPerSec,NULL,NULL);
	wf.nAvgBytesPerSec=wf.nSamplesPerSec*wf.nBlockAlign;
	fwrite("RIFF\0\0\0\0WAVEfmt \20\0\0\0",20,1,fp);
	fwrite(&wf,16,1,fp);
	fwrite("data\0\0\0\0",8,1,fp);

	/* NOTE: some compilers don't support _kbhit */
	while (!_kbhit() && BASS_ChannelIsActive(chan)) {
		fwrite(buf,1,BASS_ChannelGetData(chan,buf,20000),fp);
		pos=BASS_ChannelGetPosition(chan);
		if (str)
			printf("pos %09I64d\r",pos);
		else
			printf("pos %03d:%03d\r",LOWORD(pos),HIWORD(pos));
		Sleep(1); // don't hog the CPU too much :)
	}

	/* complete WAV header */
	fflush(fp);
	p=ftell(fp);
	fseek(fp,4,SEEK_SET);
	_putw(p-8,fp);
	fflush(fp);
	fseek(fp,40,SEEK_SET);
	_putw(p-44,fp);
	fflush(fp);
	fclose(fp);

	BASS_Free();
}
