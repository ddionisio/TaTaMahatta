#include "tata_main.h"

#include "tata_globals.h"

#include "tata_sound.h"

struct SndDat {
	bool		b3D;		//is 3D?
	HSAMPLE		sound;		//the sound
};

vector<SndDat> g_sounds;

/////////////////////////////////////
// Name:	TaTaSoundLoad
// Purpose:	loads many sounds used in
//			the game
// Output:	loads many sounds
// Return:	none
/////////////////////////////////////
PUBLIC void TaTaSoundLoad(const char *filename)
{
	hCFG cfg = CfgFileLoad(filename);

	if(cfg)
	{
		char sect[MAXCHARBUFF], filename[MAXCHARBUFF];

		string sndPath;

		int maxSample, bLoop, b3D;
		float minDist, maxDist;

		DWORD flag;

		TaTaSoundDestroy();

		for(int i = 0;; i++)
		{
			SndDat newSND={0};

			flag=0;

			sprintf(sect, "sound_%d", i);

			if(CfgSectionExist(cfg, sect))
			{
				if(CfgGetItemStr(cfg, sect, "file", filename))
				{
					sndPath = GAMEFOLDER;
					sndPath += "\\";
					sndPath += filename;
				}

				maxSample = CfgGetItemInt(cfg, sect, "maxSample");
				bLoop	  = CfgGetItemInt(cfg, sect, "bLoop");
				b3D		  = CfgGetItemInt(cfg, sect, "b3D");
				minDist	  = CfgGetItemFloat(cfg, sect, "minDist");
				maxDist	  = CfgGetItemFloat(cfg, sect, "maxDist");

				newSND.b3D = b3D ? true : false;

				if(b3D) flag = BASS_SAMPLE_3D;
				if(bLoop) flag |= BASS_SAMPLE_LOOP;

				newSND.sound = BASS_SampleLoad(FALSE, (void *)sndPath.c_str(),
					0, 0, maxSample, flag | BASS_SAMPLE_MUTEMAX |
		BASS_SAMPLE_OVER_VOL | BASS_SAMPLE_OVER_POS | BASS_SAMPLE_OVER_DIST);

				if(newSND.sound)
				{
					BASS_SAMPLE info;

					BASS_SampleGetInfo(newSND.sound, &info);

					info.mindist = minDist;
					info.maxdist = maxDist;

					BASS_SampleSetInfo(newSND.sound, &info);
				}
				else
				{
					ErrorMsg("TaTaSoundLoad Can't Load", sndPath.c_str(), false);
				}

				g_sounds.push_back(newSND);
			}
			else
				break;
		}

		CfgFileDestroy(&cfg);
	}
}

/////////////////////////////////////
// Name:	TaTaSoundDestroy
// Purpose:	destroys many sounds
// Output:	many sounds destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void TaTaSoundDestroy()
{
	for(int i = 0; i < g_sounds.size(); i++)
	{
		if(g_sounds[i].sound)
			BASS_SampleFree(g_sounds[i].sound);
	}

	g_sounds.clear();
}

/////////////////////////////////////
// Name:	TaTaSoundPlay
// Purpose:	plays a sound with given
//			index
// Output:	sound played
// Return:	the channel you can
//			manipulate
/////////////////////////////////////
PUBLIC HCHANNEL TaTaSoundPlay(DWORD ind, BASS_3DVECTOR *pos, 
							  BASS_3DVECTOR *orient, BASS_3DVECTOR *vel)
{
	HCHANNEL chan = 0;

	if(ind < g_sounds.size() && g_sounds[ind].sound)
	{
		if(g_sounds[ind].b3D)
		{
			chan = BASS_SamplePlay3D(g_sounds[ind].sound, pos, orient, vel);

			if(!chan)
				ErrorMsg("TaTaSoundPlay", "Unable to play 3D", false);
		}
		else
		{
			chan = BASS_SamplePlay(g_sounds[ind].sound);

			if(!chan)
				ErrorMsg("TaTaSoundPlay", "Unable to play 2D", false);
		}
	}

	return chan;
}