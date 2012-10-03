#include "tata_main.h"
#include "tata_world.h"

RETCODE World::EntityLoad_Light(hQBSP qbsp, const EntityParse & entityDat)
{
	GFXLIGHT newLight;
	memset(&newLight, 0, sizeof(GFXLIGHT));

	newLight.Type = D3DLIGHT_POINT;

	newLight.Attenuation0 = 1.0f;

	//get diffuse color
	sscanf(entityDat.GetVal("_color"), "%f %f %f", 
		&newLight.Diffuse.r, &newLight.Diffuse.g, &newLight.Diffuse.b);

	//get point
	//swap at the same time, the y and z
	sscanf(entityDat.GetVal("origin"), "%f %f %f", 
		&newLight.Position.x, &newLight.Position.z, &newLight.Position.y);

	newLight.Position.z *= -1;

	//get range
	sscanf(entityDat.GetVal("light"), "%f", &newLight.Range);

	LightCreate(&newLight);

	return RETCODE_SUCCESS;
}