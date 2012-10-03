#include "tata_main.h"

#define NUM_BK_IMG	6

#define BK_SUB_FILENAME		"logo_scene\\bk_"
#define RENEGADE_FILENAME	"logo_scene\\renegadeware.tga"
#define DDPROD_FILENAME		"logo_scene\\dd_prod.tga"
#define BLACKIMG_FILENAME	"logo_scene\\black.jpg"

#define DAVID_IMG_FILENAME	"logo_scene\\david.tga"
#define DAVID_SPR_FILENAME	"logo_scene\\david.spr"

#define COW_SND_FILENAME	"logo_scene\\cows.mp3"

hTXT				g_bkImg[NUM_BK_IMG]={0};
hTXT				g_renegadeImg=0;
hTXT				g_ddProdImg=0;

hTXT				g_blackImg=0;

const float			g_cBKLoc[NUM_BK_IMG][2]={{0,0},{0.4f,0},{0.8f,0},{0,0.533333333f},{0.4f,0.533333333f},{0.8f,0.533333333f}};
float				g_BKLoc[NUM_BK_IMG][2]={0};

float				g_BKSize[NUM_BK_IMG][2]={0};

const float			g_cRenegadeLoc[2]={0.1f, 0.089583333f};
float				g_RenegadeLoc[2]={0};

float				g_RenegadeSize[2]={0};

hSPRITE				g_DSpr=0;

HSAMPLE				g_cowSnd=0;

const float			g_cDLocS[2]={1.0015625f,0.731666667f};
const float			g_cDLocE[2]={0.446875f,0.741666667f};

float				g_DLocS[2]={0,0};
float				g_DLocE[2]={0,0};

float				g_DLoc[2]={0,0};

float				g_DSize[2]={0};

float				g_DDProdSize[2]={0};

DWORD				g_bkClr=0;
DWORD				g_renegadeClr=0;
DWORD				g_blackClr=0;
DWORD				g_ddProdClr=0;

typedef enum {
	SCENE_BACKGROUND_APPEAR,
	SCENE_COW_SOUNDS,
	SCENE_DAVID_MOVES_IN,
	SCENE_DAVID_LOOKS_AT_FRONT_1,
	SCENE_DAVID_GRABS_SPRAY,
	SCENE_DAVID_SHAKES_SPRAY,
	SCENE_DAVID_SPRAYS_LOGO,
	SCENE_DAVID_LOOKS_AT_FRONT_2,
	SCENE_DAVID_GIVES_THUMBS_UP,
	SCENE_PRODUCTION,
	SCENE_PRODUCTION_WAIT,
	SCENE_WAIT_EXIT
} eStupidSceneState;

int g_stupidState = 0;

#define BK_DELAY				1000
#define COW_DELAY				0
#define DAVID_MOVE_DELAY		3000
#define DAVID_LOOK_FRONT_DELAY	1000
#define DAVID_SHAKE_SPRAY_DELAY	1000
#define DAVID_THUMBS_UP_DELAY	1500
#define LOGO_DELAY				3000
#define DDPROD_DELAY			2000
#define DDPROD_WAIT				1000
#define EXIT_DELAY				2000

win32Time	g_ahTimer;

PUBLIC RETCODE IntroProc(DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case GMSG_INIT:
		{
			g_stupidState = SCENE_BACKGROUND_APPEAR;

			char buff[MAXCHARBUFF];
			string filePath;

			//load background images
			//set their location and size
			for(int i = 0; i < NUM_BK_IMG; i++)
			{
				sprintf(buff, "%d", i+1);

				filePath = GAMEFOLDER;
				filePath += "\\";
				filePath += BK_SUB_FILENAME;
				filePath += buff;
				filePath += ".tga";

				g_bkImg[i] = TextureCreate(0, filePath.c_str(), false, 0);

				g_BKLoc[i][eX] = g_cBKLoc[i][eX]*((float)GFXGetScrnSize().cx);
				g_BKLoc[i][eY] = g_cBKLoc[i][eY]*((float)GFXGetScrnSize().cy);

				g_BKSize[i][eX] = (TextureGetWidth(g_bkImg[i])/640.0f)*((float)GFXGetScrnSize().cx);
				g_BKSize[i][eY] = (TextureGetHeight(g_bkImg[i])/480.0f)*((float)GFXGetScrnSize().cy);
			}

			//load renegadeware logo
			filePath = GAMEFOLDER;
			filePath += "\\";
			filePath += RENEGADE_FILENAME;

			g_renegadeImg = TextureCreate(0, filePath.c_str(), false, 0);

			g_RenegadeLoc[eX] = g_cRenegadeLoc[eX]*((float)GFXGetScrnSize().cx);
			g_RenegadeLoc[eY] = g_cRenegadeLoc[eY]*((float)GFXGetScrnSize().cy);

			if(g_renegadeImg)
			{
				g_RenegadeSize[eX] = (TextureGetWidth(g_renegadeImg)/640.0f)*((float)GFXGetScrnSize().cx);
				g_RenegadeSize[eY] = (TextureGetHeight(g_renegadeImg)/480.0f)*((float)GFXGetScrnSize().cy);
			}

			//load black image
			filePath = GAMEFOLDER;
			filePath += "\\";
			filePath += BLACKIMG_FILENAME;

			g_blackImg = TextureCreate(0, filePath.c_str(), false, 0);

			//load 'a david dionisio production'
			filePath = GAMEFOLDER;
			filePath += "\\";
			filePath += DDPROD_FILENAME;

			g_ddProdImg = TextureCreate(0, filePath.c_str(), false, 0);

			if(g_ddProdImg)
			{
				g_DDProdSize[eX] = (TextureGetWidth(g_ddProdImg)/640.0f)*((float)GFXGetScrnSize().cx);
				g_DDProdSize[eY] = (TextureGetHeight(g_ddProdImg)/480.0f)*((float)GFXGetScrnSize().cy);
			}

			//load the sprite
			filePath = GAMEFOLDER;
			filePath += "\\";
			filePath += DAVID_IMG_FILENAME;

			hTXT davidTxt = TextureCreate(0, filePath.c_str(), false, 0);

			filePath = GAMEFOLDER;
			filePath += "\\";
			filePath += DAVID_SPR_FILENAME;

			g_DSpr = SpriteLoad(davidTxt, -1, 0, filePath.c_str());

			TextureDestroy(&davidTxt);

			if(g_DSpr)
			{
				g_DLoc[eX] = g_DLocS[eX] = g_cDLocS[eX]*((float)GFXGetScrnSize().cx);
				g_DLoc[eY] = g_DLocS[eY] = g_cDLocS[eY]*((float)GFXGetScrnSize().cy);

				g_DLocE[eX] = g_cDLocE[eX]*((float)GFXGetScrnSize().cx);
				g_DLocE[eY] = g_cDLocE[eY]*((float)GFXGetScrnSize().cy);

				g_DSize[eX] = (SpriteGetSize(g_DSpr).cx/640.0f)*((float)GFXGetScrnSize().cx);
				g_DSize[eY] = (SpriteGetSize(g_DSpr).cy/480.0f)*((float)GFXGetScrnSize().cy);
			}

			filePath = GAMEFOLDER;
			filePath += "\\";
			filePath += COW_SND_FILENAME;
			g_cowSnd = BASS_SampleLoad(FALSE, (void*)filePath.c_str(), 0, 0, 1, 0);

			g_bkClr=0;
			g_renegadeClr=0;
			g_ddProdClr=0;
			g_blackClr = 0;

			TimeInit(&g_ahTimer, BK_DELAY);	

			//BASS_SamplePlay(g_cowSnd);
		}
		break;

	case GMSG_UPDATE:
		{
			float t;

			if(INPXKbAnyKeyPressed())
			{
				t = 1;

				g_stupidState = SCENE_WAIT_EXIT;
			}
			else
			{
				t = TimeGetTime(&g_ahTimer)/TimeGetDelay(&g_ahTimer);

				if(t > 1) t = 1;
			}

			if(g_DSpr)
				SpriteUpdateFrame(g_DSpr);

			switch(g_stupidState)
			{
			case SCENE_BACKGROUND_APPEAR:
				{
					BYTE alpha = (BYTE)(255*t);

					g_bkClr = D3DCOLOR_RGBA(255,255,255,alpha);

					if(t == 1)
					{
						TimeInit(&g_ahTimer, COW_DELAY);
						TimeReset(&g_ahTimer);

						g_stupidState = SCENE_COW_SOUNDS;
					}
				}
				break;
			case SCENE_COW_SOUNDS:
				{
					if(t == 1)
					{
						TimeInit(&g_ahTimer, DAVID_MOVE_DELAY);
						TimeReset(&g_ahTimer);

						g_stupidState = SCENE_DAVID_MOVES_IN;
					}
				}
				break;
			case SCENE_DAVID_MOVES_IN:
				{
					g_DLoc[eX] = g_DLocS[eX] + t*(g_DLocE[eX]-g_DLocS[eX]);
					g_DLoc[eY] = g_DLocS[eY] + t*(g_DLocE[eY]-g_DLocS[eY]);

					if(t == 1)
					{
						TimeInit(&g_ahTimer, DAVID_LOOK_FRONT_DELAY);
						TimeReset(&g_ahTimer);

						g_stupidState = SCENE_DAVID_LOOKS_AT_FRONT_1;

						if(g_DSpr)
							SpriteSetState(g_DSpr, 1);
					}
				}
				break;
			case SCENE_DAVID_LOOKS_AT_FRONT_1:
				if(t == 1)
				{
					TimeInit(&g_ahTimer, DAVID_LOOK_FRONT_DELAY);
					TimeReset(&g_ahTimer);

					g_stupidState = SCENE_DAVID_GRABS_SPRAY;

					if(g_DSpr)
						SpriteSetState(g_DSpr, 2);
				}
				break;
			case SCENE_DAVID_GRABS_SPRAY:
				if(t == 1)
				{
					TimeInit(&g_ahTimer, DAVID_SHAKE_SPRAY_DELAY);
					TimeReset(&g_ahTimer);

					g_stupidState = SCENE_DAVID_SHAKES_SPRAY;

					if(g_DSpr)
						SpriteSetState(g_DSpr, 3);
				}
				break;
			case SCENE_DAVID_SHAKES_SPRAY:
				if(t == 1)
				{
					TimeInit(&g_ahTimer, LOGO_DELAY);
					TimeReset(&g_ahTimer);

					g_stupidState = SCENE_DAVID_SPRAYS_LOGO;

					if(g_DSpr)
						SpriteSetState(g_DSpr, 4);
				}
				break;
			case SCENE_DAVID_SPRAYS_LOGO:
				{
					BYTE alpha = (BYTE)(255*t);

					g_renegadeClr = D3DCOLOR_RGBA(255,255,255,alpha);

					if(t == 1)
					{
						TimeInit(&g_ahTimer, DAVID_LOOK_FRONT_DELAY);
						TimeReset(&g_ahTimer);

						g_stupidState = SCENE_DAVID_LOOKS_AT_FRONT_2;

						if(g_DSpr)
							SpriteSetState(g_DSpr, 1);
					}
				}
				break;
			case SCENE_DAVID_LOOKS_AT_FRONT_2:
				if(t == 1)
				{
					TimeInit(&g_ahTimer, DAVID_THUMBS_UP_DELAY);
					TimeReset(&g_ahTimer);

					g_stupidState = SCENE_DAVID_GIVES_THUMBS_UP;

					if(g_DSpr)
						SpriteSetState(g_DSpr, 5);
				}
				break;
			case SCENE_DAVID_GIVES_THUMBS_UP:
				if(t == 1)
				{
					TimeInit(&g_ahTimer, DDPROD_DELAY);
					TimeReset(&g_ahTimer);

					//g_stupidState = SCENE_PRODUCTION;
					ProcChangeState(PROC_GAME);
				}
				break;
			case SCENE_PRODUCTION:
				{
					BYTE alpha = (BYTE)(255*t);

					g_blackClr = g_ddProdClr = D3DCOLOR_RGBA(255,255,255,alpha);

					if(t == 1)
					{
						TimeInit(&g_ahTimer, DDPROD_WAIT);
						TimeReset(&g_ahTimer);

						g_stupidState = SCENE_PRODUCTION_WAIT;
					}
				}
				break;	
			case SCENE_PRODUCTION_WAIT:
				if(t == 1)
				{
					TimeInit(&g_ahTimer, EXIT_DELAY);
					TimeReset(&g_ahTimer);

					g_stupidState = SCENE_WAIT_EXIT;
				}
				break;
			case SCENE_WAIT_EXIT:
				{
					BYTE alpha = (BYTE)(255.0f - 255.0f*t);

					g_ddProdClr = D3DCOLOR_RGBA(255,255,255,alpha);

					if(t == 1)
					{
						fadeaway_init tBKFX={255,255,255,1000};
						TataSetBKFX(BKFXCreate(eBKFX_FADEAWAY, &tBKFX));

						ProcChangeState(PROC_GAME);
					}
				}
				break;
			}
		}
		break;

	case GMSG_DISPLAY:
		{
			GFXBltModeEnable(0);

			RECT sArea={0,0,GFXGetScrnSize().cx,GFXGetScrnSize().cy};

			//display background
			for(int i = 0; i < NUM_BK_IMG; i++)
			{
				if(g_bkImg[i])
					TextureStretchBlt(g_bkImg[i], g_BKLoc[i][eX], g_BKLoc[i][eY], g_BKSize[i][eX], g_BKSize[i][eY], 0, &g_bkClr, 0);
			}

			//display david
			if(g_DSpr)
				SpriteStretchBlt(g_DSpr, g_DLoc, g_DSize[eX], g_DSize[eY], 0, 0);

			//display renegadeware
			if(g_renegadeImg)
				TextureStretchBlt(g_renegadeImg, g_RenegadeLoc[eX], g_RenegadeLoc[eY], g_RenegadeSize[eX], g_RenegadeSize[eY], 0, &g_renegadeClr, 0);

			if(g_blackImg)
			{
				TextureStretchBlt(g_blackImg, 0, 0, GFXGetScrnSize().cx, GFXGetScrnSize().cy, 0, &g_blackClr, 0);
			}

			//display 'a david dionisio production'
			if(g_ddProdImg)
			{
				float loc[2] = {(GFXGetScrnSize().cx/2.0f)-(g_DDProdSize[eX]/2.0f),(GFXGetScrnSize().cy/2.0f)-(g_DDProdSize[eY]/2.0f)};
				TextureStretchBlt(g_ddProdImg, loc[eX], loc[eY], g_DDProdSize[eX], g_DDProdSize[eY], 0, &g_ddProdClr, 0);
			}

			GFXBltModeDisable();
		}
		break;

	case GMSG_DESTROY:
		{
			for(int i = 0; i < NUM_BK_IMG; i++)
			{
				if(g_bkImg[i])
					TextureDestroy(&g_bkImg[i]);
			}

			if(g_renegadeImg)
				TextureDestroy(&g_renegadeImg);

			if(g_ddProdImg)
				TextureDestroy(&g_ddProdImg);

			if(g_blackImg)
				TextureDestroy(&g_blackImg);

			if(g_DSpr)
				SpriteDestroy(&g_DSpr);

			if(g_cowSnd)
				BASS_SampleFree(g_cowSnd);
		}
		break;
	}

	return RETCODE_SUCCESS;
}

