#include "gdix.h"
#include "gdix_i.h"

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hCAMERA CameraCreate(const float vEye[eMaxPt], angle yaw, angle pitch, angle roll)
{
	hCAMERA newCam = (hCAMERA)GFX_MALLOC(sizeof(gfxCamera));

	if(!newCam)
	{ ASSERT_MSG(0, "Unable to allocate new camera", "Error in CameraCreate"); return 0; }

	float *pVUp = (float*)newCam->vUp;
	pVUp[eX]=0;
	pVUp[eY]=1;
	pVUp[eZ]=0;

	newCam->vEye.x = vEye[eX];
	newCam->vEye.y = vEye[eY];
	newCam->vEye.z = vEye[eZ];

	CameraYawPitchRoll(newCam, yaw, pitch, roll);

	return newCam;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraSetScene(hCAMERA cam)
{
	D3DXMatrixLookAtRH(
	  &cam->mtxCamera,
	  &cam->vEye,
	  &cam->vTarget,
	  &cam->vUp
	);

	g_p3DDevice->SetTransform(D3DTS_VIEW, &cam->mtxCamera);

	//update proj. mtx
	D3DXMATRIX projMtx;
	g_p3DDevice->GetTransform(D3DTS_PROJECTION, &projMtx);
	g_projMtx = cam->mtxCamera*projMtx;

	//update the billboard to face this camera
	TextureBillboardPrepare();

	//calculate the frustrum planes
	FrustrumCalculate();

	//set the eye pt.
	g_eyePt = cam->vEye;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraMov(hCAMERA cam, const float amt[eMaxPt])
{
	cam->vEye.x += amt[eX];
	cam->vEye.y += amt[eY];
	cam->vEye.z += amt[eZ];

	cam->vTarget = cam->vDir+cam->vEye;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraSet(hCAMERA cam, const float vEye[eMaxPt])
{
	if(vEye)
	{
		cam->vEye.x = vEye[eX];
		cam->vEye.y = vEye[eY];
		cam->vEye.z = vEye[eZ];
	}

	//set up look-at
	cam->vTarget = cam->vDir+cam->vEye;
}

/////////////////////////////////////
// Name:	CameraSetTarget
// Purpose:	set camera target
// Output:	set the camera target
// Return:	none
/////////////////////////////////////
PUBLIC void CameraSetTarget(hCAMERA cam, const float vTarget[eMaxPt])
{
	if(vTarget)
	{
		cam->vTarget.x = vTarget[eX];
		cam->vTarget.y = vTarget[eY];
		cam->vTarget.z = vTarget[eZ];
	}

	//set up look-at
	cam->vDir = cam->vTarget-cam->vEye;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraMovDir(hCAMERA cam, float xAmt, float yAmt, float zAmt)
{
	float *eye = (float*)cam->vEye, *dir = (float*)cam->vDir;

	xAmt *= dir[eX];
	yAmt *= dir[eY];
	zAmt *= dir[eZ];

	eye[eX] += xAmt;
	eye[eY] += yAmt;
	eye[eZ] += zAmt;

	//set up look-at
	cam->vTarget = cam->vDir+cam->vEye;

	/*D3DXMATRIX mtxTran;
	D3DXMatrixTranslation(&mtxTran, -xAmt, -yAmt, -zAmt);
	D3DXMatrixMultiply(&cam->mtxCamera,&cam->mtxCamera,&mtxTran);*/
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraYawPitchRoll(hCAMERA cam, angle yaw, angle pitch, angle roll)
{
	cam->aYaw+=yaw; cam->aPitch+=pitch; cam->aRoll+=roll;

	float cosY,cosP,cosR, sinY,sinP,sinR;

	cosY = COS(cam->aYaw);   sinY = SIN(cam->aYaw);
	cosP = COS(cam->aPitch); sinP = SIN(cam->aPitch);
	cosR = COS(cam->aRoll);  sinR = SIN(cam->aRoll);

	//set up dir(fwd vect)
	float *dir = (float*)cam->vDir;

	dir[eX] = sinY*cosP;
	dir[eY] = sinP;
	dir[eZ] = cosP*-cosY;

	//set up look-at
	cam->vTarget = cam->vDir+cam->vEye;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraDestroy(hCAMERA *cam)
{
	if(*cam)
	{
		GFX_FREE(*cam);
		cam=0;
	}
}

/////////////////////////////////////
// Name:	CameraGetLoc
// Purpose:	grabs camera's eye loc
// Output:	the eye location
// Return:	none
/////////////////////////////////////
PUBLIC void CameraGetLoc(hCAMERA cam, float loc[eMaxPt])
{
	loc[eX] = cam->vEye.x;
	loc[eY] = cam->vEye.y;
	loc[eZ] = cam->vEye.z;
}

/////////////////////////////////////
// Name:	CameraGetDir
// Purpose:	grabs camera's direction
//			vector
// Output:	the direction vector
// Return:	none
/////////////////////////////////////
PUBLIC void CameraGetDir(hCAMERA cam, float dir[eMaxPt])
{
	dir[eX] = cam->vDir.x;
	dir[eY] = cam->vDir.y;
	dir[eZ] = cam->vDir.z;
}

/////////////////////////////////////
// Name:	CameraGetYawPitchRoll
// Purpose:	grabs camera's yaw/pitch/roll
// Output:	none
// Return:	none
/////////////////////////////////////
PUBLIC void CameraGetYawPitchRoll(hCAMERA cam, angle *yaw, angle *pitch, angle *roll)
{
	if(yaw)
		*yaw=cam->aYaw;
	if(pitch)
		*pitch=cam->aPitch;
	if(roll)
		*roll=cam->aRoll;
}