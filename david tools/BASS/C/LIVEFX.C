/* BASS Full-duplex test, copyright (c) 2002-2003 Ian Luck.
===========================================================
Other source: livefx.rc
Imports: bass.lib, kernel32.lib, user32.lib, comctl32.lib
*/

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <math.h>
#include "bass.h"

HWND win=NULL;
HINSTANCE inst;

#define MESS(id,m,w,l) SendDlgItemMessage(win,id,m,(WPARAM)w,(LPARAM)l)

HSTREAM chan;	// playback stream
HFX fx[4]={0};	// FX handles
int chunk;		// recording chunk size
int input;		// current input source
int latency=0;	// current latency

OPENFILENAME ofn;
char path[MAX_PATH];

void Error(char *es)
{
	char mes[200];
	sprintf(mes,"%s\n(error code: %d)",es,BASS_ErrorGetCode());
	MessageBox(win,mes,"Error",0);
}

// stream function - play the recording data
DWORD CALLBACK stream(HSTREAM handle, char *buffer, int length, DWORD user)
{
	int c;
	// check how much recorded data is buffered
	c=BASS_ChannelGetData(RECORDCHAN,0,BASS_DATA_AVAILABLE);
	c-=length;
	if (c>2*chunk+1764) { // buffer has gotten pretty large so remove some
		c-=chunk; // leave a single 'chunk'
		c&=~3;	// align to sample boundary
		BASS_ChannelGetData(RECORDCHAN,0,c); // remove it
	}
	// fetch recorded data into stream
	c=BASS_ChannelGetData(RECORDCHAN,buffer,length);
	if (c<length) memset(buffer+c,0,length-c); // short of data
	return length;
}


static BOOL Initialize()
{
	BASS_INFO i;
	// setup output - 10ms update period, get device latency
	if (!BASS_Init(-1,44100,MAKELONG(BASS_DEVICE_LATENCY,10),win)) {
		Error("Can't initialize device");
		return FALSE;
	}
	BASS_Start();

	i.size=sizeof(i);
	BASS_GetInfo(&i);
	// buffer size = update period + 'minbuf'
	BASS_SetBufferLength((10+i.minbuf)/1000.0f);

	if (i.dsver<8) { // no DX8, so disable effect buttons
		EnableWindow(GetDlgItem(win,20),FALSE);
		EnableWindow(GetDlgItem(win,21),FALSE);
		EnableWindow(GetDlgItem(win,22),FALSE);
		EnableWindow(GetDlgItem(win,23),FALSE);
	}

	// start recording - default device, 44100hz, stereo, 16 bits, no callback function
	if (!BASS_RecordInit(-1) || !BASS_RecordStart(44100,0,NULL,0)) {
		BASS_Free();
		Error("Can't initialize recording device");
		return FALSE;
	}
	// wait for recorded data to start arriving (calculate the latency)
	while (!(chunk=BASS_ChannelGetData(RECORDCHAN,0,BASS_DATA_AVAILABLE))) ;

	// create a stream to play the recording data, and start it
	chan=BASS_StreamCreate(44100,0,(STREAMPROC*)stream,0);
	BASS_StreamPlay(chan,0,BASS_SAMPLE_LOOP);

	{ // get list of inputs
		int c;
		char *i;
		for (c=0;i=BASS_RecordGetInputName(c);c++) {
			MESS(10,CB_ADDSTRING,0,i);
			if (!(BASS_RecordGetInput(c)&BASS_INPUT_OFF)) { // this 1 is currently "on"
				input=c;
				MESS(10,CB_SETCURSEL,input,0);
				MESS(11,TBM_SETPOS,TRUE,LOWORD(BASS_RecordGetInput(input))); // get level
			}
		}
	}

	return TRUE;
}

BOOL CALLBACK dialogproc(HWND h,UINT m,WPARAM w,LPARAM l)
{
	switch (m) {
		case WM_TIMER:
			{ // display current latency (averaged)
				char buf[20];
				latency=(latency*3+BASS_ChannelGetData(chan,0,BASS_DATA_AVAILABLE)
					+BASS_ChannelGetData(RECORDCHAN,0,BASS_DATA_AVAILABLE))/4;
				sprintf(buf,"%d",latency*1000/176400);
				MESS(15,WM_SETTEXT,0,buf);
			}
			return 1;
		case WM_COMMAND:
			switch (LOWORD(w)) {
				case IDCANCEL:
					DestroyWindow(h);
					return 1;
				case 10:
					if (HIWORD(w)==CBN_SELCHANGE) { // input selection changed
						int i;
						input=MESS(10,CB_GETCURSEL,0,0); // get the selection
						for (i=0;BASS_RecordSetInput(i,BASS_INPUT_OFF);i++) ; // 1st disable all inputs, then...
						BASS_RecordSetInput(input,BASS_INPUT_ON); // enable the selected input
						MESS(11,TBM_SETPOS,TRUE,LOWORD(BASS_RecordGetInput(input))); // get the level
					}
					return 1;
				case 20: // toggle chorus
					if (fx[0]) {
						BASS_ChannelRemoveFX(chan,fx[0]);
						fx[0]=0;
					} else
						fx[0]=BASS_ChannelSetFX(chan,BASS_FX_CHORUS);
					return 1;
				case 21: // toggle gargle
					if (fx[1]) {
						BASS_ChannelRemoveFX(chan,fx[1]);
						fx[1]=0;
					} else
						fx[1]=BASS_ChannelSetFX(chan,BASS_FX_GARGLE);
					return 1;
				case 22: // toggle reverb
					if (fx[2]) {
						BASS_ChannelRemoveFX(chan,fx[2]);
						fx[2]=0;
					} else
						fx[2]=BASS_ChannelSetFX(chan,BASS_FX_REVERB);
					return 1;
				case 23: // toggle flanger
					if (fx[3]) {
						BASS_ChannelRemoveFX(chan,fx[3]);
						fx[3]=0;
					} else
						fx[3]=BASS_ChannelSetFX(chan,BASS_FX_FLANGER);
					return 1;
			}
			break;
		case WM_HSCROLL:
			if (l) { // set input source level
				int level=SendMessage((HWND)l,TBM_GETPOS,0,0);
				BASS_RecordSetInput(input,BASS_INPUT_LEVEL|level);
			}
			return 1;
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
			MESS(11,TBM_SETRANGE,FALSE,MAKELONG(0,100)); // initialize input level slider
			MessageBox(win,
				"Do not set the input to 'WAVE' / 'What you hear' (etc...) with\n"
				"the level set high, as that is likely to result in nasty feedback.\n",
				"Feedback warning",MB_ICONWARNING);
			if (!Initialize()) {
				DestroyWindow(win);
				return 1;
			}
			SetTimer(h,1,250,NULL);
			return 1;

		case WM_DESTROY:
			KillTimer(h,1);
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

	{ // enable trackbar support (for the level control)
		INITCOMMONCONTROLSEX cc={sizeof(cc),ICC_BAR_CLASSES};
		InitCommonControlsEx(&cc);
	}

	DialogBox(inst,(char*)1000,0,&dialogproc);

	// release it all
	BASS_RecordFree();
	BASS_Free();

	return 0;
}
