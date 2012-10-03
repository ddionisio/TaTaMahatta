#include "gdix.h"
#include "gdix_i.h"

PRIVATE void _destroyTxt(hTXT *txt) //this will destroy the object, will not search list
{
	if(*txt)
	{
		if((*txt)->texture) //destroy frame
			(*txt)->texture->Release();

		GFX_FREE(*txt);
		txt=0;
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hTXT TextureCreateCustom(unsigned int newID, const char *name,
								DWORD width, DWORD height, DWORD mipLevels,
								DWORD usage, eBPP bpp)
{
	hTXT newTxt = (hTXT)GFX_MALLOC(sizeof(gfxTexture));

	if(!newTxt)
	{ ASSERT_MSG(0, "Unable to allocate new texture", "Error in TextureCreate"); return 0; }

	newTxt->ID = newID;

	//get filename
	strcpy(newTxt->name,name);

	D3DFORMAT d3dFormat;
	HRESULT hr;

	switch(bpp)
	{
	case BPP_32:
		d3dFormat=D3DFMT_A8R8G8B8;//D3DFMT_X8R8G8B8;
		break;
	case BPP_24:
		d3dFormat=D3DFMT_R8G8B8;
		break;
	case BPP_16:
		d3dFormat=D3DFMT_R5G6B5;
		break;
	case BPP_15a:
		d3dFormat=D3DFMT_A1R5G5B5;
		break;
	case BPP_15:
		d3dFormat=D3DFMT_X1R5G5B5;
		break;
	case BPP_8:
		d3dFormat=D3DFMT_P8;
		break;
	}

	hr = D3DXCreateTexture(g_p3DDevice, width, height,
		mipLevels, usage, d3dFormat, D3DPOOL_MANAGED,
		&newTxt->texture);

	if(_GFXCheckError(hr, true, "Error in TextureCreateCustom"))
		return 0;

	D3DSURFACE_DESC d3dsd;
	newTxt->texture->GetLevelDesc(0, &d3dsd);

	newTxt->width = d3dsd.Width;
	newTxt->height = d3dsd.Height;

	newTxt->bFile = false;

	return newTxt;
}

PROTECTED inline HRESULT _TextureLoad(hTXT txt)
{
	if(!txt->bClrKey)
		return D3DXCreateTextureFromFile(g_p3DDevice, txt->name, &txt->texture);

	return D3DXCreateTextureFromFileEx(g_p3DDevice, txt->name,
			D3DX_DEFAULT, D3DX_DEFAULT, //width/height
			D3DX_DEFAULT,				//mip level
			0,							//WTF
			D3DFMT_UNKNOWN,				//pixel format (use file)
			D3DPOOL_DEFAULT,//D3DPOOL_MANAGED,			//WTF
			D3DX_DEFAULT,				//filter         (we should let user do this...maybe...)
			//D3DX_FILTER_LINEAR,
			D3DX_DEFAULT,				//mip-map filter (we should let user do this...maybe...)
			txt->srcClrKey,					//the color keying
			0,							//src info, whatever the hell that is
			0,							//palette info
			&txt->texture);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hTXT TextureCreate(unsigned int newID, const char *filename, 
						  bool clrkey, D3DCOLOR srcClrKey)
{
	char theName[NAMEMAX]={0};
	GetFilename(filename, theName, NAMEMAX-1);

	hTXT newTxt = TextureSearchName(filename);

	//we found an identical name, so use that
	if(newTxt)
	{
		TextureAddRef(newTxt);
		return newTxt;
	}

	newTxt = (hTXT)GFX_MALLOC(sizeof(gfxTexture));

	if(!newTxt)
	{ ASSERT_MSG(0, "Unable to allocate new texture", "Error in TextureCreate"); return 0; }

	newTxt->ID = newID;

	newTxt->srcClrKey = srcClrKey;
	newTxt->bClrKey = clrkey;

	//get filename
	strcpy(newTxt->name,filename);

	newTxt->bFile = true;

	HRESULT hr = _TextureLoad(newTxt);

	if(_GFXCheckError(hr, true, "Error in TextureCreate"))
		return 0;

	D3DSURFACE_DESC d3dsd;
	newTxt->texture->GetLevelDesc(0, &d3dsd);

	newTxt->width = d3dsd.Width;
	newTxt->height = d3dsd.Height;

	//append object to list
	g_TXTLIST.insert(g_TXTLIST.end(), (unsigned int)newTxt);

	return newTxt;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TextureAddRef(hTXT txt)
{
	if(txt)
		txt->ref++;
}

/////////////////////////////////////
// Name:	TextureSetListFile
// Purpose:	sets the texture list file
// Output:	the texture list file is set
// Return:	none
/////////////////////////////////////
PUBLIC void TextureSetListFile(const char *filename)
{
	//just copy it
	strcpy(g_TXTLISTFILE, filename);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PRIVATE hTXT _TextureSearchList(unsigned int ID)
{
	hTXT thisTxt;

	for(LISTPTR::iterator i = g_TXTLIST.begin(); i != g_TXTLIST.end(); ++i)
	{
		thisTxt = (hTXT)(*i);

		if(thisTxt->ID == ID)
			return thisTxt;
	}

	return 0;
}

/////////////////////////////////////
// Name:	TextureSearchName
// Purpose:	get texture by it's name
// Output:	none
// Return:	the texture if found
/////////////////////////////////////
PUBLIC hTXT TextureSearchName(const char *name)
{
	hTXT thisTxt;

	for(LISTPTR::iterator i = g_TXTLIST.begin(); i != g_TXTLIST.end(); ++i)
	{
		thisTxt = (hTXT)(*i);

		if(stricmp(thisTxt->name, name) == 0)
			return thisTxt;
	}

	return 0;
}

/////////////////////////////////////
// Name:	TextureSearch
// Purpose:	load image/texture via 
//			list file with ID
//			NOTE: This will first find
//			the ID from the texture
//			list.
// Output:	add texture to list if not
//			yet done
// Return:	the texture
/////////////////////////////////////
PUBLIC hTXT TextureSearch(unsigned int ID)
{
	//first, check to see if image already exist
	hTXT texture=_TextureSearchList(ID);

	if(!texture) //search through the file
	{
		char txtFName[MAXCHARBUFF];
		unsigned int txtID, clrKey, r=0, g=0, b=0;

		//load up file
		FILE *fp = fopen(g_TXTLISTFILE, "rt");

		if(!fp)
			return 0;

		while(!feof(fp))
		{
			fscanf(fp, "%d %s %d", &txtID, txtFName, &clrKey);

			if(clrKey == 1)
				fscanf(fp, "%d,%d,%d", &r, &g, &b);
			else
				fscanf(fp, "\n");

			if(txtID == ID) //does it match???
			{
				//load this puppy up!
				texture = TextureCreate(ID, txtFName, clrKey == 1 ? true : false, D3DCOLOR_XRGB(r,g,b));
				break;
			}
		}

		fclose(fp);
	}
	else
		TextureAddRef(texture);

	return texture;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void _TextureDestroyAll()
{
	hTXT thisTxt;

	for(LISTPTR::iterator i = g_TXTLIST.begin(); i != g_TXTLIST.end(); ++i)
	{
		thisTxt = (hTXT)(*i);
		_destroyTxt(&thisTxt);
	}

	g_TXTLIST.clear();
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TextureDestroy(hTXT *txt)
{
	if(*txt)
	{
		if((*txt)->ref <= 0)
		{
			//just remove the pointer of model from list
			g_TXTLIST.remove((unsigned int)(*txt));

			_destroyTxt(txt);
		}
		else
			(*txt)->ref--;
	}

	txt=0;
}

/////////////////////////////////////
// Name:	TextureSet
// Purpose:	sets texture for texture
//			mapping.  Base is usually
//			0.
// Output:	texture is set
// Return:	none
/////////////////////////////////////
PUBLIC void TextureSet(hTXT txt, unsigned int base)
{
	if(txt)
		g_p3DDevice->SetTexture(base, txt->texture);
	else
		g_p3DDevice->SetTexture(base, 0);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int TextureGetWidth(hTXT txt)
{
	return txt->width;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int TextureGetHeight(hTXT txt)
{
	return txt->height;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC unsigned int TextureGetID(hTXT txt)
{
	return txt->ID;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureLock(hTXT txt, DWORD lvl, const RECT *pRect, DWORD flags,
						   int *pitchOut, void **bitsOut)
{
	D3DLOCKED_RECT lRect;

	if(FAILED(txt->texture->LockRect(lvl, &lRect, pRect, flags)))
		return RETCODE_FAILURE;

	*pitchOut = lRect.Pitch;

	*bitsOut = lRect.pBits;

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TextureUnlock(hTXT txt, DWORD lvl)
{
	txt->texture->UnlockRect(lvl);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hTXT TextureCreateAlphaCircle(const char *name, 
									 BYTE r, BYTE g, BYTE b,
									 DWORD usage, DWORD radius)
{
	hTXT newTxt = TextureCreateCustom(0, name,
								radius, radius, 0,
								usage, BPP_32);

	if(!newTxt)
	{ ASSERT_MSG(0, "Unable to allocate new texture", "Error in TextureCreateAlphaCircle"); return 0; }

	//now calculate circle stuff
	BYTE *gooks, *gooksWalk;

	int		xOff, yOff, Value, x, y, pitch;

	double TempValue;

	if(TextureLock(newTxt, 0, 0, 0, &pitch, (void**)&gooks) == RETCODE_SUCCESS)
	{
		for (y = 0; y < newTxt->height; y++)
		{
			gooksWalk = gooks + y*pitch;
			//BYTE *gooksWalker = &BOARDPOS(gooks, y, 0, Width);
			for (x = 0; x < newTxt->width; x++)
			{
				xOff = x - newTxt->width / 2;
				yOff = y - newTxt->height / 2;
				TempValue = sqrtf(xOff * xOff + yOff * yOff) / (newTxt->width / 2);
				// You can run TempValue through whatever transforms you want here.
				// Here I've chosen to make the solid center a bit larger by having
				// TempValue stay zero over a larger range while still hitting 1.0
				// at the same point.
				TempValue = (TempValue - 0.1) * (1.0 / 0.9);
				
				Value = 255 - (int)(TempValue * 255 + 0.5);
				if (Value < 0)
					Value = 0;
				if (Value > 255)
					Value = 255;

				gooksWalk[3] = Value;
				gooksWalk[2] = r;
				gooksWalk[1] = g;
				gooksWalk[0] = b;
				
				gooksWalk+=4;
			}
		}

		TextureUnlock(newTxt, 0);
	}

	return newTxt;
}

/////////////////////////////////////
// Name:	TextureInvalidateAll
// Purpose:	invalidate textures
// Output:	textures invalidated
// Return:	none
/////////////////////////////////////
PROTECTED void TextureInvalidateAll()
{
	hTXT thisTxt;

	for(LISTPTR::iterator i = g_TXTLIST.begin(); i != g_TXTLIST.end(); ++i)
	{
		thisTxt = (hTXT)(*i);

		if(thisTxt)
		{
			if(thisTxt->bFile)
			{
				thisTxt->texture->Release();
				thisTxt->texture = 0;
			}
			else
				thisTxt->texture->PreLoad();
		}
	}
}

/////////////////////////////////////
// Name:	TextureRestoreAll
// Purpose:	restore textures
// Output:	textures restored
// Return:	none
/////////////////////////////////////
PROTECTED void TextureRestoreAll()
{
	hTXT thisTxt;

	for(LISTPTR::iterator i = g_TXTLIST.begin(); i != g_TXTLIST.end(); ++i)
	{
		thisTxt = (hTXT)(*i);

		if(thisTxt && thisTxt->bFile)
		{
			_TextureLoad(thisTxt);
		}
	}
}