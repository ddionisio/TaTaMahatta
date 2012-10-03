#include "gdix.h"
#include "gdix_i.h"

#include "gdix_qbsp.h"
#include "gdix_qbsp_i.h"

/////////////////////////////////////
// Name:	QBSPFindLeaf
// Purpose:	find the leaf index where
//			the given pt resides.
// Output:	none
// Return:	leaf index >= 0 if found
/////////////////////////////////////
PUBLIC int QBSPFindLeaf(hQBSP qbsp, const float pt[eMaxPt])
{
	int i = 0, pI;
	float distance = 0.0f;

	while(i >= 0)
	{
		pI = qbsp->nodes[i].plane;
		distance = qbsp->planes[pI].a*pt[eX] + qbsp->planes[pI].b*pt[eY]
			      +qbsp->planes[pI].c*pt[eZ] - qbsp->planes[pI].d;

		if(distance >= 0) //front
			i = qbsp->nodes[i].child[0];
		else
			i = qbsp->nodes[i].child[1];
	}

	return ~i; //== -(i+1)
}

/////////////////////////////////////
// Name:	QBSPUseVertexLighting
// Purpose:	enable/disable vertex lighting
//			QBSP will use lightmap if
//			vertex lighting is disabled
// Output:	vertex lighting set
// Return:	none
/////////////////////////////////////
PUBLIC void QBSPUseVertexLighting(hQBSP qbsp, bool bEnable)
{
	gfxVtxQBSP *pVtx;//=qbsp->vtx;

	BYTE	   *pVtxClr = qbsp->vtxClr;

	if(SUCCEEDED(qbsp->vtx->Lock(0,0, (void**)&pVtx, 0)))
	{
		if(!bEnable)
		{
			for(int i = 0; i < qbsp->numVtx; i++)
			{
				pVtx[i].color = D3DCOLOR_RGBA(255,255,255,pVtxClr[eA]);

				pVtxClr += eMaxClr;
			}

			CLEARFLAG(qbsp->status, QBSP_FLAG_VTXLIGHTING);
		}
		else
		{
			for(int i = 0; i < qbsp->numVtx; i++)
			{
				pVtx[i].color = D3DCOLOR_RGBA(pVtxClr[eR],pVtxClr[eG],pVtxClr[eB],pVtxClr[eA]);

				pVtxClr += eMaxClr;
			}

			SETFLAG(qbsp->status, QBSP_FLAG_VTXLIGHTING);
		}

		qbsp->vtx->Unlock();
	}
}

/////////////////////////////////////
// Name:	QBSPGetTexture
// Purpose:	get the texture
// Output:	none
// Return:	the texture
/////////////////////////////////////
PUBLIC hTXT __fastcall QBSPGetTexture(hQBSP qbsp, int txtInd)
{
	return qbsp->textures[txtInd].texture;
}

/////////////////////////////////////
// Name:	QBSPGetTextureFlags
// Purpose:	grab the flags of texture
//			(based on QBSP texture flags)
// Output:	none
// Return:	flags
/////////////////////////////////////
PUBLIC int __fastcall QBSPGetTextureFlags(hQBSP qbsp, int txtInd)
{
	return qbsp->textures[txtInd].flags;
}

/////////////////////////////////////
// Name:	QBSPGetTextureContents
// Purpose:	grab the contents of texture
//			(based on QBSP texture contents)
// Output:	none
// Return:	contents
/////////////////////////////////////
PUBLIC int __fastcall QBSPGetTextureContents(hQBSP qbsp, int txtInd)
{
	return qbsp->textures[txtInd].contents;
}

/////////////////////////////////////
// Name:	QBSPGetNumLeaves
// Purpose:	get number of leaves
// Output:	none
// Return:	the number of leaves.
/////////////////////////////////////
PUBLIC int QBSPGetNumLeaves(hQBSP qbsp)
{
	return qbsp->numLeaves;
}