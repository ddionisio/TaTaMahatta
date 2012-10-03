#include "tata_main.h"
#include "tata_world.h"

/////////////////////////////////////
// Purpose:	set the background music
//			this will also play the
//			new music
// Output:	destroys the old music and
//			sets the new one
// Return:	none
/////////////////////////////////////
void World::MusicSet(const char *filename)
{
	//check to see if the file path is the same
	if(strcmp(filename, m_musicPath.c_str()) == 0)
		return;

	MusicFree();

	m_music = BASS_StreamCreateFile(FALSE,(void*)filename,0,0,BASS_MP3_SETPOS);
	m_musicPath = filename;

	MusicPlay(true);
}

/////////////////////////////////////
// Purpose:	play the music or not
//			NOTE: music is always played
//			in loop.
// Output:	music is played or stop
// Return:	none
/////////////////////////////////////
void World::MusicPlay(bool bYes)
{
	if(m_music)
	{
		if(bYes)
		{
			BASS_StreamPlay(m_music, FALSE, BASS_SAMPLE_LOOP);
		}
		else
		{
			BASS_ChannelStop(m_music);
		}
	}
}

void World::MusicFree()
{
	if(m_music)
	{ BASS_StreamFree(m_music); m_music = 0; }

	m_musicPath = "none";
}