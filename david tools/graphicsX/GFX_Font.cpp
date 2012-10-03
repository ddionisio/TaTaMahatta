#include "gdix.h"
#include "gdix_i.h"

//list of all fonts created (IDs)
list<hFNT>		g_fontIDList;

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void FontInvalidateAll()
{
	for(list<hFNT>::iterator i = g_fontIDList.begin(); i != g_fontIDList.end(); i++)
	{
		if((*i) && (*i)->pD3DFont)
		{
			(*i)->pD3DFont->InvalidateDeviceObjects();
		}
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void FontRestoreAll()
{
	for(list<hFNT>::iterator i = g_fontIDList.begin(); i != g_fontIDList.end(); i++)
	{
		if((*i) && (*i)->pD3DFont)
		{
			(*i)->pD3DFont->RestoreDeviceObjects();
		}
	}
}

PRIVATE HRESULT _FontInit(hFNT fnt)
{
	HRESULT hr = S_OK;

	fnt->pD3DFont = new CD3DFont( fnt->strFont, fnt->fontSize, fnt->fntFormat );

	if(FAILED(hr = fnt->pD3DFont->InitDeviceObjects(g_p3DDevice)))
	{ return hr; }

	if(FAILED(hr = fnt->pD3DFont->RestoreDeviceObjects()))
	{ return hr; }

	return hr;
}

PRIVATE void _FontDestroyData(hFNT fnt)
{
	fnt->pD3DFont->InvalidateDeviceObjects();
	fnt->pD3DFont->DeleteDeviceObjects();
	delete fnt->pD3DFont;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hFNT FontCreate(const char *typeName, unsigned int size, unsigned int fmtFlag)
{
	hFNT newFnt = (hFNT)GFX_MALLOC(sizeof(gfxFont));

	if(!newFnt)
	{ ASSERT_MSG(0, "Unable to allocate new font", "Error in FontCreate"); return 0; }

	lstrcpy( newFnt->strFont, _T(typeName) );

	newFnt->fontSize = size;
	newFnt->fntFormat = fmtFlag;

	newFnt->pD3DFont = new CD3DFont( newFnt->strFont, size, fmtFlag );

	if(FAILED(_FontInit(newFnt)))
	{ FontDestroy(&newFnt); return 0; }

	g_fontIDList.push_back(newFnt);
	
	return newFnt;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE FontResize(hFNT fnt, unsigned int size)
{
	_FontDestroyData(fnt);

	fnt->fontSize = size;

	if(FAILED(_FontInit(fnt)))
		return RETCODE_FAILURE;

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE FontPrintf2D(hFNT fnt, float x, float y, unsigned int color, const char *text, ...)
{
	char buff[MAXCHARBUFF];								// Holds Our String

	//do the printf thing
	va_list valist;
	va_start(valist, text);
	_vsnprintf(buff, sizeof(buff), text, valist);
	va_end(valist);

	if(FAILED(fnt->pD3DFont->DrawText(x, y, color, buff, D3DFONT_FILTERED)))
		return RETCODE_FAILURE;

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE FontPrintf3D(hFNT fnt, D3DXMATRIX *wrldMtx, GFXMATERIAL *mtrl, DWORD flags, const char *text, ...)
{
	char buff[MAXCHARBUFF];								// Holds Our String

	//do the printf thing
	va_list valist;
	va_start(valist, text);
	_vsnprintf(buff, sizeof(buff), text, valist);
	va_end(valist);

	g_p3DDevice->SetMaterial(mtrl);
    g_p3DDevice->SetTransform(D3DTS_WORLD, wrldMtx);
    
	if(FAILED(fnt->pD3DFont->Render3DText(buff, flags | D3DFONT_FILTERED)))
		return RETCODE_FAILURE;

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void FontDestroy(hFNT *fnt)
{
	assert(fnt);

	if(*fnt)
	{
		g_fontIDList.remove((*fnt));

		if((*fnt)->pD3DFont)
		{
			_FontDestroyData(*fnt);

			(*fnt)->pD3DFont = 0;
		}

		GFX_FREE(*fnt);
		fnt=0;
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void FontGetStrSize(hFNT fnt, char *str, SIZE *pSize)
{
	fnt->pD3DFont->GetTextExtent(str, pSize);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void FontGetCharSize(hFNT fnt, char c, SIZE *pSize)
{
	char foo[2] = {c,0};
	fnt->pD3DFont->GetTextExtent(foo, pSize);
}

/////////////////////////////////////
// Name:	FontTextBox
// Purpose:	display font in a box
// Output:	font blt in a box
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE FontTextBox(hFNT fnt, float sX, float sY, 
						   float eX, float eY, unsigned int color, 
						   const char *text)
{
	SIZE fntSize, cSize;
	SIZE boxSize={eX-sX,eY-sY};

	char word[MAXCHARBUFF]={0};
	char *pText = (char*)text;
	char retC;

	int ind;
	//ParserReadWordBuff(char **theBuff, char *buff, int buffSize, const char stopat)

	float curY = sY, curX = sX;

	while(*pText != 0 && curY < eY)
	{
		//get spaces before getting the word
		ind = 0;
		while(1)
		{
			if(pText[ind] == ' ')
			{
				FontGetCharSize(fnt, ' ', &cSize);
				curX += cSize.cx;
			}
			else if(pText[ind] == '\n')
			{
				//huh?
				FontGetCharSize(fnt, 'X', &cSize);
				curX = sX;
				curY += cSize.cy;
			}
			else
				break;

			ind++;
		}

		//this will skip the stuff we just went over above
		retC = ParserReadWordBuff(&pText, word, MAXCHARBUFF, 0);

		FontGetStrSize(fnt, word, &fntSize);

		//word doesn't even fit the box, just blt it and go to next line
		if(fntSize.cx > boxSize.cx)
		{
			curX = sX;
			curY += fntSize.cy;

			FontPrintf2D(fnt, curX, curY, color, word);
		}
		//wrap to the next line
		else if(curX + fntSize.cx > eX)
		{
			curY += fntSize.cy;

			FontPrintf2D(fnt, sX, curY, color, word);

			//add up the word
			curX = sX+fntSize.cx;
		}
		//safe in a line
		else
		{
			FontPrintf2D(fnt, curX, curY, color, word);

			//add up the word
			curX += fntSize.cx;
		}

		//increment x or y depending on retC
		if(retC == ' ')
		{
			FontGetCharSize(fnt, ' ', &cSize);
			curX += cSize.cx;
		}
		else if(retC == '\n')
		{
			//huh?
			FontGetCharSize(fnt, 'X', &cSize);
			curY += cSize.cy;
			curX = sX;
		}

		if(curX > eX)
		{
			curX = sX;
			curY += fntSize.cy;
		}
	}
	
	return RETCODE_SUCCESS;
}