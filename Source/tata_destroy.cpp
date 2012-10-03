#include "tata_main.h"

#include "tata_ID.h"

#include "tata_sound.h"

//destroy all engine and everything

/////////////////////////////////////
// Name:	TataDestroy
// Purpose:	destroy engine and what-not
// Output:	death
// Return:	none
/////////////////////////////////////
PUBLIC void TataDestroy()
{
	//destroy shadow
	TataShadowDestroy();

	IDPageClear();  //remove all IDs (TODO: list out IDs that were not removed)

	GFXDestroy();

	//destroy all sounds
	TaTaSoundDestroy();

	BASS_Stop();
	BASS_Free();

	InputDestroy(); //destroy game input components
	INPXDestroy();
}