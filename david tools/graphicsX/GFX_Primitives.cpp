#include "gdix.h"
#include "gdix_i.h"

/////////////////////////////////////
// Name:	GFXDrawBoxXZ
// Purpose:	draws a box from xz plane
// Output:	pMtx is filled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXDrawBoxXZ(float xMin, float zMin, float xMax, float zMax, float h, unsigned int color)
{
	D3DXMATRIX mtx;
	g_p3DDevice->SetTransform(D3DTS_WORLD, D3DXMatrixIdentity(&mtx));

	gfxVtx vtx[NUMPTQUAD*2]={0};

	//line one
	vtx[0].x = xMin; vtx[0].y = h; vtx[0].z = zMin;
	vtx[0].nX = 0; vtx[0].nY = 1; vtx[0].nZ = 0;

	vtx[1].x = xMin; vtx[1].y = h; vtx[1].z = zMax;
	vtx[1].nX = 0; vtx[1].nY = 1; vtx[1].nZ = 0;

	//line two
	vtx[2] = vtx[1];

	vtx[3].x = xMax; vtx[3].y = h; vtx[3].z = zMax;
	vtx[3].nX = 0; vtx[3].nY = 1; vtx[3].nZ = 0;

	//line three
	vtx[4] = vtx[3];

	vtx[5].x = xMax; vtx[5].y = h; vtx[5].z = zMin;
	vtx[5].nX = 0; vtx[5].nY = 1; vtx[5].nZ = 0;

	//line four
	vtx[6] = vtx[5];
	vtx[7] = vtx[0];

	g_p3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, NUMPTQUAD, vtx, sizeof(gfxVtx));
}

#define PIXELFORMAT (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

struct pixelVtx {
	float	x,y,z,rhw;
	DWORD	clr;
};

/////////////////////////////////////
// Name:	GFXDrawPixel
// Purpose:	draws a 2d pixel
// Output:	yay
// Return:	none
/////////////////////////////////////
PUBLIC void GFXDrawPixel(float x, float y, unsigned int clr)
{
	pixelVtx pt;
	pt.x=x; pt.y=y; pt.z=1.0f; pt.rhw=1.0f; pt.clr=clr;

	g_p3DDevice->SetFVF(PIXELFORMAT);

	g_p3DDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &pt, sizeof(pixelVtx));
}

/*typedef struct _gfxVtx {
    float	x, y, z; //model space vertex position
	DWORD	clr;
} gfxVtx;

#define GFXVERTEXFLAG (D3DFVF_XYZ | D3DFVF_DIFFUSE) //xyz and color

/////////////////////////////////////
// Name:	GFXDrawLine
// Purpose:	draws a line from pt1 to pt2
// Output:	line drawn
// Return:	none
/////////////////////////////////////
PUBLIC void GFXDrawLine(float pt1[eMaxPt], float pt2[eMaxPt], unsigned int clr, bool bAntiAlias)
{
}*/

#define BOX2DVTX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE) //xyzw and color

typedef struct _box2DVtx {
    float x, y, z, rhw;
    DWORD color;
} box2DVtx;

/////////////////////////////////////
// Name:	GFXDrawBox2D
// Purpose:	draws a 2d filled box
// Output:	box
// Return:	none
/////////////////////////////////////
PUBLIC void GFXDrawBox2D(const RECT *pRect, DWORD clr)
{
	box2DVtx vtxData[4];

	vtxData[0].x = pRect->left; vtxData[0].y = pRect->top; vtxData[0].z = 0; vtxData[0].rhw = 1;
	vtxData[0].color = clr;

	vtxData[1].x = pRect->left; vtxData[1].y = pRect->bottom; vtxData[1].z = 0; vtxData[1].rhw = 1;
	vtxData[1].color = clr;

	vtxData[2].x = pRect->right; vtxData[2].y = pRect->bottom; vtxData[2].z = 0; vtxData[2].rhw = 1;
	vtxData[2].color = clr;

	vtxData[3].x = pRect->right; vtxData[3].y = pRect->top; vtxData[3].z = 0; vtxData[3].rhw = 1;
	vtxData[3].color = clr;

	g_p3DDevice->SetFVF(BOX2DVTX);

	g_p3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vtxData, sizeof(box2DVtx));
}