#include "gdix.h"
#include "gdix_i.h"

#include "gdix_qbsp.h"
#include "gdix_qbsp_i.h"

PROTECTED void QBSPVisBitClearAll(unsigned int *vis, int size)
{
	memset(vis, 0, sizeof(unsigned int) * size);
}

PROTECTED unsigned int * QBSPVisBitCreate(int count, int & sOut)
{
	sOut = count/32 + 1;

	unsigned int *newVis = (unsigned int *)GFX_MALLOC(sizeof(unsigned int)*sOut);

	if(!newVis)
	{ ASSERT_MSG(0, "Unable to allocate vis bits", "QBSPVisBitCreate"); return 0; }

	QBSPVisBitClearAll(newVis, sOut);

	return newVis;
}