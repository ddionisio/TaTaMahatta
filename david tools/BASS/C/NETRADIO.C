/* BASS Internet radio example, copyright (c) 2002-2003 Ian Luck.
=================================================================
Other source: netradio.rc
Imports: bass.lib, kernel32.lib, user32.lib
*/

#include <windows.h>
#include <stdio.h>
#include "bass.h"


HWND win=NULL;

HSTREAM chan;

char *url[10]={ // stream URLs
	"http://205.188.234.67:8048","http://205.188.234.129:8024",
	"http://205.188.234.35:8034","http://160.79.1.141:8406",
	"http://193.201.220.87:8000","http://160.79.1.141:8006",
	"http://205.188.234.4:8016","http://205.188.234.4:8014",
	"http://server2.somafm.com:8000","http://server2.somafm.com:8082"
};

/* display error messages */
void Error(char *es)
{
	char mes[200];
	sprintf(mes,"%s\n(error code: %d)",es,BASS_ErrorGetCode());
	MessageBox(win,mes,"Error",0);
}

#define MESS(id,m,w,l) SendDlgItemMessage(win,id,m,(WPARAM)w,(LPARAM)l)

/* update stream title from metadata */
void DoMeta(char *meta)
{
	char *p;
	if (meta && (p=strstr(meta,"StreamTitle='"))) {
		p=strdup(p+13);
		strchr(p,';')[-1]=0;
		MESS(30,WM_SETTEXT,0,p);
		free(p);
	}
}

void CALLBACK MetaSync(HSYNC handle, DWORD channel, DWORD data, DWORD user)
{
	DoMeta((char*)data);
}

BOOL CALLBACK dialogproc(HWND h,UINT m,WPARAM w,LPARAM l)
{
	switch (m) {
		case WM_COMMAND:
			switch (LOWORD(w)) {
				case IDCANCEL:
					DestroyWindow(h);
					return 1;
				default:
					if (LOWORD(w)>=10 && LOWORD(w)<20) {
						BASS_StreamFree(chan); // close old stream
						MESS(31,WM_SETTEXT,0,"connecting...");
						MESS(30,WM_SETTEXT,0,"");
						MESS(32,WM_SETTEXT,0,"");
						if (!(chan=BASS_StreamCreateURL(url[LOWORD(w)-10],0,BASS_STREAM_META,0))) {
							MESS(31,WM_SETTEXT,0,"not playing");
							Error("Can't play the file");
						} else {
							/* get the broadcast name and bitrate */
							char *icy=BASS_StreamGetTags(chan,BASS_TAG_ICY);
							if (icy)
								for (;*icy;icy+=strlen(icy)+1) {
									if (!memcmp(icy,"icy-name:",9))
										MESS(31,WM_SETTEXT,0,icy+9);
									if (!memcmp(icy,"icy-br:",7)) {
										char br[30]="bitrate: ";
										strcat(br,icy+7);
										MESS(32,WM_SETTEXT,0,br);
									}
								}
							/* get the stream title and set sync for subsequent titles */
							DoMeta(BASS_StreamGetTags(chan,BASS_TAG_META));
							BASS_ChannelSetSync(chan,BASS_SYNC_META,0,&MetaSync,0);
							/* play it! */
							BASS_StreamPlay(chan,0,0);
						}
					}
			}
			break;

		case WM_INITDIALOG:
			win=h;
			// setup output device
			if (!BASS_Init(-1,44100,0,win)) {
				Error("Can't initialize device");
				DestroyWindow(win);
			}
			BASS_Start();
			return 1;
	}
	return 0;
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	/* check that BASS 1.8 was loaded */
	if (BASS_GetVersion()!=MAKELONG(1,8)) {
		MessageBox(0,"BASS version 1.8 was not loaded","Incorrect BASS.DLL",0);
		return 0;
	}

	/* display the window */
	DialogBox(hInstance,MAKEINTRESOURCE(1000),0,&dialogproc);

	BASS_Free();

	return 0;
}
