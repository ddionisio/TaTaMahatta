#include "gdix.h"
#include "gdix_i.h"

PUBLIC void GFXHideCursor (void)
{
	ShowCursor (FALSE);
}

PUBLIC void GFXShowCursor (void)
{
	ShowCursor (TRUE);
}
