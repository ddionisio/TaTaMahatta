#ifndef _gdix_qbsp_i_h
#define _gdix_qbsp_i_h

//internal use of QBSP stuff

#define	DIST_EPSILON	0.03125f	//stuff

#define VIS_SET(vis, i) ((vis)[(i) >> 5] |= (1 << ((i) & 31)))
#define VIS_IS_ON(vis, i) ((vis)[(i) >> 5] & (1 << ((i) & 31 )))
#define VIS_CLEAR(vis, i) ((vis)[(i) >> 5] &= ~(1 << ((i) & 31)))

PROTECTED void QBSPVisBitClearAll(unsigned int *vis, int size);

PROTECTED unsigned int * QBSPVisBitCreate(int count, int & sOut);

PROTECTED inline int QBSPIsClusterVisible(hQBSP qbsp, int current, int test)
{
	// Make sure we have valid memory and that the current cluster is > 0.
	// If we don't have any memory or a negative cluster, return a visibility (1).
	if(!qbsp->clusters.pBitsets || current < 0) return 1;

	// Use binary math to get the 8 bit visibility set for the current cluster
	BYTE visSet = qbsp->clusters.pBitsets[(current*qbsp->clusters.bytesPerCluster) + (test / 8)];

	// Now that we have our vector (bitset), do some bit shifting to find if
	// the "test" cluster is visible from the "current" cluster, according to the bitset.

	// Return the result ( either 1 (visible) or 0 (not visible) )
	int r = visSet & (1 << ((test) & 7));
	return r;
}

//translate the actual raw vertex data and plane
PROTECTED void _QBSPModelTranslate(hQBSP qbsp, gfxVtxQBSP *pVtx, gfxBSPModel *mdl, float trans[eMaxPt]);

#endif