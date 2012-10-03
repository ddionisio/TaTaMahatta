VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form frmFXtest 
   BorderStyle     =   4  'Fixed ToolWindow
   Caption         =   "BASS DX8 effects test for Visual Basic"
   ClientHeight    =   1965
   ClientLeft      =   45
   ClientTop       =   285
   ClientWidth     =   4365
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1965
   ScaleWidth      =   4365
   StartUpPosition =   2  'CenterScreen
   Begin MSComDlg.CommonDialog CMD 
      Left            =   3720
      Top             =   2160
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.CommandButton cmdOpenFP 
      Caption         =   "click here to open a file..."
      Height          =   495
      Left            =   120
      Style           =   1  'Graphical
      TabIndex        =   4
      Top             =   120
      Width           =   4215
   End
   Begin VB.VScrollBar VScroll1 
      Height          =   855
      Index           =   3
      Left            =   3240
      Max             =   20
      TabIndex        =   3
      Top             =   720
      Value           =   10
      Width           =   735
   End
   Begin VB.VScrollBar VScroll1 
      Height          =   855
      Index           =   2
      Left            =   2040
      Max             =   20
      TabIndex        =   2
      Top             =   720
      Value           =   10
      Width           =   735
   End
   Begin VB.VScrollBar VScroll1 
      Height          =   855
      Index           =   1
      Left            =   1200
      Max             =   20
      TabIndex        =   1
      Top             =   720
      Value           =   10
      Width           =   735
   End
   Begin VB.VScrollBar VScroll1 
      Height          =   855
      Index           =   0
      Left            =   360
      Max             =   20
      TabIndex        =   0
      Top             =   720
      Value           =   10
      Width           =   735
   End
   Begin VB.Label Label4 
      AutoSize        =   -1  'True
      Caption         =   "reverb"
      Height          =   195
      Left            =   3360
      TabIndex        =   8
      Top             =   1680
      Width           =   450
   End
   Begin VB.Label Label3 
      AutoSize        =   -1  'True
      Caption         =   "8 khz"
      Height          =   195
      Left            =   2160
      TabIndex        =   7
      Top             =   1680
      Width           =   480
   End
   Begin VB.Label Label2 
      AutoSize        =   -1  'True
      Caption         =   "1 khz"
      Height          =   195
      Left            =   1320
      TabIndex        =   6
      Top             =   1680
      Width           =   390
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      Caption         =   "125 hz"
      Height          =   195
      Left            =   480
      TabIndex        =   5
      Top             =   1680
      Width           =   480
   End
End
Attribute VB_Name = "frmFXtest"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'//////////////////////////////////////////////////////////////
' BASS.DLL v1.1 Copyright (c) Ian Luck www.un4seen.com/music
' BASS.BAS v1.1 Copyright (c) Ian Luck www.un4seen.com/music
'
' frmFXtest.frm - VB6 EE Version - Copyright (c) 2001
'                                    JOBnik! [Arthur Aminov]
'                                     jobnik2k@hotmail.com
' Originaly Translated from - fxtest.c -
'
' Include: 3 Band EQ + Reverb / VScroll.Max = 20 Min = 0 (dB)
'//////////////////////////////////////////////////////////////

Dim CHAN As Long         ' channel handle
Dim fx(3) As Long        ' 3 EQ band + reverb

Const ID_OPEN = 10
Const ID_C0 = 20
Const ID_C1 = 21
Const ID_C2 = 22
Const ID_C3 = 23

Public Sub UpdateFX(ByVal b As Integer)

  Dim v As Integer
  
  v = VScroll1(b).Value
  
  Select Case b
   Case 0, 1, 2
     Dim p As BASS_FXPARAMEQ
       Call BASS_FXGetParameters(fx(b), p)
       p.fGain = 10 - v
       Call BASS_FXSetParameters(fx(b), p)
   Case 3
     Dim p1 As BASS_FXREVERB
       Call BASS_FXGetParameters(fx(b), p1)
       p1.fReverbMix = -0.012 * v * v * v
       Call BASS_FXSetParameters(fx(b), p1)
  End Select
  
End Sub


Private Sub cmdOpenFP_Click()
  On Error Resume Next
  
  CMD.FileName = ""
  CMD.CancelError = True
  CMD.flags = cdlOFNExplorer Or cdlOFNFileMustExist Or cdlOFNHideReadOnly
  CMD.Filter = "Playable files (*.mo3;*.xm;*.mod;*.s3m;*.it;*.mtm;*.mp3;*.wav)|*.mo3;*.xm;*.mod;*.s3m;*.it;*.mtm;*.mp3;*.wav"
  CMD.ShowOpen
  
  'if cancel was pressed, exit the procedure
  If Err.Number = 32755 Then Exit Sub
  
  cmdOpenFP.Caption = CMD.FileName
  
  Dim fp As String
  
  fp = CMD.FileName
  
  'Free old stream (if any) and create new one
  BASS_StreamFree CHAN
  BASS_MusicFree CHAN
  
  CHAN = BASS_StreamCreateFile(BASSFALSE, fp, 0, 0, BASS_SAMPLE_FX)
  If CHAN = 0 Then CHAN = BASS_MusicLoad(BASSFALSE, fp, 0, 0, BASS_MUSIC_LOOP Or BASS_MUSIC_RAMP Or BASS_MUSIC_FX)
    
    'setup the effects
    
     Dim p As BASS_FXPARAMEQ
    
     fx(0) = BASS_ChannelSetFX(CHAN, BASS_FX_PARAMEQ) 'bass
     fx(1) = BASS_ChannelSetFX(CHAN, BASS_FX_PARAMEQ) 'mid
     fx(2) = BASS_ChannelSetFX(CHAN, BASS_FX_PARAMEQ) 'treble
     fx(3) = BASS_ChannelSetFX(CHAN, BASS_FX_REVERB)  'reverb
     
     p.fGain = 0
     p.fBandwidth = 18
     p.fCenter = 125                     ' bass   [125hz]
     Call BASS_FXSetParameters(fx(0), p)
     p.fCenter = 1000                    ' mid    [1khz]
     Call BASS_FXSetParameters(fx(1), p)
     p.fCenter = 8000                   ' treble [8khz]
     Call BASS_FXSetParameters(fx(2), p)
     
     ' you can add more EQ bands with changing:
     ' p.fCenter = N [hz] N>=80 and N<=16000
     
     Call UpdateFX(0) ' bass
     Call UpdateFX(1) ' mid
     Call UpdateFX(2) ' treble
     Call UpdateFX(3) ' reverb
     
     Call BASS_MusicPlay(CHAN)
     Call BASS_StreamPlay(CHAN, 0, BASS_SAMPLE_LOOP)
End Sub


Private Sub Form_Initialize()
  Dim InitBass As Boolean
  
  'change and set the current path
  'so it won't ever tell you that bass.dll isn't found
  
  ChDrive App.Path
  ChDir App.Path
  
  'check if bass.dll is exists
  
  If FileExists(RPP(App.Path) & "bass.dll") = False Then
    MsgBox "BASS.DLL not exists", vbCritical, "BASS.DLL"
    End
  End If
  
  'Check that BASS 1.6 was loaded
  If BASS_GetStringVersion <> "1.6" Then
    MsgBox "BASS version 1.6 was not loaded", vbCritical, "Bass.Dll"
    End
  End If
  
  InitBass = BASS_Init(-1, 44100, 0, Me.hWnd)

  ' Start digital output
  If InitBass Then
    BASS_Start
  Else
    MsgBox "Error: Couldn't Initialize Digital Output", vbCritical, "Digital output"
    End
  End If
  
  'check if DX8 features are available
  Dim bi As BASS_INFO
  bi.size = LenB(bi)      'LenB(..) returns a byte data
  Call BASS_GetInfo(bi)
  If (bi.dsver < 8) Then
    BASS_Free
    MsgBox "DirectX version 8 is not Installed!!!", vbCritical, "DX8"
    End
  End If
    
  Dim i As Integer
  
  For i = VScroll1.lBound To VScroll1.UBound - 1
    VScroll1(i).Value = 10 'eq
  Next i
  
  VScroll1(3).Value = 20 'reverb

End Sub

Private Sub Form_Unload(Cancel As Integer)
  BASS_Stop
  BASS_StreamFree CHAN
  BASS_MusicFree CHAN
  BASS_Free
End Sub

Private Sub VScroll1_Change(Index As Integer)
  Call UpdateFX(Index)
End Sub

Private Sub VScroll1_scroll(Index As Integer)
  Call UpdateFX(Index)
End Sub

'check if any file exists
Function FileExists(FileName) As Boolean
  On Local Error Resume Next
  FileExists = (Dir$(FileName) <> "")
End Function

' RPP = Return Proper Path
Function RPP(ByVal fp As String) As String
  If Right(fp, 1) <> "\" Then
    RPP = fp & "\"
  Else
    RPP = fp
  End If
End Function

