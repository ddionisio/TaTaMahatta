/* BASS Multiple output example, copyright (c) 2001-2003 Ian Luck.
==================================================================
Demonstrates how you can use LoadLibrary and GetProcAddress to import
multiple copies of BASS for multiple output devices.
Other source: multi.rc
Imports: kernel32.lib, user32.lib comdlg32.lib
*/

#include <windows.h>
#include <stdio.h>

#define devices 2
#define BASSDEF(f) (WINAPI *f[devices])	// define the functions as pointers
#include "bass.h"

char bassfile[devices-1][MAX_PATH];	// temporary BASS.DLL files
HINSTANCE bass[devices]={0};		// bass handles

/* load BASS[n] and the required functions */
void LoadBASS(int n)
{
	char tempfile[MAX_PATH];

	if (n) {
		char dllfile[MAX_PATH],temppath[MAX_PATH],*c;
		if (!SearchPath(NULL,"bass.dll",NULL,MAX_PATH,dllfile,&c)) {
			MessageBox(0,"Error: Can't find BASS.DLL\n","",0);
			ExitProcess(0);
		}

		/* get a temporary filename and copy BASS.DLL there */
		GetTempPath(MAX_PATH,temppath);
		GetTempFileName(temppath,"bas",0,tempfile);
		CopyFile(dllfile,tempfile,0);
		strcpy(bassfile[n-1],tempfile);
	} else
		strcpy(tempfile,"bass.dll");

	/* load the BASS.DLL library */
	if (!(bass[n]=LoadLibrary(tempfile))) {
		MessageBox(0,"Error: Can't load BASS.DLL\n","",0);
		ExitProcess(0);
	}
	/* load all the BASS functions that are to be used */
#define LOADBASSFUNCTION(f) ((void**)f)[n]=GetProcAddress(bass[n],#f)
	LOADBASSFUNCTION(BASS_GetVersion);
	LOADBASSFUNCTION(BASS_ErrorGetCode);
	LOADBASSFUNCTION(BASS_GetDeviceDescription);
	LOADBASSFUNCTION(BASS_Init);
	LOADBASSFUNCTION(BASS_Free);
	LOADBASSFUNCTION(BASS_Start);
	LOADBASSFUNCTION(BASS_StreamCreateFile);
	LOADBASSFUNCTION(BASS_StreamPlay);
	LOADBASSFUNCTION(BASS_StreamFree);
}

/* free the BASS library from memory and delete the temporary files */
void FreeBASS()
{
	int c;
	for (c=0;c<devices;c++) {
		if (bass[c]) {
			FreeLibrary(bass[c]);
			bass[c]=0;
			if (c) DeleteFile(bassfile[c-1]);
		}
	}
}


HWND win=NULL;
HINSTANCE inst;

int outdev[2];		// output devices
HSTREAM chan[2];	// the streams

/* display error messages */
void Error(int n,char *es)
{
	char mes[200];
	sprintf(mes,"%s\n(error %d code: %d)",es,n,BASS_ErrorGetCode[n]());
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
				case 10: // open a file to play on device #1
					{
						char file[MAX_PATH]="";
						ofn.lpstrFilter="streamable files\0*.mp3;*.mp2;*.mp1;*.ogg;*.wav\0All files\0*.*\0\0";
						ofn.lpstrFile=file;
						if (GetOpenFileName(&ofn)) {
							memcpy(path,file,ofn.nFileOffset);
							path[ofn.nFileOffset-1]=0;
							BASS_StreamFree[0](chan[0]);
							if (!(chan[0]=BASS_StreamCreateFile[0](FALSE,file,0,0,0))) {
								MESS(10,WM_SETTEXT,0,"click here to open a file...");
								Error(0,"Can't play the file");
								break;
							}
							MESS(10,WM_SETTEXT,0,file);
							BASS_StreamPlay[0](chan[0],0,BASS_SAMPLE_LOOP);
						}
					}
					return 1;
				case 11: // open a file to play on device #2
					{
						char file[MAX_PATH]="";
						ofn.lpstrFilter="streamable files\0*.mp3;*.mp2;*.mp1;*.ogg;*.wav\0All files\0*.*\0\0";
						ofn.lpstrFile=file;
						if (GetOpenFileName(&ofn)) {
							memcpy(path,file,ofn.nFileOffset);
							path[ofn.nFileOffset-1]=0;
							BASS_StreamFree[1](chan[1]);
							if (!(chan[1]=BASS_StreamCreateFile[1](FALSE,file,0,0,0))) {
								MESS(11,WM_SETTEXT,0,"click here to open a file...");
								Error(1,"Can't play the file");
								break;
							}
							MESS(11,WM_SETTEXT,0,file);
							BASS_StreamPlay[1](chan[1],0,BASS_SAMPLE_LOOP);
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
			// setup output devices
			if (!BASS_Init[0](outdev[0],44100,0,win)) {
				Error(0,"Can't initialize device");
				DestroyWindow(win);
			}
			if (!BASS_Init[1](outdev[1],44100,0,win)) {
				Error(1,"Can't initialize device");
				DestroyWindow(win);
			}
			BASS_Start[0]();
			BASS_Start[1]();
			return 1;
	}
	return 0;
}


// Simple device selector dialog stuff begins here
int device;		// selected device
BOOL CALLBACK devicedialogproc(HWND h,UINT m,WPARAM w,LPARAM l)
{
	switch (m) {
		case WM_COMMAND:
			switch (LOWORD(w)) {
				case 10:
					if (HIWORD(w)==LBN_SELCHANGE)
						device=SendMessage((HWND)l,LB_GETCURSEL,0,0);
					else if (HIWORD(w)==LBN_DBLCLK)
						EndDialog(h,0);
					break;
				case IDOK:
					EndDialog(h,0);
					return 1;
			}
			break;

		case WM_INITDIALOG:
			{
				char buf[30],*d;
				int c;
				sprintf(buf,"Select output device #%d",device+1);
				SetWindowText(h,buf);
				for (c=0;d=BASS_GetDeviceDescription[0](c);c++)
					SendDlgItemMessage(h,10,LB_ADDSTRING,0,(int)d);
				SendDlgItemMessage(h,10,LB_SETCURSEL,0,0);
			}
			device=0;
			return 1;
	}
	return 0;
}
// Device selector stuff ends here

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	inst=hInstance;

	/* Load BASS */
	LoadBASS(0);
	LoadBASS(1);
	/* Check that BASS 1.8 was loaded */
	if (BASS_GetVersion[0]()!=MAKELONG(1,8) || BASS_GetVersion[1]()!=MAKELONG(1,8)) {
		MessageBox(0,"BASS version 1.8 was not loaded","Incorrect BASS.DLL",0);
		FreeBASS();
		return 0;
	}

	/* Let the user choose the output devices */
	device=0;
	DialogBox(inst,(char*)2000,win,&devicedialogproc);
	outdev[0]=device;
	device=1;
	DialogBox(inst,(char*)2000,win,&devicedialogproc);
	outdev[1]=device;

	/* main dialog */
	DialogBox(inst,(char*)1000,0,&dialogproc);

	BASS_Free[1]();
	BASS_Free[0]();
	FreeBASS();

	return 0;
}
