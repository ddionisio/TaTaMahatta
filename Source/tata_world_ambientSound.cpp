#include "tata_main.h"
#include "tata_world.h"

#include "tata_globals.h"

AmbientSound::AmbientSound(const char *name, const char *filename, 
		const D3DXVECTOR3 & loc, 
		const D3DXVECTOR3 & orient, 
		float minLength, float length, bool bLoop)
: GameID(this), Name(name), m_sound(0), m_bLoop(bLoop)
{
	DWORD flags = m_bLoop ? BASS_SAMPLE_LOOP : 0;

	m_sound = BASS_SampleLoad(FALSE, (void *)filename, 0, 0, 
		MAX_SAMPLE_PLAY, flags | BASS_SAMPLE_3D | BASS_SAMPLE_MUTEMAX |
		BASS_SAMPLE_OVER_VOL | BASS_SAMPLE_OVER_POS | BASS_SAMPLE_OVER_DIST);

	BASS_SAMPLE info;

	BASS_SampleGetInfo(m_sound, &info);

	info.mindist = minLength;//
	info.maxdist = length;

	BASS_SampleSetInfo(m_sound, &info);

	m_loc.x = loc.x;
	m_loc.y = loc.y;
	m_loc.z = -loc.z;

	m_orient.x = orient.x;
	m_orient.y = orient.y;
	m_orient.z = -orient.z;

	DWORD ret = BASS_SamplePlay3D(m_sound, &m_loc, &m_orient, 0);

	if(ret == 0)
	{
		DWORD ass = BASS_ErrorGetCode();
		int arg=0;

		switch(ass)
		{
		case BASS_ERROR_START:
			arg=-1;
			break;
		case BASS_ERROR_HANDLE:
			arg=-2;
			break;
		case BASS_ERROR_NO3D:
			arg=-3;
			break;
		case BASS_ERROR_NOCHAN:
			arg=-4;
			break;
		case BASS_ERROR_NOHW:
			arg=-5;
			break;
		case BASS_ERROR_BUFLOST:
			arg=-6;
			break;
		}
	}
}

AmbientSound::~AmbientSound()
{
	if(m_sound)
		BASS_SampleFree(m_sound);
}

bool AmbientSound::Update()
{
	if(m_sound)
	{
		DWORD ret = BASS_ChannelIsActive(m_sound);

		if(m_bLoop
			|| ret == BASS_ACTIVE_STOPPED
			|| ret == BASS_ACTIVE_STALLED)
			return false;
	}

	return true;
}