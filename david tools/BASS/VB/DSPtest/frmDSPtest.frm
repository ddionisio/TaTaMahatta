VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form frmDSPtest 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "BASS simple DSP test"
   ClientHeight    =   1185
   ClientLeft      =   45
   ClientTop       =   435
   ClientWidth     =   4515
   Icon            =   "frmDSPtest.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1185
   ScaleWidth      =   4515
   StartUpPosition =   2  'CenterScreen
   Begin VB.CheckBox chkSwap 
      Caption         =   "swap"
      Height          =   315
      Left            =   3360
      TabIndex        =   4
      Top             =   720
      Width           =   855
   End
   Begin VB.CheckBox chkFlange 
      Caption         =   "flanger"
      Height          =   315
      Left            =   2280
      TabIndex        =   3
      Top             =   720
      Width           =   855
   End
   Begin VB.CheckBox chkEcho 
      Caption         =   "echo"
      Height          =   315
      Left            =   1200
      TabIndex        =   2
      Top             =   720
      Width           =   735
   End
   Begin MSComDlg.CommonDialog cmd 
      Left            =   3960
      Top             =   0
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.CheckBox chkRotate 
      Caption         =   "rotate"
      Height          =   315
      Left            =   240
      TabIndex        =   1
      Top             =   720
      Width           =   735
   End
   Begin VB.CommandButton cmdOpen 
      Caption         =   "click here to open a file..."
      Height          =   375
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   4335
   End
End
Attribute VB_Name = "frmDSPtest"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'//////////////////////////////////////////////////////////////////////
' frmDSPtest.frm - Copyright (c) 2003 JOBnik! [Arthur Aminov, ISRAEL]
'                                     e-mail: jobnik2k@hotmail.com
'
' BASS simple DSP test
' Originally Translated from: - dsptest.c - Example of Ian Luck
'//////////////////////////////////////////////////////////////////////

'Display error message
Sub Error_(ByVal Message As String)
    Call MsgBox(Message & vbCrLf & vbCrLf & "Error Code : " & BASS_ErrorGetCode & vbCrLf & _
                    BASS_GetErrorDescription(BASS_ErrorGetCode), vbExclamation, "Error")
End Sub

Private Sub Form_Load()
    'change and set the current path
    'so VB won't ever tell you, that "bass.dll" hasn't been found
    ChDrive App.Path
    ChDir App.Path
    
    'check if "bass.dll" is exists
    If FileExists(RPP(App.Path) & "bass.dll") = False Then
        Call Error_("BASS.DLL does not exists")
        End
    End If
    
    'Check that BASS 1.8 was loaded
    If BASS_GetStringVersion <> "1.8" Then
        Call Error_("BASS version 1.8 was not loaded")
        End
    End If
    
    'initialize - default device, 44100hz, stereo, 16 bits, floating-point DSP
    If (BASS_Init(-1, 44100, BASS_DEVICE_FLOATDSP, Me.hWnd) = 0) Then
        Call Error_("Can't initialize device")
        Unload Me
    End If
    
    Call BASS_Start
    'check for floating-point capability
    floatable = BASS_StreamCreate(44100, BASS_SAMPLE_FLOAT, 0, 0)
    If (floatable) Then BASS_StreamFree (floatable) 'woohoo!
    
    rotdsp = 0
    echdsp = 0
    fladsp = 0
    swpdsp = 0
            
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Call BASS_Free
End Sub

'toggle "echo"
Private Sub chkEcho_Click()
    If (chkEcho.value = vbChecked) Then
        echpos = 0
        echdsp = BASS_ChannelSetDSP(chan, AddressOf Echo, 0)
    Else
        Call BASS_ChannelRemoveDSP(chan, echdsp)
    End If
End Sub

'toggle "flanger"
Private Sub chkFlange_Click()
    If (chkFlange.value = vbChecked) Then
        flapos = 0
        flas = FLABUFLEN / 2
        flasinc = 0.002
        fladsp = BASS_ChannelSetDSP(chan, AddressOf Flange, 0)
    Else
        Call BASS_ChannelRemoveDSP(chan, fladsp)
    End If
End Sub

'toggle "rotate"
Private Sub chkRotate_Click()
    If (chkRotate.value = vbChecked) Then
        rotpos = 0.7853981
        rotdsp = BASS_ChannelSetDSP(chan, AddressOf Rotate, 0)
    Else
        Call BASS_ChannelRemoveDSP(chan, rotdsp)
    End If
End Sub

'toggle "swapper"
Private Sub chkSwap_Click()
    If (chkSwap.value = vbChecked) Then
        swpdsp = BASS_ChannelSetDSP(chan, AddressOf Swapper, 0)
    Else
        Call BASS_ChannelRemoveDSP(chan, swpdsp)
    End If
End Sub

Private Sub cmdOpen_Click()
    On Error Resume Next    'if Cancel pressed...
    
    Dim speaker As Long
    speaker = Index
    
    cmd.CancelError = True
    cmd.flags = cdlOFNExplorer Or cdlOFNFileMustExist Or cdlOFNHideReadOnly
    cmd.DialogTitle = "Open"
    cmd.Filter = "playable files|*.mo3;*.xm;*.mod;*.s3m;*.it;*.mtm;*.mp3;*.mp2;*.mp1;*.ogg;*.wav|All files|*.*"
    cmd.ShowOpen
    
    'if cancel was pressed, exit the procedure
    If Err.Number = 32755 Then Exit Sub

    'free both MOD and stream, it must be one of them! :)
    Call BASS_MusicFree(chan)
    Call BASS_StreamFree(chan)

    
    chan = BASS_StreamCreateFile(BASSFALSE, cmd.FileName, 0, 0, IIf(floatable, BASS_SAMPLE_FLOAT, 0))
    
    If (chan = 0) Then chan = BASS_MusicLoad(BASSFALSE, cmd.FileName, 0, 0, BASS_MUSIC_LOOP Or BASS_MUSIC_RAMP Or IIf(floatable, BASS_MUSIC_FLOAT, 0))
    
    'whatever it is, it ain't playable
    If (chan = 0) Then
        Call Error_("Can't play the file")
        Exit Sub
    End If
    
    If (BASS_ChannelGetFlags(chan) And BASS_SAMPLE_MONO) Then
        ' mono = not allowed
        Call BASS_MusicFree(chan)
        Call BASS_StreamFree(chan)
        Call Error_("mono sources are not supported")
        Exit Sub
    End If
    
    cmdOpen.Caption = cmd.FileName
    
    'setup DSPs on new channel
    chkRotate_Click
    chkEcho_Click
    chkFlange_Click
    chkSwap_Click
    
    'play both MOD and stream, it must be one of them!
    Call BASS_MusicPlay(chan)
    Call BASS_StreamPlay(chan, 0, BASS_SAMPLE_LOOP)
End Sub

'----------------------------------
' some useful function :)
'----------------------------------

'check if any file exists
Public Function FileExists(ByVal FileName As String) As Boolean
  On Local Error Resume Next
  FileExists = (Dir$(FileName) <> "")
End Function

' RPP = Return Proper Path
Function RPP(ByVal fp As String) As String
    RPP = IIf(Mid(fp, Len(fp), 1) <> "\", fp & "\", fp)
End Function

