#include "gdix.h"
#include "gdix_i.h"

#include "gdix_qbsp.h"
#include "gdix_qbsp_i.h"

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void QBSPDestroy(hQBSP *qbsp)
{
	if(*qbsp)
	{
		if((*qbsp)->vtx)
			(*qbsp)->vtx->Release();

		if((*qbsp)->vtxClr)
			GFX_FREE((*qbsp)->vtxClr);

		if((*qbsp)->faces)
			GFX_FREE((*qbsp)->faces);

		if((*qbsp)->faceVis)
			GFX_FREE((*qbsp)->faceVis);

		if((*qbsp)->textures)
		{
			//release all textures
			for(int i = 0; i < (*qbsp)->numTxt; i++)
			{
				if((*qbsp)->textures[i].texture)
					TextureDestroy(&(*qbsp)->textures[i].texture);
			}

			GFX_FREE((*qbsp)->textures);
		}

		if((*qbsp)->lightmaps)
		{
			//release all textures
			for(int i = 0; i < (*qbsp)->numLightMap; i++)
				TextureDestroy(&(*qbsp)->lightmaps[i]);

			GFX_FREE((*qbsp)->lightmaps);
		}

		if((*qbsp)->nodes)
			GFX_FREE((*qbsp)->nodes);

		if((*qbsp)->leaves)
			GFX_FREE((*qbsp)->leaves);

		if((*qbsp)->leafFaces)
			GFX_FREE((*qbsp)->leafFaces);

		if((*qbsp)->planes)
			GFX_FREE((*qbsp)->planes);

		if((*qbsp)->clusters.pBitsets)
			GFX_FREE((*qbsp)->clusters.pBitsets);

		if((*qbsp)->meshIndBuff)
			(*qbsp)->meshIndBuff->Release();

		if((*qbsp)->models)
		{
			for(int i = 1; i < (*qbsp)->numModels; i++)
			{
				if((*qbsp)->models[i].brushes)
				{
					for(int j = 0; j < (*qbsp)->models[i].numOfBrushes; j++)
					{
						if((*qbsp)->models[i].brushes[j].planes)
							GFX_FREE((*qbsp)->models[i].brushes[j].planes);

						if((*qbsp)->models[i].brushes[j].tPlanes)
							GFX_FREE((*qbsp)->models[i].brushes[j].tPlanes);

						if((*qbsp)->models[i].brushes[j].txtID)
							GFX_FREE((*qbsp)->models[i].brushes[j].txtID);
					}

					GFX_FREE((*qbsp)->models[i].brushes);
				}
			}

			GFX_FREE((*qbsp)->models);
		}

		if((*qbsp)->brushes)
			GFX_FREE((*qbsp)->brushes);

		if((*qbsp)->brushVis)
			GFX_FREE((*qbsp)->brushVis);

		if((*qbsp)->leafBrushes)
			GFX_FREE((*qbsp)->leafBrushes);

		if((*qbsp)->brushSides)
			GFX_FREE((*qbsp)->brushSides);

		GFX_FREE(*qbsp);
	}

	qbsp=0;
}