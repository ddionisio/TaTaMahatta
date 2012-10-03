VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form frmSpectrum 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Bass spectrum example (click to toggle mode)"
   ClientHeight    =   2535
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4800
   Icon            =   "frmSpectrum.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   169
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   320
   StartUpPosition =   2  'CenterScreen
   Begin VB.Timer tmrSpectrum 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   3480
      Top             =   1440
   End
   Begin VB.CommandButton btnLoadFile 
      Caption         =   "Load a File && Play It :)"
      Height          =   375
      Left            =   120
      TabIndex        =   0
      Top             =   2040
      Width           =   4575
   End
   Begin MSComDlg.CommonDialog cmd 
      Left            =   4080
      Top             =   1440
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
End
Attribute VB_Name = "frmSpectrum"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'//////////////////////////////////////////////////////////////
' frmSpectrum.frm - Copyright (c) 2002
'                        JOBnik! [Arthur Aminov, ISRAEL]
'                        e-mail: jobnik2k@hotmail.com
'
' Originally Translated from: - spectrum.c - Example of Ian Luck
'//////////////////////////////////////////////////////////////

Function PlayFile() As Boolean
    On Error Resume Next    'if Cancel pressed...
    
    cmd.CancelError = True
    cmd.flags = cdlOFNExplorer Or cdlOFNFileMustExist Or cdlOFNHideReadOnly
    cmd.DialogTitle = "Select a file to play"
    cmd.Filter = "Playable files (*.mo3;*.xm;*.mod;*.s3m;*.it;*.mtm;*.umx;*.mp?;*.ogg;*.wav)|*.mo3;*.xm;*.mod;*.s3m;*.it;*.mtm;*.umx;*.mp?;*.ogg;*.wav"
    cmd.ShowOpen
    
    'if cancel was pressed, exit the procedure
    If Err.Number = 32755 Then Exit Function
    
    BASS_StreamFree chan
    BASS_MusicFree chan
     
    chan = BASS_StreamCreateFile(BASSFALSE, cmd.FileName, 0, 0, BASS_SAMPLE_LOOP)
    If chan = 0 Then chan = BASS_MusicLoad(BASSFALSE, cmd.FileName, 0, 0, BASS_MUSIC_LOOP Or BASS_MUSIC_RAMP)
    
    If chan = 0 Then
        MsgBox "Error: Selected file couldn't be played!", vbCritical
        PlayFile = False ' Can't load the file
        Exit Function
    End If

    ' play both MOD and stream, it must be one of them! :)
    Call BASS_MusicPlay(chan)
    Call BASS_StreamPlay(chan, 0, BASS_SAMPLE_LOOP)
    
    PlayFile = True
    
End Function

Private Sub btnLoadFile_Click()
  Call PlayFile
End Sub

Private Sub Form_Load()
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
    
    'Check that BASS 1.8 was loaded
    If BASS_GetStringVersion <> "1.8" Then
        MsgBox "BASS version 1.8 was not loaded", vbCritical, "BASS.DLL"
        End
    End If
    
    InitBass = BASS_Init(-1, 44100, 0, Me.hWnd)
    
    ' Start digital output
    If InitBass Then
        BASS_Start
    Else
        MsgBox "Error: Can't initialize device", vbCritical, "Digital output"
        End
    End If
  
    If Not PlayFile Then       'start a file playing
        BASS_Free
        End
    End If

    specpos = 0
    specmode = False
    SPECWIDTH = 320
    SPECHEIGHT = 127
    
    'create bitmap to draw spectrum in - 8 bit for easy updating :)
    With bh.bmiHeader
        .biBitCount = 8
        .biPlanes = 1
        .biSize = Len(bh.bmiHeader)
        .biWidth = SPECWIDTH
        .biHeight = SPECHEIGHT  'upside down (line 0=bottom)
        .biClrUsed = 256
        .biClrImportant = 256
    End With
    
    Dim a As Byte
                 
    'setup palette
    For a = 1 To 127
        bh.bmiColors(a).rgbGreen = 255 - 2 * a
        bh.bmiColors(a).rgbRed = 2 * a
    Next a
    For a = 0 To 31
        bh.bmiColors(128 + a).rgbBlue = 8 * a
        bh.bmiColors(128 + 32 + a).rgbBlue = 255
        bh.bmiColors(128 + 32 + a).rgbRed = 8 * a
        bh.bmiColors(128 + 64 + a).rgbRed = 255
        bh.bmiColors(128 + 64 + a).rgbBlue = 8 * (31 - a)
        bh.bmiColors(128 + 64 + a).rgbGreen = 8 * a
        bh.bmiColors(128 + 96 + a).rgbRed = 255
        bh.bmiColors(128 + 96 + a).rgbGreen = 255
        bh.bmiColors(128 + 96 + a).rgbBlue = 8 * a
    Next a
            
    tmrSpectrum.Enabled = True
    
    'setup update timer - if using an API timer
    'may crash your program in a Compiled Mode!(sometimes!)
    'timing = timeSetEvent(25, 25, AddressOf UpdateSpectrum, 0, TIME_PERIODIC)
        
End Sub

Private Sub Form_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    specmode = Not specmode     'swap spectrum mode
    specpos = 0                 'set spectrum starting position
    tmrSpectrum.Interval = IIf(specmode, 25, 1)
    ReDim specbuf(SPECWIDTH * (SPECHEIGHT + 1)) As Byte 'clear display
End Sub

Private Sub Form_Unload(Cancel As Integer)
    BASS_Stop
    BASS_Free
    'Call timeKillEvent(timing)
End Sub

Private Sub tmrSpectrum_Timer()
    'function Variables are not in use with a VB timer!
    Call UpdateSpectrum(0, 0, 0, 0, 0)
End Sub
