VERSION 5.00
Begin VB.Form frmLiveSpec 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "BASS ""live"" spectrum (click to toggle mode)"
   ClientHeight    =   1905
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4800
   BeginProperty Font 
      Name            =   "MS Sans Serif"
      Size            =   9.75
      Charset         =   177
      Weight          =   700
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   Icon            =   "frmLiveSpec.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   127
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   320
   StartUpPosition =   2  'CenterScreen
   Begin VB.Timer tmrLiveSpec 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   4320
      Top             =   1440
   End
End
Attribute VB_Name = "frmLiveSpec"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'///////////////////////////////////////////////////////////////
' frmLiveSpec.frm - Copyright (c) 2002
'                        JOBnik! [Arthur Aminov, ISRAEL]
'                        e-mail: jobnik2k@hotmail.com
'
' BASS "Live" spectrum analyser example
' Originally Translated from: - livespec.c - Example of Ian Luck
'///////////////////////////////////////////////////////////////

Private Sub Form_Load()
    'change and set the current path
    'so it won't ever tell you that bass.dll isn't found
    ChDrive App.Path
    ChDir App.Path
    
    'check if bass.dll is exists
    If FileExists(RPP(App.Path) & "bass.dll") = False Then
        MsgBox "BASS.DLL not exists", vbCritical
        End
    End If
    
    'Check that BASS 1.8 was loaded
    If BASS_GetStringVersion <> "1.8" Then
        MsgBox "BASS version 1.8 was not loaded", vbCritical
        End
    End If
    
    'initialize BASS recording (default device)
    If BASS_RecordInit(-1) = 0 Then
        MsgBox "Error: Can't initialize device", vbCritical
        End
    End If
        
    'start recording (44100hz mono 16-bit)
    'calling a CALLBACK function DuffRecording!
    If BASS_RecordStart(44100, BASS_SAMPLE_MONO, AddressOf DuffRecording, 0) = 0 Then
        MsgBox "Error: Can't start recording", vbCritical
        End
    End If
  
    specpos = 0
    specmode = False
    SPECWIDTH = 320
    SPECHEIGHT = 127
    
    Me.ScaleMode = vbPixels
    
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
    
    tmrLiveSpec.Enabled = True
    
    'setup update timer (40hz)- if using an API timer
    'may crash your program in a Compiled Mode!(sometimes!)
    'timing = timeSetEvent(25, 25, AddressOf UpdateSpectrum, 0, TIME_PERIODIC)

End Sub

Private Sub Form_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    specmode = Not specmode     'swap spectrum mode
    specpos = 0                 'set spectrum starting position
    tmrLiveSpec.Interval = IIf(specmode, 25, 1)
    ReDim specbuf(SPECWIDTH * (SPECHEIGHT + 1)) As Byte 'clear display
End Sub

Private Sub Form_Unload(Cancel As Integer)
    'Call timeKillEvent(timing)
    BASS_RecordFree
End Sub

Private Sub tmrLiveSpec_Timer()
    'function Variables are NOT in use with VB timer!
    Call UpdateSpectrum(0, 0, 0, 0, 0)
End Sub
