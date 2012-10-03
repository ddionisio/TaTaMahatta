VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "comdlg32.ocx"
Begin VB.Form frmFXtest 
   BorderStyle     =   4  'Fixed ToolWindow
   Caption         =   "BASS DX8 effects test"
   ClientHeight    =   1965
   ClientLeft      =   45
   ClientTop       =   285
   ClientWidth     =   4455
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1965
   ScaleWidth      =   4455
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
   Begin VB.VScrollBar vsEQR 
      Height          =   855
      Index           =   3
      Left            =   3240
      Max             =   20
      TabIndex        =   3
      Top             =   720
      Value           =   20
      Width           =   735
   End
   Begin VB.VScrollBar vsEQR 
      Height          =   855
      Index           =   2
      Left            =   2040
      Max             =   20
      TabIndex        =   2
      Top             =   720
      Value           =   10
      Width           =   735
   End
   Begin VB.VScrollBar vsEQR 
      Height          =   855
      Index           =   1
      Left            =   1200
      Max             =   20
      TabIndex        =   1
      Top             =   720
      Value           =   10
      Width           =   735
   End
   Begin VB.VScrollBar vsEQR 
      Height          =   855
      Index           =   0
      Left            =   360
      Max             =   20
      TabIndex        =   0
      Top             =   720
      Value           =   10
      Width           =   735
   End
   Begin VB.Label lblEQR 
      AutoSize        =   -1  'True
      Caption         =   "reverb"
      Height          =   195
      Index           =   3
      Left            =   3360
      TabIndex        =   8
      Top             =   1680
      Width           =   450
   End
   Begin VB.Label lblEQR 
      AutoSize        =   -1  'True
      Caption         =   "8 khz"
      Height          =   195
      Index           =   2
      Left            =   2160
      TabIndex        =   7
      Top             =   1680
      Width           =   480
   End
   Begin VB.Label lblEQR 
      AutoSize        =   -1  'True
      Caption         =   "1 khz"
      Height          =   195
      Index           =   1
      Left            =   1320
      TabIndex        =   6
      Top             =   1680
      Width           =   390
   End
   Begin VB.Label lblEQR 
      AutoSize        =   -1  'True
      Caption         =   "125 hz"
      Height          =   195
      Index           =   0
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
'/////////////////////////////////////////////////////////////////////
' frmFXtest.frm - Copyright (c) 2001  JOBnik! [Arthur Aminov, ISRAEL]
'                                     e-mail: jobnik2k@hotmail.com
'
' Originaly Translated from - fxtest.c - example of Ian Luck
'
' Include: 3 Band EQ + Reverb
'/////////////////////////////////////////////////////////////////////

Dim chan As Long         ' channel handle
Dim fx(3) As Long        ' 3 EQ band + reverb

Private Sub Form_Initialize()
    'change and set the current path
    'so it won't ever tell you that bass.dll isn't found
    ChDrive App.Path
    ChDir App.Path
    
    'check if bass.dll is exists
    If Not FileExists(RPP(App.Path) & "bass.dll") Then
        Call MsgBox("BASS.DLL not exists", vbCritical, "BASS.DLL")
        End
    End If
    
    'Check that BASS 1.8 was loaded
    If BASS_GetStringVersion <> "1.8" Then
        Call MsgBox("BASS version 1.8 was not loaded", vbCritical, "Bass.Dll")
        End
    End If
    
    If (BASS_Init(-1, 44100, 0, Me.hWnd) <> 0) Then
        'Start digital output
        Call BASS_Start
    Else
        Call MsgBox("Error: Couldn't Initialize Digital Output", vbCritical, "Digital output")
        End
    End If
    
    'check if DX8 features are available
    Dim bi As BASS_INFO
    bi.size = LenB(bi)      'LenB(..) returns a byte data
    Call BASS_GetInfo(bi)
    If (bi.dsver < 8) Then
        Call BASS_Free
        Call MsgBox("DirectX version 8 is not Installed!!!", vbCritical, "DX8")
        End
    End If
  
End Sub

Private Sub Form_Unload(Cancel As Integer)
  Call BASS_Stop
  Call BASS_StreamFree(chan)
  Call BASS_MusicFree(chan)
  Call BASS_Free
End Sub

Public Sub UpdateFX(ByVal b As Integer)

  Dim v As Integer
  
  v = vsEQR(b).value
  
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
   
  'Free old stream (if any) and create new one
  Call BASS_StreamFree(chan)
  Call BASS_MusicFree(chan)
  
  chan = BASS_StreamCreateFile(BASSFALSE, CMD.FileName, 0, 0, BASS_SAMPLE_FX)
  If chan = 0 Then chan = BASS_MusicLoad(BASSFALSE, CMD.FileName, 0, 0, BASS_MUSIC_LOOP Or BASS_MUSIC_RAMP Or BASS_MUSIC_FX)
    
    'setup the effects
    
     Dim p As BASS_FXPARAMEQ
    
     fx(0) = BASS_ChannelSetFX(chan, BASS_FX_PARAMEQ) 'bass
     fx(1) = BASS_ChannelSetFX(chan, BASS_FX_PARAMEQ) 'mid
     fx(2) = BASS_ChannelSetFX(chan, BASS_FX_PARAMEQ) 'treble
     fx(3) = BASS_ChannelSetFX(chan, BASS_FX_REVERB)  'reverb
     
     p.fGain = 0
     p.fBandwidth = 18
     
     p.fCenter = 125                     ' bass   [125hz]
     Call BASS_FXSetParameters(fx(0), p)
     
     p.fCenter = 1000                    ' mid    [1khz]
     Call BASS_FXSetParameters(fx(1), p)
     
     p.fCenter = 8000                    ' treble [8khz]
     Call BASS_FXSetParameters(fx(2), p)
     
     ' you can add more EQ bands with changing:
     ' p.fCenter = N [hz] N>=80 and N<=16000
     
     Call UpdateFX(0) ' bass
     Call UpdateFX(1) ' mid
     Call UpdateFX(2) ' treble
     Call UpdateFX(3) ' reverb
     
     Call BASS_MusicPlay(chan)
     Call BASS_StreamPlay(chan, 0, BASS_SAMPLE_LOOP)
End Sub

Private Sub vsEQR_Scroll(Index As Integer)
  Call UpdateFX(Index)
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
