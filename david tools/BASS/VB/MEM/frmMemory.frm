VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form frmMemory 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "JOBnik! - Playing Media File from Memory"
   ClientHeight    =   3300
   ClientLeft      =   45
   ClientTop       =   360
   ClientWidth     =   4215
   Icon            =   "frmMemory.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3300
   ScaleWidth      =   4215
   StartUpPosition =   2  'CenterScreen
   Begin VB.CheckBox chkSYNC 
      Caption         =   "SYNC @ END {will show an API MessageBox}"
      Height          =   255
      Left            =   120
      TabIndex        =   10
      Top             =   3000
      Width           =   3975
   End
   Begin VB.Frame Frame1 
      Height          =   3015
      Left            =   0
      TabIndex        =   0
      Top             =   -80
      Width           =   4215
      Begin VB.CommandButton cmdOpenPlay 
         Caption         =   "Click here to open a file  && play it"
         Height          =   495
         Left            =   120
         TabIndex        =   1
         Top             =   1440
         Width           =   3975
      End
      Begin VB.Timer tmrBASS 
         Enabled         =   0   'False
         Interval        =   1000
         Left            =   2880
         Top             =   840
      End
      Begin MSComDlg.CommonDialog cmd 
         Left            =   3480
         Top             =   840
         _ExtentX        =   847
         _ExtentY        =   847
         _Version        =   393216
      End
      Begin VB.Label lblFilePath 
         AutoSize        =   -1  'True
         Caption         =   "File:"
         Height          =   195
         Left            =   120
         TabIndex        =   9
         Top             =   240
         Width           =   285
      End
      Begin VB.Label lblDur 
         AutoSize        =   -1  'True
         Caption         =   "Total Duration: 0 seconds"
         Height          =   195
         Left            =   120
         TabIndex        =   8
         Top             =   600
         Width           =   1830
      End
      Begin VB.Label lblPos 
         AutoSize        =   -1  'True
         Caption         =   "Playing Position: 0 seconds"
         Height          =   195
         Left            =   120
         TabIndex        =   7
         Top             =   840
         Width           =   1935
      End
      Begin VB.Label lblMins 
         AutoSize        =   -1  'True
         Caption         =   "Time: 00:00:00"
         Height          =   195
         Left            =   120
         TabIndex        =   6
         Top             =   1080
         Width           =   1065
      End
      Begin VB.Label lblDXVer 
         AutoSize        =   -1  'True
         Caption         =   "DX Version:"
         Height          =   195
         Left            =   120
         TabIndex        =   5
         Top             =   2040
         Width           =   840
      End
      Begin VB.Label lblFreq 
         AutoSize        =   -1  'True
         Caption         =   "Frequency:"
         Height          =   195
         Left            =   120
         TabIndex        =   4
         Top             =   2280
         Width           =   795
      End
      Begin VB.Label lblBPS 
         AutoSize        =   -1  'True
         Caption         =   "Bytes/s:"
         Height          =   195
         Left            =   120
         TabIndex        =   3
         Top             =   2520
         Width           =   585
      End
      Begin VB.Label lblBitsPS 
         AutoSize        =   -1  'True
         Caption         =   "Kbp/s:"
         Height          =   195
         Left            =   120
         TabIndex        =   2
         Top             =   2760
         Width           =   480
      End
   End
End
Attribute VB_Name = "frmMemory"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'///////////////////////////////////////////////////////////////////
' BASS.DLL v1.6 Copyright (c) 2002 Ian Luck @ www.un4seen.com/music
' BASS.BAS v1.6 Copyright (c) 2002 Ian Luck @ www.un4seen.com/music
'
' [frmMemory.frm], [CBASS_TIME.cls] and [SYNCtest.bas]
'                      - Copyright (c) 2001-2002
'                                  JOBnik! [Arthur Aminov, ISRAEL]
'                                  e-mail: jobnik2k@hotmail.com
'
' * This is a VB6 Example of how to Play/Read a Data
'   from Allocated Memory Location with BASS Audio Library
'
' * Plus: There are Functions to Get Playing Position in Seconds
'         and Much MORE :) in CBASS_TIME.cls
'
' * Memory Xample based on Ian Lucks 'C' example!
'   (: of course with VB it's kinda Much Difficult to do :)
'///////////////////////////////////////////////////////////////////

'Memory allocation functions [Included only the used in project]
Const GMEM_FIXED = &H0
Private Declare Function GlobalAlloc Lib "kernel32" (ByVal wFlags As Long, ByVal dwBytes As Long) As Long
Private Declare Function GlobalFree Lib "kernel32" (ByVal hMem As Long) As Long
Private Declare Sub CopyMemory Lib "kernel32" Alias "RtlMoveMemory" (lpvDest As Any, lpvSource As Any, ByVal cbCopy As Long)
      
Dim SMHandle As Long    'Stream/Module Handle
Dim DataPtr As Long     'Data Pointer
Dim isMusic As Boolean  'coz we can't get the playing position in secs of music
Public MusicTimer As Long, SyncTimer As Long

Dim jbk_Timing As CBASS_TIME  'Class module Handle

Private Sub Form_Initialize()
  
    'change and set the current path
    'so it won't ever tell you that bass.dll isn't found
    ChDrive App.Path
    ChDir App.Path

    'check if "bass.dll" is exists
    If FileExists(RPP(App.Path) & "bass.dll") = False Then
        'MsgBox "BASS.DLL does not exists", vbCritical, "BASS.DLL"
        'End
    End If

    'Check that BASS 1.6 was loaded
    If BASS_GetStringVersion <> "1.6" Then
        MsgBox "BASS version 1.6 was not loaded", vbCritical, "Bass.Dll"
        End
    End If

    'Start digital output
    If (BASS_Init(-1, 44100, 0, Me.hwnd) = 0) Then
        MsgBox "Error: Couldn't Initialize Digital Output", vbCritical, "Digital output"
        End
    Else
        BASS_Start
    End If

    Set jbk_Timing = New CBASS_TIME
    
    lblDXVer.Caption = "DX Version: " & jbk_Timing.GetDXver
  
End Sub

Private Sub cmdOpenPlay_Click()
    On Error Resume Next          'if Cancel was pressed

    cmd.FileName = ""
    cmd.CancelError = True
    cmd.Flags = cdlOFNExplorer Or cdlOFNFileMustExist Or cdlOFNHideReadOnly
    cmd.Filter = "Playable files (all bass supported files)|*.mo3;*.xm;*.mod;*.s3m;*.it;*.mtm;*.mp1;*.mp2;*.mp3;*.wav;*.ogg|All files (*.*)|*.*"
    cmd.ShowOpen

    'if cancel was pressed, exit the procedure
    If Err.Number = 32755 Then Exit Sub
    
    tmrBASS.Enabled = False
    
    lblFilePath.Caption = "File: " & Mid(cmd.FileName, InStrRev(cmd.FileName, "\") + 1)

    'Free old stream (if any) and create new one
    Call BASS_StreamFree(SMHandle)
    Call BASS_MusicFree(SMHandle)
    
    'Free Memory
    Call GlobalFree(ByVal DataPtr)

    isMusic = False
    MusicTimer = 0
    
    If Not MAllocate(cmd.FileName) Then Exit Sub

    With jbk_Timing
        lblDur.Caption = "Total Duration: " & .GetDuration(SMHandle) & " seconds / " & .GetTime(.GetDuration(SMHandle))
        lblFreq.Caption = "Frequency: " & .GetFrequency(SMHandle) & " Hz, " & .GetBits(SMHandle) & " bits, " & .GetMode(SMHandle)
        lblBPS.Caption = "Bytes/s: " & .GetBytesPerSecond(SMHandle)
        lblBitsPS.Caption = "Kbp/s: " & .GetBitsPerSecond(SMHandle, FileLen(cmd.FileName)) & " [average kbp/s for vbr mp3s]"
    End With

    Call chkSYNC_Click
    
    'setup the MusicTimer
    SyncTimer = BASS_ChannelSetSync(SMHandle, BASS_SYNC_END, 0, AddressOf SYNCtest.SetMusicTimer, 1)
    
    Call BASS_MusicPlay(SMHandle)
    Call BASS_StreamPlay(SMHandle, 0, BASS_SAMPLE_LOOP)

    tmrBASS.Enabled = True
End Sub

Private Sub chkSYNC_Click()
    If chkSYNC.Value = vbChecked Then
        SyncEnd = BASS_ChannelSetSync(SMHandle, BASS_SYNC_END, 0, AddressOf SYNCtest.Stest, 1)
    Else
        Call BASS_ChannelRemoveSync(SMHandle, SyncEnd)
    End If
End Sub

Public Function MAllocate(ByVal FilePath As String) As Boolean

    Dim DataLength As Long      'file length
    Dim DataStore() As Byte     'data array
  
    DataLength = FileLen(FilePath)
    
    If DataLength = 0 Then
        MAllocate = False
        Exit Function
    End If
    
    'Allocate a Pointer to Memory
    DataPtr = GlobalAlloc(GMEM_FIXED, DataLength)
  
    Open FilePath For Binary Lock Read Write As #100
    ReDim DataStore(DataLength) As Byte
    
    'Insert All The File Data into a Byte Array
    Get 100, 1, DataStore
   
    'copy file into allocated memory location
    Call CopyMemory(ByVal DataPtr, DataStore(0), DataLength)
  
    Close #100
  
    'read data from memory location
    SMHandle = BASS_StreamCreateFile(BASSTRUE, DataPtr, 0, DataLength, 0)
    
    If SMHandle = 0 Then
        SMHandle = BASS_MusicLoad(BASSTRUE, DataPtr, 0, DataLength, BASS_MUSIC_LOOP Or BASS_MUSIC_RAMP Or BASS_MUSIC_CALCLEN)
        If SMHandle <> 0 Then
            isMusic = True
            MusicTimer = Timer
        End If
    End If
 
    If SMHandle = 0 Then
        'free stream and music (if any)
        Call BASS_StreamFree(SMHandle)
        Call BASS_MusicFree(SMHandle)
    
        ' free memory
        Call GlobalFree(ByVal DataPtr)
        
        MsgBox "Error: Couldn't Get Data from Memory", vbCritical, "Memory Data error..."
        
        MAllocate = False
        Exit Function
    End If
    
    MAllocate = True
    
End Function

Private Sub Form_Unload(Cancel As Integer)
    Call StopAll
End Sub

Public Sub StopAll()
    Call BASS_Stop
    Call BASS_StreamFree(SMHandle)
    Call BASS_MusicFree(SMHandle)
    Call BASS_ChannelRemoveSync(SMHandle, SyncEnd)
    Call BASS_ChannelRemoveSync(SMHandle, SyncTimer)
    Call BASS_Free
    'free data from allocate memory
    Call GlobalFree(ByVal DataPtr)
    'free Class Module
    Set jbk_Timing = Nothing
End Sub

Private Sub tmrBASS_Timer()
    With jbk_Timing
        If Not isMusic Then
            lblPos.Caption = "Playing Position: " & .GetPlayingPos(SMHandle) & " seconds"
            lblMins.Caption = "Time: " & .GetTime(.GetDuration(SMHandle) - .GetPlayingPos(SMHandle))
        Else
            lblPos.Caption = "Playing Position: " & Math.Round(Timer - MusicTimer)
            lblMins.Caption = "Time: " & .GetTime(CLng(.GetDuration(SMHandle) - Math.Round(Timer - MusicTimer)))
        End If
    End With
End Sub

'check if any file exists
Function FileExists(ByVal FileName As String) As Boolean
    On Local Error Resume Next
    FileExists = (Dir$(FileName) <> "")
End Function

' RPP = Return Proper Path
Function RPP(ByVal fp As String) As String
    RPP = IIf(Mid(fp, Len(fp), 1) <> "\", fp & "\", fp)
End Function
