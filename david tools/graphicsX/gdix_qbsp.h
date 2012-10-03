#ifndef _gdix_qbsp_h
#define _gdix_qbsp_h

#include "gdix_qbsp_flags.h"

//THIS IS FOR INTERNAL GFX USE ONLY!

//THIS IS THE QUAKE 3 BSP

//#define SHOW_INVISIBLE

#define QBSP_TXT_INVISIBLE		0x80000000

#define STR_INVISIBLE			"invisible"

//flags for the QBSP
#define QBSP_FLAG_VTXLIGHTING	1

//flags for the level model
#define QBSP_MODEL_FLAG_DISABLE	1

// Lump types
typedef enum
{
    eLumpEntities,				// objects and triggers...
    eLumpTextures,				// texture info (file, etc)
    eLumpPlanes,			    // planes for each leaf
    eLumpNodes,					// the BSP nodes
    eLumpLeafs,					// the leafs of BSP nodes
    eLumpLeafFaces,				// leaf index to faces
    eLumpLeafBrushes,			// leaf index to brushes
    eLumpModels,				// stuff about models (non-static objects)
    eLumpBrushes,				// collision stuff
    eLumpBrushSides,			// more collision stuff
    eLumpVertices,				// these are all the vertices
    eLumpMeshVerts,				// vertex indices
    eLumpShaders,				// shader fx (not used for now...)
    eLumpFaces,					// all the faces
    eLumpLightmaps,				// lightmaps for shadows and stuff
    eLumpLightVolumes,			// more light stuff (fog effect, etc)
    eLumpVisData,				// PVS and cluster for culling faces
    eNumLumps
} eLumps;

//face types
typedef enum
{
	eFacePolygon=1,		//regular old polygon
	eFacePatch,			//parametrized curve surface
	eFaceMesh,			//static object
	eFaceBillboard		//billboard
} eFaceType;

// This is our BSP header structure
typedef struct _BSPHeader
{
    char ID[4];				// This should always be 'IBSP'
    int ver;				// This should be 0x2e for Quake 3 files
} BSPHeader;

//This is our BSP brush side structure
typedef struct _BSPBrushSide
{
    int plane;              // The plane index
    int textureID;          // The texture index
} BSPBrushSide;

//This is our BSP brush structure
typedef struct _BSPBrush
{
    int brushSide;           // The starting brush side for the brush 
    int numOfBrushSides;     // Number of brush sides for the brush
    int textureID;           // The texture index for the brush
} BSPBrush;

//This is the BSP model structure
typedef struct _BSPModel 
{
    float min[eMaxPt];      // The min position for the bounding box
    float max[eMaxPt];      // The max position for the bounding box. 
    int faceIndex;          // The first face index in the model 
    int numOfFaces;         // The number of faces in the model 
    int brushIndex;         // The first brush index in the model 
    int numOfBrushes;       // The number brushes for the model
} BSPModel; 

// This is our BSP lump structure
typedef struct _BSPLump
{
	int offset;					// The offset into the file for the start of this lump
	int length;					// The length in bytes for this lump
} BSPLump;

// This stores a node in the BSP tree
typedef struct _BSPNode
{
    int plane;					// The index into the planes array 
    int front;					// The child index for the front node 
    int back;					// The child index for the back node 
    int min[eMaxPt];			// The bounding box min position. (int)
    int max[eMaxPt];			// The bounding box max position. (int)
} BSPNode;

// This stores a leaf (end node) in the BSP tree
typedef struct _BSPLeaf
{
    int cluster;				// The visibility cluster 
    int area;					// The area portal 
    int min[eMaxPt];			// The bounding box min position. (int)
    int max[eMaxPt];			// The bounding box max position. (int)
    int leafface;				// The first index into the face array 
    int numOfLeafFaces;			// The number of faces for this leaf 
    int leafBrush;				// The first index for into the brushes 
    int numOfLeafBrushes;		// The number of brushes for this leaf 
} BSPLeaf;

// This stores a splitter plane in the BSP tree
typedef struct _BSPPlane
{
    float a,b,c,d;
} BSPPlane;

// This stores the cluster data for the PVS's
typedef struct _BSPVisData
{
	int numOfClusters;			// The number of clusters
	int bytesPerCluster;		// The amount of bytes (8 bits) in the cluster's bitset
	BYTE *pBitsets;				// The array of bytes that holds the cluster bitsets				
} BSPVisData;

// This is our BSP lightmap structure which stores the 128x128 RGB values
typedef struct _BSPLightmap
{
    BYTE imageBits[128][128][3];   // The RGB data in a 128x128 image
} BSPLightmap;


// BSP vertex definition
typedef struct _BSPVertex
{
    float pos[eMaxPt];				//x,y,z
    float txtCoord[eMaxTxtPt];		//u,v
    float lightmapCoord[eMaxTxtPt];	//u,v
    float vNormal[eMaxPt];			//x,y,z
    BYTE  color[eMaxClr];			//r,g,b,a
} BSPVertex;


// This is our BSP face structure
typedef struct _BSPFace
{
    int textureID;				// The index into the texture array 
    int effect;					// The index for the effects (or -1 = n/a) 
    int type;					// 1=polygon, 2=patch, 3=mesh, 4=billboard 
    int startVertIndex;			// The starting index into this face's first vertex 
    int numOfVerts;				// The number of vertices for this face 
    int meshVertIndex;			// The index into the first meshvertex 
    int numMeshVerts;			// The number of mesh vertices 
    int lightmapID;				// The texture index for the lightmap 
    int lMapCorner[2];			// The face's lightmap corner in the image 
    int lMapSize[2];			// The size of the lightmap section 
    float lMapPos[eMaxPt];		// The 3D origin of lightmap. 
    float lMapVecs[2][eMaxPt];	// The 3D space for s and t unit vectors. 
    //float vNormal[eMaxPt];		// The face normal. 
	D3DXVECTOR3 vNormal;		//The face normal
    int size[2];				// The bezier patch dimensions. 
} BSPFace;


// This is our BSP texture structure
typedef struct _BSPTexture
{
    char strName[64];			// The name of the texture w/o the extension 
    unsigned int flags;			// The surface flags (unknown) 
    int contents;				// The content flags (unknown)
} BSPTexture;

//////////////////////////////////////////////////////////////////////
//OUR vertex definition for the map
typedef struct _gfxVtxQBSP {
    D3DXVECTOR3 pt; //vertex
	D3DXVECTOR3 n; //normal
	DWORD color;   //the color!
	float s, t;	   //texture coord
	float ls, lt;	   //lightmap coord
	float dls, dlt;	   //dynamic lightmap coord
} gfxVtxQBSP;

//OUR texture definition for the map
typedef struct _gfxTxtQBSP {
	hTXT	texture;			// texture buffer
	int		flags;				// The surface flags (unknown) 
    int		contents;			// The content flags (unknown)
} gfxTxtQBSP;

//OUR node definition
typedef struct _gfxBSPNode
{
    int plane;					// The index into the planes array
	int child[2];				// the front (0) and back (1) child
    float min[eMaxPt];			// The bounding box min position.
    float max[eMaxPt];			// The bounding box max position.
} gfxBSPNode;

//OUR leaf (end node) definition
typedef struct _gfxBSPLeaf
{
    int cluster;				// The visibility cluster 
    int area;					// The area portal 
    float min[eMaxPt];			// The bounding box min position.
    float max[eMaxPt];			// The bounding box max position.
	D3DXVECTOR3 loc;			// The center location of this leaf
    int leafface;				// The first index into the face array 
    int numOfLeafFaces;			// The number of faces for this leaf 
    int leafBrush;				// The first index for into the brushes 
    int numOfLeafBrushes;		// The number of brushes for this leaf 
} gfxBSPLeaf;

//OUR BSP model structure
typedef struct _ConvexPolyhedron {
	D3DXPLANE *planes;		// The group of planes used by this polyhedron. (numPlane)
							// This is the model space planes

	D3DXPLANE *tPlanes;		// This is similar to planes, but transformed

	int		  *txtID;		// The texture IDs (numPlane)
	int		  numPlane;
} ConvexPolyhedron;

typedef struct _gfxBSPModel 
{
    float min[eMaxPt];      // The min position for the bounding box
    float max[eMaxPt];      // The max position for the bounding box. 
	float radius;			// The radius of the model

    int faceIndex;          // The first face index in the model 
    int numOfFaces;         // The number of faces in the model 
    int brushIndex;         // The first brush index in the model 
    int numOfBrushes;       // The number brushes for the model

	ConvexPolyhedron *brushes; //The brushes this model uses (numOfBrushes)

	float        wrldTrans[eMaxPt];	 //world translation
	float		 wrldRot[eMaxPt];	 //world rotation (RADIANS: along x,y,z)

	D3DXMATRIX	 wrldMtx;    		 //world matrix

	FLAGS		 status;			 //model status
} gfxBSPModel; 
//////////////////////////////////////////////////////////////////////

#define GFXVERTEXQBSPFLAG (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX3)

//The GFX QBSP map structure
typedef struct _gfxQBSP {
	GFXVTXBUFF			   *vtx;		//all the map vertices
	BYTE				   *vtxClr;		//vertex colors [(r)(g)(b)(a)][...etc]
	int						numVtx;		//number of vertices

	BSPFace				   *faces;		//all the faces (numFaces)
	int					    numFaces;	//number of faces

	unsigned int		   *faceVis;	//is face visible?
	int						numFaceVis; //number of face vis

	unsigned int		   *brushVis;	 //has brush been processed yet?
	int						numBrushVis; //number of brush vis

	gfxTxtQBSP			   *textures;	//all the textures (numTxt)
	int						numTxt;		//number of textures

	hTXT				   *lightmaps;	 //special hTXT containing lightmap (128x128) (numLightMap)
	int						numLightMap; //number of light maps

	gfxBSPNode			   *nodes;		 //the nodes (numNodes)
	int						numNodes;

	gfxBSPLeaf			   *leaves;		 //edge nodes (numLeaves)
	int						numLeaves;	 //number of leaves

	D3DXPLANE			   *planes;		 //plane equations (numPlanes)
	int						numPlanes;	 //number of planes

	int					   *leafFaces;	 //leaf faces... (numLeafFaces)
	int						numLeafFaces;//number of leaf faces

	//unsigned short		   *meshVtxInd;	 //mesh vertex indices (numMeshVtxInd)
	GFXINDBUFF			   *meshIndBuff;//mesh vertex indices (numMeshVtxInd)
	int						numMeshVtxInd;//number of mesh vertex indices

	gfxBSPModel			   *models;		 //all non-static models (numModels)
	int						numModels;	 //number of models

	BSPBrush			   *brushes;	 //all the brushes for collision (numBrushes)
	int						numBrushes;	 //number of brushes

	int					   *leafBrushes; //The index into the brush array (numLeafBrushes)
	int						numLeafBrushes;//number of leaf brushes

	BSPBrushSide		   *brushSides;	 //Brush sides refered by brushes (numBrushSides)
	int						numBrushSides;//number of brush sides

	BSPVisData				clusters;	 //the PVS clusters

	D3DXMATRIX			    wrldMtx;	 //the world matrix! (DO NOT CHANGE!!! SHOULD ALWAYS BE IDENTITY!)

	FLAGS					status;		 //some stuff: vertex lighting/lightmap
} gfxQBSP;

#endif