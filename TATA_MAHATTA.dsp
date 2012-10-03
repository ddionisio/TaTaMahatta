# Microsoft Developer Studio Project File - Name="TATA_MAHATTA" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TATA_MAHATTA - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TATA_MAHATTA.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TATA_MAHATTA.mak" CFG="TATA_MAHATTA - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TATA_MAHATTA - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TATA_MAHATTA - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/", aaaaaaaa"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TATA_MAHATTA - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "david tools" /I "source" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib DXGUID.LIB /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "TATA_MAHATTA - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "david tools" /I "source" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib DXGUID.LIB /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "TATA_MAHATTA - Win32 Release"
# Name "TATA_MAHATTA - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\mywin.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_destroy.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_init.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_main.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_tool.cpp
# End Source File
# End Group
# Begin Group "Procs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\proc_credits.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\proc_game.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\proc_intro.cpp
# End Source File
# End Group
# Begin Group "Interfaces"

# PROP Default_Filter ""
# Begin Group "world"

# PROP Default_Filter ""
# Begin Group "load"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\tata_world_load.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_load_entity_block.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_load_entity_button.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_load_entity_enemy.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_load_entity_gas.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_load_entity_item.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_load_entity_lever.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_load_entity_light.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_load_entity_misc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_load_entity_platform.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_load_entity_sign.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_load_entity_start.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_load_entity_steak.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_load_entity_tata.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_load_entity_trigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_load_entity_waypoint.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Source\tata_world.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_ambientSound.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_cutscene.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_display.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_HUD.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_music.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_profile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_target.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world_update.cpp
# End Source File
# End Group
# Begin Group "creature"

# PROP Default_Filter ""
# Begin Group "Ta-Ta"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\creature_BabyTaTa.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_CaptainTaTa.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_ChiTa.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_FrostTa.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_HopTaHop.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_KeyKeyTa.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_LoopTaLoop.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_ParaTaTa.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_TaTaTrample.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_TaTaTug.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_TaVatar.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_TinkerTa.cpp
# End Source File
# End Group
# Begin Group "Enemy"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\boss_CatterShroom.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\boss_CorrupTa.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\boss_ShroomPa.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_FungaBark.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_FungaMusketeer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_FungaSmug.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_FungaSpy.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_ShroomGuard.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\creature_ShroomShooter.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Source\tata_creature.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_creature_status.cpp
# End Source File
# End Group
# Begin Group "object (level objects)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\object_block.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\object_button.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\object_doodad.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\object_geyser.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\object_goal.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\object_lever.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\object_lightningarea.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\object_ouchfield.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\object_platform.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\object_sign.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\object_steak.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\object_teleport.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\object_trigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\object_turret.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_object.cpp
# End Source File
# End Group
# Begin Group "projectiles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\projectile_attackmelee.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\projectile_frost.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\projectile_gas.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\projectile_openmelee.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\projectile_pullpower.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\projectile_pushmelee.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\projectile_scanner.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\projectile_spike.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\projectile_spit.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\projectile_teleport.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\projectile_usemelee.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\projectile_whip.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_projectile.cpp
# End Source File
# End Group
# Begin Group "item"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\tata_item.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Source\tata_basic.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_bound.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_collision.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_error.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_ID.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_input.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_proc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_sound.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_stage_select.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_view.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_waypoint.cpp
# End Source File
# End Group
# Begin Group "Script Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\script_creature.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\script_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\script_entity.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\script_image.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\script_object.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\script_target.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\script_tata.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\script_vector.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\script_view.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\script_world.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_script_query.cpp
# End Source File
# End Group
# Begin Group "Menu"

# PROP Default_Filter ""
# Begin Group "Menu Item"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\menu_item_button2D.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_item_button3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_item_buttonText.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_menu_item.cpp
# End Source File
# End Group
# Begin Group "Menu Callbacks"

# PROP Default_Filter ""
# Begin Group "tutorial"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\menu_callback_playtutorial.cpp
# End Source File
# End Group
# Begin Group "options"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\menu_callback_options.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_callback_options_audio.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_callback_options_gfx.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_callback_options_input.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Source\menu_callback_exit.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_callback_gameover.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_callback_jstick_cfg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_callback_kb_cfg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_callback_levelselect.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_callback_levelstart.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_callback_load.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_callback_mainmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_callback_new.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_callback_pause.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_callback_pause_ingame.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\menu_callback_victory.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Source\tata_menu.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_menu_cursor.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tata_menu_game.cpp
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Menu Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\tata_menu.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_menu_cursor.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_menu_game.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_menu_item.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_menu_item_common.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_menu_options.h
# End Source File
# End Group
# Begin Group "Creature Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\tata_creature.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_creature_enemy.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_creature_status.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_creature_tata.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Source\bass.h
# End Source File
# Begin Source File

SOURCE=".\david tools\common.h"
# End Source File
# Begin Source File

SOURCE=.\Source\mywin.h
# End Source File
# Begin Source File

SOURCE=.\Source\resource.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_basic.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_bound.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_collision.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_entityparse.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_globals.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_ID.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_item.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_main.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_object.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_object_common.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_projectiles.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_projectiles_common.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_script.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_sound.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_stage_select.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_types.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_view.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_waypoint.h
# End Source File
# Begin Source File

SOURCE=.\Source\tata_world.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Source\tata_win_resource.rc
# End Source File
# Begin Source File

SOURCE=.\Source\tataicon.ico
# End Source File
# Begin Source File

SOURCE=.\bass.lib
# End Source File
# End Group
# End Target
# End Project
