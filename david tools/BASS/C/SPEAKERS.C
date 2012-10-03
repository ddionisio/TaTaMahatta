/* BASS multi-speaker example, copyright (c) 2003 Ian Luck.
===========================================================
Other source: speakers.rc
Imports: bass.lib, kernel32.lib, user32.lib, comdlg32.lib, comctl32.lib
*/

#include <windows.h>
#include <stdio.h>
#include "bass.h"

HWND win=NULL;
HINSTANCE inst;

DWORD flags[4]={BASS_SPEAKER_FRONT,BASS_SPEAKER_REAR,BASS_SPEAKER_CENLFE,BASS_SPEAKER_REAR2};
HSTREAM chan[4];

/* display error messages */
void Error(char *es)
{
	char mes[200];
	sprintf(mes,"%s\n(error code: %d)",es,BASS_ErrorGetCode());
	MessageBox(win,mes,"Error",0);
}

#define MESS(id,m,w,l) SendDlgItemMessage(win,id,m,(WPARAM)w,(LPARAM)l)

BOOL CALLBACK dialogproc(HWND h,UINT m,WPARAM w,LPARAM l)
{
	static OPENFILENAME ofn;
	static char path[MAX_PATH];

	switch (m) {
		case WM_COMMAND:
			switch (LOWORD(w)) {
				case IDCANCEL:
					DestroyWindow(h);
					return 1;
				case 10: // open a file to play on #1
				case 11: // open a file to play on #2
				case 12: // open a file to play on #3
				case 13: // open a file to play on #4
					{
						int speaker=LOWORD(w)-10;
						char file[MAX_PATH]="";
						ofn.lpstrFile=file;
						if (GetOpenFileName(&ofn)) {
							memcpy(path,file,ofn.nFileOffset);
							path[ofn.nFileOffset-1]=0;
							BASS_StreamFree(chan[speaker]); // free old stream before opening new
							if (!(chan[speaker]=BASS_StreamCreateFile(FALSE,file,0,0,flags[speaker]))) {
								MESS(10+speaker,WM_SETTEXT,0,"click here to open a file...");
								Error("Can't play the file");
								break;
							}
							MESS(10+speaker,WM_SETTEXT,0,file);
							BASS_StreamPlay(chan[speaker],0,BASS_SAMPLE_LOOP);
						}
					}
					return 1;
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
			ofn.lpstrFilter="streamable files\0*.mp3;*.mp2;*.mp1;*.ogg;*.wav\0All files\0*.*\0\0";
			// initialize BASS - default device
			if (!BASS_Init(-1,44100,0,win)) {
				Error("Can't initialize device");
				DestroyWindow(win);
				return 1;
			}
			{ // check how many speakers the device supports
				BASS_INFO i;
				i.size=sizeof(i);
				BASS_GetInfo(&i);
				if (i.speakers<4) { // no extra speakers detected, enable them anyway?
					if (MessageBox(0,"Do you wish to enable \"speaker assignment\" anyway?","No extra speakers detected",MB_ICONQUESTION|MB_YESNO)==IDYES) {
						// reinitialize BASS - forcing speaker assignment
						BASS_Free();
						if (!BASS_Init(-1,44100,BASS_DEVICE_SPEAKERS,win)) {
							Error("Can't initialize device");
							DestroyWindow(win);
							return 1;
						}
						BASS_GetInfo(&i); // get info again
					}
				}
				if (i.speakers<8) EnableWindow(GetDlgItem(h,13),FALSE);
				if (i.speakers<6) EnableWindow(GetDlgItem(h,12),FALSE);
				if (i.speakers<4) {
					EnableWindow(GetDlgItem(h,11),FALSE);
					// no multi-speaker support, so remove speaker flag for normal stereo output
					flags[0]=0;
				}
			}
			BASS_Start();
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

	/* main dialog */
	DialogBox(inst,(char*)1000,0,&dialogproc);

	BASS_Free();

	return 0;
}
