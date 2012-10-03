/* BASS simple DSP test, copyright (c) 2000-2003 Ian Luck.
==========================================================
Other source: dsptest.rc
Imports: bass.lib, kernel32.lib, user32.lib, comdlg32.lib
*/

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "bass.h"

HWND win=NULL;
HINSTANCE inst;

DWORD floatable; // floating-point channel support?
DWORD chan;	// the channel... HMUSIC or HSTREAM

OPENFILENAME ofn;
char path[MAX_PATH];

/* display error messages */
void Error(char *es)
{
	char mes[200];
	sprintf(mes,"%s\n(error code: %d)",es,BASS_ErrorGetCode());
	MessageBox(win,mes,"Error",0);
}


/* "rotate" */
HDSP rotdsp=0;	// DSP handle
float rotpos;	// cur.pos
void CALLBACK Rotate(HDSP handle, DWORD channel, void *buffer, DWORD length, DWORD user)
{
	float *d=(float*)buffer;
	DWORD a;

	for (a=0;a<length/4;a+=2) {
		d[a]*=fabs(sin(rotpos));
		d[a+1]*=fabs(cos(rotpos));
		rotpos=fmod(rotpos+0.00003,3.1415927);
	}
}

/* "echo" */
HDSP echdsp=0;	// DSP handle
#define ECHBUFLEN 1200	// buffer length
float echbuf[ECHBUFLEN][2];	// buffer
int echpos;	// cur.pos
void CALLBACK Echo(HDSP handle, DWORD channel, void *buffer, DWORD length, DWORD user)
{
	float *d=(float*)buffer;
	DWORD a;

	for (a=0;a<length/4;a+=2) {
		float l=d[a]+(echbuf[echpos][1]/2);
		float r=d[a+1]+(echbuf[echpos][0]/2);
#if 1 // 0=echo, 1=basic "bathroom" reverb
		echbuf[echpos][0]=d[a]=l;
		echbuf[echpos][1]=d[a+1]=r;
#else
		echbuf[echpos][0]=d[a];
		echbuf[echpos][1]=d[a+1];
		d[a]=l;
		d[a+1]=r;
#endif
		echpos++;
		if (echpos==ECHBUFLEN) echpos=0;
	}
}

/* "flanger" */
HDSP fladsp=0;	// DSP handle
#define FLABUFLEN 350	// buffer length
float flabuf[FLABUFLEN][2];	// buffer
int flapos;	// cur.pos
float flas,flasinc;	// sweep pos/increment
void CALLBACK Flange(HDSP handle, DWORD channel, void *buffer, DWORD length, DWORD user)
{
	float *d=(float*)buffer;
	DWORD a;

	for (a=0;a<length/4;a+=2) {
		int p1=(flapos+(int)flas)%FLABUFLEN;
		int p2=(p1+1)%FLABUFLEN;
		float f=fmod(flas,1);
		float s;

		s=d[a]+((flabuf[p1][0]*(1-f))+(flabuf[p2][0]*f));
		flabuf[flapos][0]=d[a];
		d[a]=s;

		s=d[a+1]+((flabuf[p1][1]*(1-f))+(flabuf[p2][1]*f));
		flabuf[flapos][1]=d[a+1];
		d[a+1]=s;

		flapos++;
		if (flapos==FLABUFLEN) flapos=0;
		flas+=flasinc;
		if (flas<0.0 || flas>FLABUFLEN)
			flasinc=-flasinc;
	}
}


#define MESS(id,m,w,l) SendDlgItemMessage(win,id,m,(WPARAM)w,(LPARAM)l)

BOOL CALLBACK dialogproc(HWND h,UINT m,WPARAM w,LPARAM l)
{
	switch (m) {
		case WM_COMMAND:
			switch (LOWORD(w)) {
				case IDCANCEL:
					DestroyWindow(h);
					return 1;
				case 10:
					{
						char file[MAX_PATH]="";
						ofn.lpstrFilter="playable files\0*.mo3;*.xm;*.mod;*.s3m;*.it;*.mtm;*.mp3;*.mp2;*.mp1;*.ogg;*.wav\0All files\0*.*\0\0";
						ofn.lpstrFile=file;
						if (GetOpenFileName(&ofn)) {
							memcpy(path,file,ofn.nFileOffset);
							path[ofn.nFileOffset-1]=0;
							// free both MOD and stream, it must be one of them! :)
							BASS_MusicFree(chan);
							BASS_StreamFree(chan);
							if (!(chan=BASS_StreamCreateFile(FALSE,file,0,0,floatable?BASS_SAMPLE_FLOAT:0))
								&& !(chan=BASS_MusicLoad(FALSE,file,0,0,BASS_MUSIC_LOOP|BASS_MUSIC_RAMP|(floatable?BASS_MUSIC_FLOAT:0)))) {
								// whatever it is, it ain't playable
								MESS(10,WM_SETTEXT,0,"click here to open a file...");
								Error("Can't play the file");
								break;
							}
							if (BASS_ChannelGetFlags(chan)&BASS_SAMPLE_MONO) {
								// mono = not allowed
								MESS(10,WM_SETTEXT,0,"click here to open a file...");
								BASS_MusicFree(chan);
								BASS_StreamFree(chan);
								Error("mono sources are not supported");
								break;
							}
							MESS(10,WM_SETTEXT,0,file);
							// setup DSPs on new channel
							SendMessage(win,WM_COMMAND,11,0);
							SendMessage(win,WM_COMMAND,12,0);
							SendMessage(win,WM_COMMAND,13,0);
							// play both MOD and stream, it must be one of them!
							BASS_MusicPlay(chan);
							BASS_StreamPlay(chan,0,BASS_SAMPLE_LOOP);
						}
					}
					return 1;
				case 11: // toggle "rotate"
					if (MESS(11,BM_GETCHECK,0,0)) {
						rotpos=0.7853981f;
						rotdsp=BASS_ChannelSetDSP(chan,&Rotate,0);
					} else
						BASS_ChannelRemoveDSP(chan,rotdsp);
					break;
				case 12: // toggle "echo"
					if (MESS(12,BM_GETCHECK,0,0)) {
						memset(echbuf,0,sizeof(echbuf));
						echpos=0;
						echdsp=BASS_ChannelSetDSP(chan,&Echo,0);
					} else
						BASS_ChannelRemoveDSP(chan,echdsp);
					break;
				case 13: // toggle "flanger"
					if (MESS(13,BM_GETCHECK,0,0)) {
						memset(flabuf,0,sizeof(flabuf));
						flapos=0;
					    flas=FLABUFLEN/2;
					    flasinc=0.002f;
						fladsp=BASS_ChannelSetDSP(chan,&Flange,0);
					} else
						BASS_ChannelRemoveDSP(chan,fladsp);
					break;
			}
			break;

		case WM_INITDIALOG:
			win=h;
			GetCurrentDirectory(MAX_PATH,path);
			memset(&ofn,0,sizeof(ofn));
			ofn.lStructSize=sizeof(ofn);
			ofn.hwndOwner=h;
			ofn.hInstance=inst;
			ofn.nMaxFile=MAX_PATH;
			ofn.lpstrInitialDir=path;
			ofn.Flags=OFN_HIDEREADONLY|OFN_EXPLORER;
			// initialize - default device, 44100hz, stereo, 16 bits, floating-point DSP
			if (!BASS_Init(-1,44100,BASS_DEVICE_FLOATDSP,win)) {
				Error("Can't initialize device");
				DestroyWindow(win);
				return 1;
			}
			BASS_Start();
			// check for floating-point capability
			floatable=BASS_StreamCreate(44100,BASS_SAMPLE_FLOAT,NULL,0);
			if (floatable) BASS_StreamFree(floatable); // woohoo!
			return 1;
	}
	return 0;
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	inst=hInstance;

	// Check that BASS 1.8 was loaded
	if (BASS_GetVersion()!=MAKELONG(1,8)) {
		MessageBox(0,"BASS version 1.8 was not loaded","Incorrect BASS.DLL",0);
		return 0;
	}

	DialogBox(inst,(char*)1000,0,&dialogproc);

	BASS_Free();

	return 0;
}
