# Microsoft Developer Studio Project File - Name="graphicsX" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=graphicsX - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "graphicsX.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "graphicsX.mak" CFG="graphicsX - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "graphicsX - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "graphicsX - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Rotten/graphicsX", VAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "graphicsX - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "graphicsX - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "graphicsX - Win32 Release"
# Name "graphicsX - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ParticleFX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GFX_ParticleFX.cpp
# End Source File
# Begin Source File

SOURCE=.\PARFX_3DExplode.cpp
# End Source File
# Begin Source File

SOURCE=.\PARFX_expand.cpp
# End Source File
# Begin Source File

SOURCE=.\PARFX_explode.cpp
# End Source File
# Begin Source File

SOURCE=.\PARFX_gas.cpp
# End Source File
# Begin Source File

SOURCE=.\PARFX_gather.cpp
# End Source File
# Begin Source File

SOURCE=.\PARFX_glow.cpp
# End Source File
# Begin Source File

SOURCE=.\PARFX_lightningY.cpp
# End Source File
# Begin Source File

SOURCE=.\PARFX_puff.cpp
# End Source File
# Begin Source File

SOURCE=.\PARFX_smoke.cpp
# End Source File
# End Group
# Begin Group "Model"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GFX_joints.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Model.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_ModelFX.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_ModelGenMap.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_ModelLoad_MD2.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_ModelLoad_MS3D.cpp
# End Source File
# End Group
# Begin Group "Object"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GFX_Object.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_ObjectLoad.cpp
# End Source File
# End Group
# Begin Group "Map"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GFX_Map.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_MapGenMidptFrac.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_QBSPCollision.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_QBSPDestroy.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_QBSPLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_QBSPModel.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_QBSPRender.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_QBSPTool.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_QBSPVisBit.cpp
# End Source File
# End Group
# Begin Group "Texture"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GFX_Surface.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Texture.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_TextureDisplay.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\GFX_BkgrndFX.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_ErrorCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Fog.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Font.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Frustrum.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Light.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Main.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Math.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Misc.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Pager.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Primitives.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Screen.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_SkyBox.cpp
# End Source File
# Begin Source File

SOURCE=.\GFX_Sprite.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "d3d"

# PROP Default_Filter ""
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3d9.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3d9types.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3dx9.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3dx9core.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3dx9mesh.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3dx9tex.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3dxmath.h"
# End Source File
# End Group
# Begin Group "qbsp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gdix_qbsp.h
# End Source File
# Begin Source File

SOURCE=.\gdix_qbsp_i.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\gdiX.h
# End Source File
# Begin Source File

SOURCE=.\gdiX_bkgrndFX.h
# End Source File
# Begin Source File

SOURCE=.\gdix_i.h
# End Source File
# Begin Source File

SOURCE=.\gdiX_joints_i.h
# End Source File
# Begin Source File

SOURCE=.\gdiX_mesh_i.h
# End Source File
# Begin Source File

SOURCE=.\gdiX_modelFX_i.h
# End Source File
# Begin Source File

SOURCE=.\gdiX_particleFX.h
# End Source File
# Begin Source File

SOURCE=.\gdiX_particleFX_i.h
# End Source File
# Begin Source File

SOURCE=.\gdiX_qbsp_flags.h
# End Source File
# Begin Source File

SOURCE=.\gdiX_screen.h
# End Source File
# Begin Source File

SOURCE=.\gdiX_types.h
# End Source File
# Begin Source File

SOURCE=.\normTbl.h
# End Source File
# End Group
# Begin Group "d3dstuff"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\d3dstuff\d3dfont.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dstuff\d3dfont.h
# End Source File
# Begin Source File

SOURCE=.\d3dstuff\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dstuff\ddutil.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dstuff\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dstuff\dxutil.h
# End Source File
# End Group
# End Target
# End Project
