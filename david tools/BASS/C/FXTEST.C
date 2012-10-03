/* BASS DX8 Effects Test, copyright (c) 2001-2003 Ian Luck.
===========================================================
Imports: bass.lib, kernel32.lib, user32.lib, comdlg32.lib
*/

#include <windows.h>
#include <stdio.h>
#include "bass.h"


HWND win=NULL;
HINSTANCE inst;

DWORD chan;			// channel handle
HFX fx[4];			// 3 eq bands + reverb

OPENFILENAME ofn;
char path[MAX_PATH];

/* display error messages */
void Error(char *es)
{
	char mes[200];
	sprintf(mes,"%s\n(error code: %d)",es,BASS_ErrorGetCode());
	MessageBox(win,mes,"Error",0);
}


#define MESS(id,m,w,l) SendDlgItemMessage(win,id,m,(WPARAM)w,(LPARAM)l)

void UpdateFX(int b)
{
	int v=MESS(20+b,SBM_GETPOS,0,0);
	if (b<3) {
		BASS_FXPARAMEQ p;
		BASS_FXGetParameters(fx[b],&p);
		p.fGain=10.0-v;
		BASS_FXSetParameters(fx[b],&p);
	} else {
		BASS_FXREVERB p;
		BASS_FXGetParameters(fx[3],&p);
		p.fReverbMix=-0.012*v*v*v;
		BASS_FXSetParameters(fx[3],&p);
	}
}

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
							if (!(chan=BASS_StreamCreateFile(FALSE,file,0,0,BASS_SAMPLE_FX))
								&& !(chan=BASS_MusicLoad(FALSE,file,0,0,BASS_MUSIC_LOOP|BASS_MUSIC_RAMP|BASS_MUSIC_FX))) {
								// not a WAV/MP3 or MOD
								MESS(10,WM_SETTEXT,0,"click here to open a file...");
								Error("Can't play the file");
								break;
							}
							MESS(10,WM_SETTEXT,0,file);
							{ // setup the effects
								BASS_FXPARAMEQ p;
								fx[0]=BASS_ChannelSetFX(chan,BASS_FX_PARAMEQ);
								fx[1]=BASS_ChannelSetFX(chan,BASS_FX_PARAMEQ);
								fx[2]=BASS_ChannelSetFX(chan,BASS_FX_PARAMEQ);
								fx[3]=BASS_ChannelSetFX(chan,BASS_FX_REVERB);
								p.fGain=0;
								p.fBandwidth=18;
								p.fCenter=125;
								BASS_FXSetParameters(fx[0],&p);
								p.fCenter=1000;
								BASS_FXSetParameters(fx[1],&p);
								p.fCenter=8000;
								BASS_FXSetParameters(fx[2],&p);
								UpdateFX(0);
								UpdateFX(1);
								UpdateFX(2);
								UpdateFX(3);
							}
							// play both MOD and stream, it must be one of them! :)
							BASS_MusicPlay(chan);
							BASS_StreamPlay(chan,0,BASS_SAMPLE_LOOP);
						}
					}
					return 1;
			}
			break;

		case WM_VSCROLL:
			{
				int a=SendMessage((HWND)l,SBM_GETPOS,0,0);
				switch (LOWORD(w)) {
					case SB_THUMBPOSITION:
						a=HIWORD(w);
						break;
					case SB_LINELEFT:
						a=max(a-1,0);
						break;
					case SB_LINERIGHT:
						if (a==20) return 1;
						a++;
						break;
					case SB_PAGELEFT:
						a=max(a-5,0);
						break;
					case SB_PAGERIGHT:
						if (a==20) return 1;
						a=min(a+5,20);
						break;
					default:
						return 1;
				}
				SendMessage((HWND)l,SBM_SETPOS,a,1);
				UpdateFX(GetDlgCtrlID((HWND)l)-20);
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
			// setup output - default device, 44100hz, stereo, 16 bits
			if (!BASS_Init(-1,44100,0,win)) {
				Error("Can't initialize device");
				DestroyWindow(win);
				return 1;
			}
			{
				// check that DX8 features are available
				BASS_INFO bi={sizeof(bi)};
				BASS_GetInfo(&bi);
				if (bi.dsver<8) {
					BASS_Free();
					Error("DirectX 8 is not installed");
					DestroyWindow(win);
				}
			}
			BASS_Start();
			/* initialize eq/reverb sliders */
			MESS(20,SBM_SETRANGE,0,20);
			MESS(20,SBM_SETPOS,10,1);
			MESS(21,SBM_SETRANGE,0,20);
			MESS(21,SBM_SETPOS,10,1);
			MESS(22,SBM_SETRANGE,0,20);
			MESS(22,SBM_SETPOS,10,1);
			MESS(23,SBM_SETRANGE,0,20);
			MESS(23,SBM_SETPOS,20,1);
			return 1;
	}
	return 0;
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	inst=hInstance;

	/* check that BASS 1.8 was loaded */
	if (BASS_GetVersion()!=MAKELONG(1,8)) {
		MessageBox(0,"BASS version 1.8 was not loaded","Incorrect BASS.DLL",0);
		return 0;
	}

	DialogBox(inst,(char*)1000,0,&dialogproc);

	BASS_Free();

	return 0;
}
