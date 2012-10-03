#ifndef _tata_sound_h
#define _tata_sound_h

//all stuff related to sound

/////////////////////////////////////
// Name:	TaTaSoundLoad
// Purpose:	loads many sounds used in
//			the game
// Output:	loads many sounds
// Return:	none
/////////////////////////////////////
PUBLIC void TaTaSoundLoad(const char *filename);

/////////////////////////////////////
// Name:	TaTaSoundDestroy
// Purpose:	destroys many sounds
// Output:	many sounds destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void TaTaSoundDestroy();

/////////////////////////////////////
// Name:	TaTaSoundPlay
// Purpose:	plays a sound with given
//			index
// Output:	sound played
// Return:	the channel you can
//			manipulate
/////////////////////////////////////
PUBLIC HCHANNEL TaTaSoundPlay(DWORD ind, BASS_3DVECTOR *pos, 
							  BASS_3DVECTOR *orient, BASS_3DVECTOR *vel);


#endif