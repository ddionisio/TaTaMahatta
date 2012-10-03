VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form frmWriteWave 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "BASS simple WAVE writer"
   ClientHeight    =   2295
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4470
   Icon            =   "frmWriteWave.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2295
   ScaleWidth      =   4470
   StartUpPosition =   2  'CenterScreen
   Begin VB.CommandButton btnConvert 
      Caption         =   "Convert to ""BASS.WAV"""
      Enabled         =   0   'False
      Height          =   495
      Left            =   120
      TabIndex        =   3
      Top             =   1680
      Width           =   4215
   End
   Begin VB.CommandButton btnLoadFile 
      Caption         =   "Select File to Convert"
      Height          =   495
      Left            =   120
      TabIndex        =   2
      Top             =   1080
      Width           =   4215
   End
   Begin MSComDlg.CommonDialog cmd 
      Left            =   3840
      Top             =   480
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.Label lblTime 
      AutoSize        =   -1  'True
      Caption         =   "Time:"
      Height          =   195
      Left            =   120
      TabIndex        =   4
      Top             =   360
      Width           =   390
   End
   Begin VB.Label lblStrFile 
      AutoSize        =   -1  'True
      Caption         =   "Streaming File:"
      Height          =   195
      Left            =   120
      TabIndex        =   1
      Top             =   120
      Width           =   1035
   End
   Begin VB.Label lblPos 
      AutoSize        =   -1  'True
      Caption         =   "Pos:"
      Height          =   195
      Left            =   120
      TabIndex        =   0
      Top             =   600
      Width           =   315
   End
End
Attribute VB_Name = "frmWriteWave"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'//////////////////////////////////////////////////////////////
' frmWriteWave.frm - Copyright (c) 2002
'                        JOBnik! [Arthur Aminov, ISRAEL]
'                        e-mail: jobnik2k@hotmail.com
'
' Originally Translated from: - writewav.c - Example of Ian Luck
'
' BASS WAVE writer example: MOD/MPx/OGG -> "BASS.WAV"
'//////////////////////////////////////////////////////////////

Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)

Dim chan As Long, pos As Long, flags As Long
Dim str As Boolean  'stream OR music
Dim buf() As Byte

Private Type WAVEHEADER_RIFF  '12 bytes
    RIFF As Long                '"RIFF" = &H46464952
    riffBlockSize As Long       'pos + 44 - 8
    riffBlockType As Long       '"WAVE" = &H45564157
End Type

Private Type WAVEHEADER_data  '8 bytes
   dataBlockType As Long        '"data" = &H61746164
   dataBlockSize As Long        'pos
End Type

Private Type WAVEFORMAT     '24 bytes
    wfBlockType As Long         '"fmt " = &H20746D66
    wfBlockSize As Long
    '--- block size begins from here = 16 bytes
    wFormatTag As Integer
    nChannels As Integer
    nSamplesPerSec As Long
    nAvgBytesPerSec As Long
    nBlockAlign As Integer
    wBitsPerSample As Integer
End Type

Dim wr As WAVEHEADER_RIFF
Dim wf As WAVEFORMAT
Dim wd As WAVEHEADER_data

Private Sub btnLoadFile_Click()
    On Error Resume Next    'if Cancel pressed...
    
    cmd.CancelError = True
    cmd.flags = cdlOFNExplorer Or cdlOFNFileMustExist Or cdlOFNHideReadOnly
    cmd.DialogTitle = "Select a file to Convert"
    cmd.Filter = "Convertable files (*.mo3;*.xm;*.mod;*.s3m;*.it;*.mtm;*.umx;*.mp?;*.ogg)|*.mo3;*.xm;*.mod;*.s3m;*.it;*.mtm;*.umx;*.mp?;*.ogg"
    cmd.ShowOpen
    
    'if cancel was pressed, exit the procedure
    If Err.Number = 32755 Then Exit Sub
    
    Dim tmpStr As String
    
    tmpStr = GetFileName(cmd.FileName)
    
    'try streaming the file/url
    chan = BASS_StreamCreateFile(BASSFALSE, cmd.FileName, 0, 0, BASS_STREAM_DECODE)
    If chan = 0 Then chan = BASS_StreamCreateURL(cmd.FileName, 0, BASS_STREAM_DECODE Or BASS_STREAM_RESTRATE, 0)
    If chan > 0 Then
        pos = BASS_StreamGetLength(chan)
        lblStrFile.Caption = "Streaming File: " & tmpStr & " [" & pos & " bytes]"
        str = True
    End If
    
    'try loading the MOD (with sensitive ramping, and calculate the duration)
    If chan = 0 Then
        chan = BASS_MusicLoad(BASSFALSE, cmd.FileName, 0, 0, BASS_MUSIC_DECODE Or BASS_MUSIC_RAMPS Or BASS_MUSIC_CALCLEN)
        If chan = 0 Then
            'not a MOD either
            MsgBox "Error: Can't play the file", vbExclamation
            Exit Sub
        Else
            lblStrFile.Caption = "Playing MOD music \" & BASS_MusicGetNameString(chan) & "\ [" & BASS_MusicGetLength(chan, BASSFALSE) & " orders]"
            pos = BASS_MusicGetLength(chan, BASSTRUE)
            str = False
        End If
    End If
    
    'display the time length
    If (pos) Then
        pos = CLng(BASS_ChannelBytes2Seconds(chan, pos))
        lblTime.Caption = "Time: " & CInt(pos / 60) & ":" & CInt(pos Mod 60)
    Else 'no time length available
        lblPos.Caption = ""
    End If
    
    btnConvert.Enabled = True
    
End Sub

Private Sub btnConvert_Click()

    btnLoadFile.Enabled = False
    btnConvert.Enabled = False
    
    'Set WAV Format
    flags = BASS_ChannelGetFlags(chan)
    wf.wFormatTag = 1
    wf.nChannels = IIf(flags And BASS_SAMPLE_MONO, 1, 2)
    Call BASS_ChannelGetAttributes(chan, wf.nSamplesPerSec, -1, -1)
    wf.wBitsPerSample = IIf(flags And BASS_SAMPLE_8BITS, 8, 16)
    wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign
    wf.wfBlockType = &H20746D66        '"fmt "
    wf.wfBlockSize = 16
        
    'Set WAV "RIFF" header
    wr.RIFF = &H46464952             '"RIFF"
    wr.riffBlockSize = 0      'after convertion
    wr.riffBlockType = &H45564157    '"WAVE"
    
    'set WAV "data" header
    wd.dataBlockType = &H61746164     '"data"
    wd.dataBlockSize = 0       'after convertion
    
    'create a file BASS.WAV
    Open RPP(App.Path) & "BASS.WAV" For Binary Lock Read Write As #1
    
    'Write WAV Header to file
    Put #1, , wr    'RIFF
    Put #1, , wf    'Format
    Put #1, , wd    'data
         
    pos = 0
    ReDim buf(19999) As Byte
    
    While BASS_ChannelIsActive(chan)
        ReDim Preserve buf(BASS_ChannelGetData(chan, buf(0), 20000) - 1) As Byte
        'write data to WAV file
        Put #1, , buf
        pos = BASS_ChannelGetPosition(chan)
        If str Then
            lblPos.Caption = "Pos: " & pos
        Else
            lblPos.Caption = "Pos: " & GetLoWord(pos) & ":" & GetHiWord(pos)
        End If

        Call Sleep(1)   'don't hog the CPU too much :)
        DoEvents        'in case you want to exit...
    Wend
    
    'complete WAV header
    wr.riffBlockSize = pos + 44 - 8
    wd.dataBlockSize = pos
    
    On Error Resume Next
        
    Put #1, 5, wr.riffBlockSize
    Put #1, 41, wd.dataBlockSize
    
    Close #1
    str = False
    btnLoadFile.Enabled = True

End Sub

Private Sub Form_Load()
    
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
    
    'setup output - "no sound" device, 44100hz, stereo, 16 bits
    If (BASS_Init(-2, 44100, BASS_DEVICE_NOTHREAD, Me.hWnd) = 0) Then
        MsgBox "Error: Can't initialize device", vbCritical, "Digital output"
        End
    End If
                
End Sub

Private Sub Form_Unload(Cancel As Integer)
    On Error Resume Next
    
    'complete WAVE header
    wr.riffBlockSize = pos + 44 - 8
    wd.dataBlockSize = pos
        
    Put #1, 5, wr.riffBlockSize
    Put #1, 41, wd.dataBlockSize
    
    Close #1
    
    BASS_Free
    
    End
End Sub

'Some useful functions :)

Public Function GetFileName(ByVal fp As String) As String
    GetFileName = Mid(fp, InStrRev(fp, "\") + 1)
End Function

'check if any file exists
Public Function FileExists(FileName) As Boolean
  On Local Error Resume Next
  FileExists = (Dir$(FileName) <> "")
End Function

' RPP = Return Proper Path
Public Function RPP(ByVal fp As String) As String
  If Right(fp, 1) <> "\" Then
    RPP = fp & "\"
  Else
    RPP = fp
  End If
End Function
