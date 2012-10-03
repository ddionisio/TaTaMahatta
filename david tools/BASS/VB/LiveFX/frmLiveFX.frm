VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "mscomctl.ocx"
Begin VB.Form frmLiveFX 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "BASS full-duplex recording test with effects"
   ClientHeight    =   795
   ClientLeft      =   45
   ClientTop       =   435
   ClientWidth     =   4965
   Icon            =   "frmLiveFX.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   795
   ScaleWidth      =   4965
   StartUpPosition =   2  'CenterScreen
   Begin VB.Timer tmrLiveFX 
      Enabled         =   0   'False
      Interval        =   250
      Left            =   3600
      Top             =   360
   End
   Begin VB.CheckBox chkFlanger 
      Caption         =   "Flanger"
      Height          =   195
      Left            =   3960
      TabIndex        =   7
      Top             =   480
      Width           =   975
   End
   Begin VB.CheckBox chkReverb 
      Caption         =   "Reverb"
      Height          =   195
      Left            =   3960
      TabIndex        =   6
      Top             =   120
      Width           =   975
   End
   Begin VB.CheckBox chkGargle 
      Caption         =   "Gargle"
      Height          =   195
      Left            =   2880
      TabIndex        =   5
      Top             =   480
      Width           =   975
   End
   Begin VB.CheckBox chkChorus 
      Caption         =   "Chorus"
      Height          =   195
      Left            =   2880
      TabIndex        =   4
      Top             =   120
      Width           =   975
   End
   Begin VB.ComboBox cmbSelChange 
      Height          =   315
      Left            =   120
      Style           =   2  'Dropdown List
      TabIndex        =   0
      Top             =   120
      Width           =   1455
   End
   Begin MSComctlLib.Slider sLevel 
      Height          =   315
      Left            =   0
      TabIndex        =   1
      Top             =   480
      Width           =   1695
      _ExtentX        =   2990
      _ExtentY        =   556
      _Version        =   393216
      Max             =   100
      TickStyle       =   3
   End
   Begin VB.Label lblLatency 
      Alignment       =   2  'Center
      BorderStyle     =   1  'Fixed Single
      Height          =   315
      Left            =   1800
      TabIndex        =   3
      Top             =   320
      Width           =   885
   End
   Begin VB.Label lblLtc 
      Alignment       =   2  'Center
      AutoSize        =   -1  'True
      Caption         =   "latency"
      Height          =   195
      Left            =   1800
      TabIndex        =   2
      Top             =   40
      Width           =   885
   End
End
Attribute VB_Name = "frmLiveFX"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'////////////////////////////////////////////////////////////////////
' frmLiveFX.frm - Copyright (c) 2002 JOBnik! [Arthur Aminov, ISRAEL]
'                                    e-mail: jobnik2k@hotmail.com
'
' BASS full-duplex recording test with effects
'
' Originally translated from - livefx.c - Example of Ian Luck
'////////////////////////////////////////////////////////////////////

Private Sub chkChorus_Click()
    If (fx(0)) Then
        Call BASS_ChannelRemoveFX(chan, fx(0))
        fx(0) = 0
    Else
        fx(0) = BASS_ChannelSetFX(chan, BASS_FX_CHORUS)
    End If
End Sub

Private Sub chkGargle_Click()
    If (fx(1)) Then
        Call BASS_ChannelRemoveFX(chan, fx(1))
        fx(1) = 0
    Else
        fx(1) = BASS_ChannelSetFX(chan, BASS_FX_GARGLE)
    End If
End Sub

Private Sub chkReverb_Click()
    If (fx(2)) Then
        Call BASS_ChannelRemoveFX(chan, fx(2))
        fx(2) = 0
    Else
        fx(2) = BASS_ChannelSetFX(chan, BASS_FX_REVERB)
    End If
End Sub

Private Sub chkFlanger_Click()
    If (fx(3)) Then
        Call BASS_ChannelRemoveFX(chan, fx(3))
        fx(3) = 0
    Else
        fx(3) = BASS_ChannelSetFX(chan, BASS_FX_FLANGER)
    End If
End Sub

Private Sub cmbSelChange_Click()
    Dim i As Integer
    
    input_ = cmbSelChange.ListIndex ' get the selection
    
    For i = 0 To cmbSelChange.ListCount - 1
        Call BASS_RecordSetInput(i, BASS_INPUT_OFF)     '1st disable all inputs, then...
    Next i
    
    Call BASS_RecordSetInput(input_, BASS_INPUT_ON) ' enable the selected input
    sLevel.Value = GetLoWord(BASS_RecordGetInput(input_))   'get the level

End Sub

Private Sub Form_Load()
    'change and set the current path
    'so it won't ever tell you that bass.dll is not found
    ChDrive App.Path
    ChDir App.Path
    
    'check if 'bass.dll' is exists
    If FileExists(RPP(App.Path) & "bass.dll") = False Then
        MsgBox "BASS.DLL does not exists", vbCritical, "BASS.DLL"
        End
    End If
    
    'Check that BASS 1.8 was loaded
    If BASS_GetStringVersion <> "1.8" Then
        MsgBox "BASS version 1.8 was not loaded", vbCritical, "BASS.DLL"
        End
    End If

    Call MsgBox("Do not set the input to 'WAVE' / 'What you hear' (etc...) with" & vbCrLf & _
                "the level set high, as that is likely to result in nasty feedback.", vbExclamation, "Feedback warning")
                
    If Not Initialize Then Unload Me
    tmrLiveFX.Enabled = True
End Sub

Private Sub sLevel_Scroll()
    Call BASS_RecordSetInput(input_, BASS_INPUT_LEVEL Or sLevel.Value)
End Sub

Private Sub tmrLiveFX_Timer()
    ' display current latency (averaged)
    latency = (latency * 3 + BASS_ChannelGetData(chan, 0, BASS_DATA_AVAILABLE) _
        + BASS_ChannelGetData(RECORDCHAN, 0, BASS_DATA_AVAILABLE)) / 4
    lblLatency.Caption = CInt(latency * 1000 / 176400)
End Sub

Private Sub Form_Unload(Cancel As Integer)
    ' release it all
    tmrLiveFX.Enabled = False
    Call BASS_RecordFree
    Call BASS_Free
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
