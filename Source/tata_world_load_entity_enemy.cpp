#include "tata_main.h"
#include "tata_world.h"

#include "tata_creature_enemy.h"

RETCODE World::EntityLoad_Enemy(hQBSP qbsp, const EntityParse & entityDat)
{
	Creature *pNewCre = 0;

	const char * pStr, *pTypeStr;

	//get type of enemy
	pTypeStr = entityDat.GetVal("Type");

	if(pTypeStr)
	{
		int enemyType = EnemyGetType(pTypeStr);

		switch(enemyType)
		{
		case ENEMY_FUNGABARK:
			pNewCre = new FungaBark;
			break;
		case ENEMY_SHROOMGUARD:
			pNewCre = new ShroomGuard;
			break;
		case ENEMY_SHROOMSHOOTER:
			pNewCre = new ShroomShooter;
			break;
		case ENEMY_FUNGASMUG:
			pNewCre = new FungaSmug;
			break;
		case ENEMY_FUNGAMUSKETEER:
			pNewCre = new FungaMusketeer;
			break;
		case ENEMY_FUNGASPY:
			{
				pNewCre = new FungaSpy;

				FungaSpy *pFS = (FungaSpy *)pNewCre;

				//get the range for scan
				pStr = entityDat.GetVal("scanAngle");
				if(pStr)
				{
					float a;
					sscanf(pStr, "%f", &a);
					pFS->SetAngleScan(D3DXToRadian(a));
				}

				//get the range for sight
				pStr = entityDat.GetVal("scanLength");
				if(pStr)
				{
					float l;
					sscanf(pStr, "%f", &l);
					pFS->SetLengthScan(l);
				}

				//get the scan delay
				pStr = entityDat.GetVal("scanDelay");
				if(pStr)
				{
					double d;
					sscanf(pStr, "%lf", &d);
					pFS->SetScanDelay(d);
				}

				//get the scan rotation spd
				pStr = entityDat.GetVal("scanSpd");
				if(pStr)
				{
					float spd;
					sscanf(pStr, "%f", &spd);
					pFS->SetRotSpd(spd);
				}
			}
			break;

		case BOSS_SHROOMPA:
			pNewCre = new ShroomPa;
			break;

		case BOSS_CATTERSHROOM:
			pNewCre = new CatterShroom;
			break;

		case BOSS_CORRUPTA:
			pNewCre = new CorrupTa;
			break;
		}
	}

	if(pNewCre)
	{
		//get name
		pStr = entityDat.GetVal("ID");

		if(pStr && pStr[0] != 0)
		{
			EntityCommon *pEntity = (EntityCommon *)IDPageQuery(CollisionGetEntity(pStr));

			/*if(pEntity)
			{
				char buff[MAXCHARBUFF]={0};
				sprintf(buff, "%s_%d_%d", pStr, pNewCre->GetID().ID, pNewCre->GetID().counter);
				pNewCre->SetName(buff);
			}
			else*/
				pNewCre->SetName(pStr);
		}
		else
		{
			char buff[MAXCHARBUFF]={0};
			sprintf(buff, "enemy_%d_%d", pNewCre->GetID().ID, pNewCre->GetID().counter);
			pNewCre->SetName(buff);
		}

		//load the components
		pNewCre->Load(pTypeStr);

		/////////////////////////////////
		//set location
		D3DXVECTOR3 theLoc;

		sscanf(entityDat.GetVal("origin"), "%f %f %f", 
			&theLoc.x, &theLoc.z, &theLoc.y);

		theLoc.z *= -1;

		pNewCre->SetLoc(theLoc);
		pNewCre->SetSpawnLoc(theLoc);

		/////////////////////////////////
		//set orientation
		pStr = entityDat.GetVal("angle");
		if(pStr)
		{
			D3DXMATRIX mtx;
			D3DXVECTOR3 dir(1,0,0);
			float angle;
			sscanf(pStr, "%f", &angle);
			
			angle = D3DXToRadian(angle);

			D3DXMatrixRotationY(&mtx, angle);
			D3DXVec3TransformNormal(&dir, &dir, &mtx);

			pNewCre->SetDir(dir);
		}

		/////////////////////////////////
		//set waypoint
		pStr = entityDat.GetVal("waypointID");
		if(pStr)
			pNewCre->SetWayPt(WaypointGet(pStr));

		WayptNode  *pWayPt = pNewCre->GetWayPoint();

		if(pWayPt)
		{
			///////////////////////////////////////////////////////
			//get waypoint mode
			pStr = entityDat.GetVal("WPMode");
			if(pStr)
				pWayPt->SetMode(WaypointGetMode(pStr));
		}
	}

	return RETCODE_SUCCESS;
}