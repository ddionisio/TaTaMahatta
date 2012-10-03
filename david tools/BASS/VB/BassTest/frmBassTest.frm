VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "comdlg32.ocx"
Begin VB.Form frmBassTest 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "BASS - Simple Test"
   ClientHeight    =   3735
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   5310
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   ScaleHeight     =   3735
   ScaleWidth      =   5310
   StartUpPosition =   2  'CenterScreen
   Begin VB.CommandButton cmdResumeAll 
      Caption         =   "Resume"
      Height          =   375
      Left            =   1680
      TabIndex        =   30
      Top             =   3240
      Width           =   855
   End
   Begin VB.CommandButton cmdStopAll 
      Caption         =   "Stop Everything"
      Height          =   375
      Left            =   120
      TabIndex        =   29
      Top             =   3240
      Width           =   1455
   End
   Begin VB.Timer tmrBass 
      Enabled         =   0   'False
      Interval        =   100
      Left            =   2640
      Top             =   2040
   End
   Begin VB.Frame Frame4 
      Caption         =   "CD"
      Height          =   975
      Left            =   2640
      TabIndex        =   17
      Top             =   2160
      Width           =   2535
      Begin VB.CheckBox chkCDInDrive 
         Caption         =   "CD?"
         Enabled         =   0   'False
         Height          =   315
         Left            =   120
         TabIndex        =   24
         Top             =   600
         Width           =   615
      End
      Begin VB.CommandButton cmdCDStop 
         Caption         =   "Stop"
         Height          =   300
         Left            =   800
         TabIndex        =   23
         Top             =   600
         Width           =   735
      End
      Begin VB.CommandButton cmdCDResume 
         Caption         =   "Resume"
         Height          =   300
         Left            =   1560
         TabIndex        =   22
         Top             =   600
         Width           =   855
      End
      Begin VB.CommandButton cmdCDPlay 
         Caption         =   "Play"
         Height          =   300
         Left            =   1560
         TabIndex        =   21
         Top             =   240
         Width           =   855
      End
      Begin VB.TextBox txtTrack 
         Height          =   285
         Left            =   600
         TabIndex        =   19
         Text            =   "1"
         Top             =   240
         Width           =   255
      End
      Begin VB.Label lblTime 
         AutoSize        =   -1  'True
         Caption         =   "0:00"
         Height          =   195
         Left            =   960
         TabIndex        =   20
         Top             =   275
         Width           =   315
      End
      Begin VB.Label Label1 
         AutoSize        =   -1  'True
         Caption         =   "Track"
         Height          =   195
         Left            =   120
         TabIndex        =   18
         Top             =   275
         Width           =   420
      End
   End
   Begin VB.Frame Frame3 
      Caption         =   "Stream"
      Height          =   975
      Left            =   120
      TabIndex        =   12
      Top             =   2160
      Width           =   2415
      Begin VB.CommandButton cmdStreamStop 
         Caption         =   "Stop"
         Height          =   300
         Left            =   1560
         TabIndex        =   16
         Top             =   600
         Width           =   735
      End
      Begin VB.CommandButton cmdStreamPlay 
         Caption         =   "Play"
         Height          =   300
         Left            =   840
         TabIndex        =   15
         Top             =   600
         Width           =   735
      End
      Begin VB.CommandButton cmdStreamNew 
         Caption         =   "New ..."
         Height          =   300
         Left            =   120
         TabIndex        =   14
         Top             =   600
         Width           =   735
      End
      Begin VB.TextBox txtStream 
         BackColor       =   &H80000000&
         Height          =   285
         Left            =   120
         Locked          =   -1  'True
         TabIndex        =   13
         Top             =   240
         Width           =   2175
      End
   End
   Begin VB.Frame Frame2 
      Caption         =   "Sample"
      Height          =   2055
      Left            =   2640
      TabIndex        =   7
      Top             =   0
      Width           =   2535
      Begin VB.CommandButton cmdSampleRemove 
         Caption         =   "Remove"
         Height          =   375
         Left            =   1320
         TabIndex        =   11
         Top             =   1560
         Width           =   1095
      End
      Begin VB.CommandButton cmdSampleAdd 
         Caption         =   "Add ..."
         Height          =   375
         Left            =   120
         TabIndex        =   10
         Top             =   1560
         Width           =   1095
      End
      Begin VB.ListBox lstSamples 
         Height          =   840
         Left            =   120
         TabIndex        =   9
         Top             =   240
         Width           =   2295
      End
      Begin VB.CommandButton cmdSamplePlay 
         Caption         =   "Play"
         Height          =   375
         Left            =   120
         TabIndex        =   8
         Top             =   1200
         Width           =   2295
      End
   End
   Begin MSComDlg.CommonDialog DLG 
      Left            =   2160
      Top             =   1920
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.Frame Frame1 
      Caption         =   "Music"
      Height          =   2055
      Left            =   120
      TabIndex        =   0
      Top             =   0
      Width           =   2415
      Begin VB.CommandButton cmdMusicRemove 
         Caption         =   "Remove"
         Height          =   375
         Left            =   1200
         TabIndex        =   6
         Top             =   1560
         Width           =   1095
      End
      Begin VB.CommandButton cmdMusicAdd 
         Caption         =   "Add ..."
         Height          =   375
         Left            =   120
         TabIndex        =   5
         Top             =   1560
         Width           =   1095
      End
      Begin VB.CommandButton cmdMusicRestart 
         Caption         =   "Restart"
         Height          =   375
         Left            =   1560
         TabIndex        =   4
         Top             =   1200
         Width           =   735
      End
      Begin VB.CommandButton cmdMusicStop 
         Caption         =   "Stop"
         Height          =   375
         Left            =   840
         TabIndex        =   3
         Top             =   1200
         Width           =   735
      End
      Begin VB.CommandButton cmdMusicPlay 
         Caption         =   "Play"
         Height          =   375
         Left            =   120
         TabIndex        =   2
         Top             =   1200
         Width           =   735
      End
      Begin VB.ListBox lstMusic 
         Height          =   840
         Left            =   120
         TabIndex        =   1
         Top             =   240
         Width           =   2175
      End
   End
   Begin VB.Label Label3 
      AutoSize        =   -1  'True
      BackStyle       =   0  'Transparent
      Caption         =   "Volume"
      Height          =   195
      Left            =   4560
      TabIndex        =   28
      Top             =   3240
      Width           =   525
   End
   Begin VB.Label Label2 
      AutoSize        =   -1  'True
      BackStyle       =   0  'Transparent
      Caption         =   "CPU%"
      Height          =   195
      Left            =   3960
      TabIndex        =   27
      Top             =   3240
      Width           =   450
   End
   Begin VB.Label lblVolume 
      AutoSize        =   -1  'True
      Caption         =   "100"
      Height          =   195
      Left            =   4680
      TabIndex        =   26
      Top             =   3450
      Width           =   270
   End
   Begin VB.Label lblCPU 
      Alignment       =   2  'Center
      AutoSize        =   -1  'True
      Caption         =   "0.0"
      Height          =   195
      Left            =   4005
      TabIndex        =   25
      Top             =   3450
      Width           =   240
   End
End
Attribute VB_Name = "frmBassTest"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'*************************************************************
'* BASS Simple test (rev .1), copyright (c) 1999 Adam Hoult. *
'* Updated: 2003 by JOBnik! [Arthur Aminov, ISRAEL]          *
'*************************************************************
Option Explicit

Dim STRM As Long            ' Stream Handle
Dim CDPlaying As Boolean    ' Is the CD Playing ??

' display error messages
Public Sub Error_(ByVal es As String)
    Call MsgBox(es & vbCrLf & "(error code: " & BASS_ErrorGetCode() & ")", vbExclamation, "Error")
End Sub

'Play the specified CD Track
Private Sub cmdCDPlay_Click()
    If BASS_CDPlay(Val(txtTrack.Text), BASSTRUE, BASSFALSE) = BASSFALSE Then
        Call Error_("Can't play CD")
    Else
        CDPlaying = True
    End If
End Sub

'Resume CD
Private Sub cmdCDResume_Click()
    If BASS_ChannelResume(CDCHANNEL) = True Then CDPlaying = True
End Sub

'Pause CD
Private Sub cmdCDStop_Click()
    Call BASS_ChannelPause(CDCHANNEL)
    CDPlaying = False
End Sub

Private Sub cmdMusicAdd_Click()
    On Error Resume Next
    DLG.FileName = ""
    DLG.CancelError = True
    DLG.flags = cdlOFNExplorer Or cdlOFNFileMustExist Or cdlOFNHideReadOnly
    DLG.Filter = "MOD Music Files (xm/mod/s3m/it/mtm)|*.xm;*.mod;*.s3m;*.it;*.mtm|All Files (*.*)|*.*|"
    DLG.ShowOpen
    
    'if cancel was pressed, exit the procedure
    If Err.Number = 32755 Then Exit Sub
    
    Dim ModHandle As Long
    
    ModHandle = BASS_MusicLoad(BASSFALSE, DLG.FileName, 0, 0, BASS_MUSIC_RAMP)
    lstMusic.AddItem Trim(Str(lstMusic.ListCount + 1)) & ". " & BASS_MusicGetNameString(ModHandle)
    
    If ModHandle = 0 Then
        Call Error_("Can't Load Music")
    Else
        lstMusic.ItemData(lstMusic.ListCount - 1) = ModHandle
    End If
End Sub

'play the selected song.
Private Sub cmdMusicPlay_Click()
    On Error GoTo E_Out
    If BASS_MusicPlay(lstMusic.ItemData(lstMusic.ListIndex)) = BASSFALSE Then
        Call Error_("Can't play music")
    End If
E_Out:
End Sub

'Free the selected mod resource
'Remove the selected list
Private Sub cmdMusicRemove_Click()
    On Error GoTo E_Out
    Call BASS_MusicFree(lstMusic.ItemData(lstMusic.ListIndex))
    lstMusic.RemoveItem lstMusic.ListIndex
E_Out:
End Sub

'Play the music from the start
Private Sub cmdMusicRestart_Click()
    On Error GoTo E_Out
    Call BASS_MusicPlayEx(lstMusic.ItemData(lstMusic.ListIndex), 0, -1, BASSTRUE)
E_Out:
End Sub

'Stop the currently selected music
Private Sub cmdMusicStop_Click()
    On Error GoTo E_Out
    If BASS_ChannelStop(lstMusic.ItemData(lstMusic.ListIndex)) = BASSFALSE Then
        Call Error_("Can't stop Music")
    End If
E_Out:
End Sub

'Resume digital output and CD
Private Sub cmdResumeAll_Click()
    If CDPlaying = True Then Call BASS_ChannelResume(CDCHANNEL)
    Call BASS_Start
End Sub

Private Sub cmdSampleAdd_Click()
    On Error Resume Next
    DLG.FileName = ""
    DLG.CancelError = True
    DLG.flags = cdlOFNExplorer Or cdlOFNFileMustExist Or cdlOFNHideReadOnly
    DLG.Filter = "WAVE sample files (*.wav)|*.wav|All Files (*.*)|*.*|"
    DLG.ShowOpen
    'if cancel was pressed, exit the procedure
    If Err.Number = 32755 Then Exit Sub
    
    Dim SampleHandle As Long
    
    lstSamples.AddItem DLG.FileName
    SampleHandle = BASS_SampleLoad(BASSFALSE, DLG.FileName, 0, 0, 3, BASS_SAMPLE_OVER_POS)
    
    If SampleHandle = 0 Then
        Call Error_("Can't Load Sample")
    Else
        lstSamples.ItemData(lstSamples.ListCount - 1) = SampleHandle
    End If
End Sub

'play the selected sample.
Private Sub cmdSamplePlay_Click()
    On Error GoTo E_Out
    If BASS_SamplePlayEx(lstSamples.ItemData(lstSamples.ListIndex), 0, -1, 50, Int((201 * Rnd) - 100), BASSFALSE) = BASSFALSE Then
        Call Error_("Can't play sample")
    End If
E_Out:
End Sub

'Free the selected sample resource
'Remove the selected list item
Private Sub cmdSampleRemove_Click()
    On Error GoTo E_Out
    Call BASS_SampleFree(lstSamples.ItemData(lstSamples.ListIndex))
    lstSamples.RemoveItem lstSamples.ListIndex
E_Out:
End Sub

'Pause digital output and CD
Private Sub cmdStopAll_Click()
    Call BASS_Pause
    Call BASS_ChannelPause(CDCHANNEL)
End Sub

Private Sub cmdStreamNew_Click()
    On Error Resume Next
    DLG.FileName = ""
    DLG.CancelError = True
    DLG.flags = cdlOFNExplorer Or cdlOFNFileMustExist Or cdlOFNHideReadOnly
    DLG.Filter = "Streamable Files (MP3/Wav)|*.mp3; *.wav|All Files (*.*)|*.*|"
    DLG.ShowOpen
    'if cancel was pressed, exit the procedure
    If Err.Number = 32755 Then Exit Sub
    
    txtStream.Text = DLG.FileName
    
    ' Free old stream (if any) and create new one
    Call BASS_StreamFree(STRM)
    
    Dim StreamHandle As Long
    
    StreamHandle = BASS_StreamCreateFile(BASSFALSE, DLG.FileName, 0, 0, 0)
    
    If StreamHandle = 0 Then
        Call Error_("Can't create stream")
    Else
        STRM = StreamHandle
    End If
End Sub

'Play stream, not flushed
Private Sub cmdStreamPlay_Click()
    If BASS_StreamPlay(STRM, BASSFALSE, 0) = BASSFALSE Then
        Call Error_("Can't play stream")
    End If
End Sub

'Stop the stream
Private Sub cmdStreamStop_Click()
    Call BASS_ChannelStop(STRM)
End Sub

Private Sub Form_Load()
    
    'change and set the current path
    'so it won't ever tell you that bass.dll is not found
    ChDrive App.Path
    ChDir App.Path
    
    'check if 'bass.dll' is exists
    If Not FileExists(RPP(App.Path) & "bass.dll") Then
        MsgBox "BASS.DLL does not exists", vbCritical, "BASS.DLL"
        End
    End If
    
    'Check that BASS 1.8 was loaded
    If BASS_GetStringVersion <> "1.8" Then
        Call Error_("BASS version 1.8 was not loaded")
        End
    End If
    
    'Initialize digital sound - default device, 44100hz, stereo, 16 bits
    If BASS_Init(-1, 44100, 0, Me.hWnd) = BASSFALSE Then
        Call Error_("Can't initialize digital sound system")
        End
    End If
    
    'Initialize CD
    If BASS_CDInit(Nothing, BASS_DEVICE_LEAVEVOL) = BASSFALSE Then
        Call Error_("Can't initialize CD system")
    End If
    
    'Start digital output
    If BASS_Start = BASSFALSE Then
        Call Error_("Can't start digital output")
    End If
    
    'Set the initial directory to the EXE directory
    DLG.InitDir = App.Path
    
    'Start the timer
    tmrBass.Enabled = True
End Sub

Private Sub Form_Unload(Cancel As Integer)
    'stop timer
    tmrBass.Enabled = False
    
    'Stop digital output
    Call BASS_Stop
    
    'Free the stream
    Call BASS_StreamFree(STRM)
    
    'It's not actually necessary to free the musics and samples
    'because they are automatically freed by BASS_Free
    
    'Free musics
    Dim i As Integer
    
    For i = 0 To lstMusic.ListCount - 1
        Call BASS_MusicFree(lstMusic.ItemData(i))
    Next i
    
    'Free samples
    For i = 0 To lstSamples.ListCount - 1
        Call BASS_SampleFree(lstSamples.ItemData(i))
    Next i
    
    'Close digital sound system
    Call BASS_Free
    
    'Close CD system
    Call BASS_CDFree
End Sub

'Main timer, to update all info needed.
Private Sub tmrBass_Timer()
    Dim p As Long
    
    ' update the CD status
    chkCDInDrive.value = BASS_CDInDrive
    
    If BASS_CDInDrive = 1 Then
        p = BASS_ChannelGetPosition(CDCHANNEL)
        lblTime.Caption = Int(p / 60000) & ":" & Right("00" & Int((p / 1000)), 2)
        Debug.Print Int((p / 1000))
    Else
        lblTime.Caption = "0:00"
    End If
    
    'update the CPU usage % display
    lblCPU.Caption = Format(BASS_GetCPU, "0.0")
    
    'update the volume level display
    lblVolume.Caption = BASS_GetVolume
End Sub

'-------------------------------------------------
'some useful functions :)
'-------------------------------------------------

'check if any file exists
Public Function FileExists(ByVal FileName As String) As Boolean
    On Local Error Resume Next
    FileExists = (Dir$(FileName) <> "")
End Function

' RPP = Return Proper Path
Public Function RPP(ByVal fp As String) As String
    RPP = IIf(Mid(fp, Len(fp), 1) = "\", fp, fp & "\")
End Function

'get file name from file path
Public Function GetFileName(ByVal fp As String) As String
    GetFileName = Mid(fp, InStrRev(fp, "\") + 1)
End Function
