sky_1
{
	qer_editorimage textures/common/invisible.tga
	q3map_lightsubdivide 512
	q3map_globaltexture
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_sun .5 .6 .8 90 225 55
	q3map_surfacelight 150
	skyparms - 512 -
	{
		map textures/common/invisible.tga
		tcMod scale 3 2
		tcMod scroll 0.15 0.15
	}
	{
		map textures/common/invisible.tga
		blendFunc GL_ONE GL_ONE
		tcMod scale 3 3
		tcMod scroll 0.05 0.05
	}
}