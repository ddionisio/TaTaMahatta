VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form frmSpeakers 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "BASS multi-speaker example"
   ClientHeight    =   3030
   ClientLeft      =   45
   ClientTop       =   435
   ClientWidth     =   4470
   Icon            =   "frmSpeakers.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3030
   ScaleWidth      =   4470
   StartUpPosition =   2  'CenterScreen
   Begin MSComDlg.CommonDialog cmd 
      Left            =   3960
      Top             =   2520
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.CommandButton cmdOpen 
      Caption         =   "click here to open a file..."
      Height          =   375
      Index           =   0
      Left            =   240
      TabIndex        =   4
      Top             =   240
      Width           =   3975
   End
   Begin VB.Frame Frame 
      Caption         =   "1 - front "
      Height          =   735
      Index           =   0
      Left            =   120
      TabIndex        =   0
      Top             =   0
      Width           =   4215
   End
   Begin VB.Frame Frame 
      Caption         =   "2 - rear "
      Height          =   735
      Index           =   1
      Left            =   120
      TabIndex        =   1
      Top             =   720
      Width           =   4215
      Begin VB.CommandButton cmdOpen 
         Caption         =   "click here to open a file..."
         Height          =   375
         Index           =   1
         Left            =   120
         TabIndex        =   5
         Top             =   240
         Width           =   3975
      End
   End
   Begin VB.Frame Frame 
      Caption         =   "3 - center/LFE "
      Height          =   735
      Index           =   2
      Left            =   120
      TabIndex        =   2
      Top             =   1440
      Width           =   4215
      Begin VB.CommandButton cmdOpen 
         Caption         =   "click here to open a file..."
         Height          =   375
         Index           =   2
         Left            =   120
         TabIndex        =   6
         Top             =   240
         Width           =   3975
      End
   End
   Begin VB.Frame Frame 
      Caption         =   "4 - rear center "
      Height          =   735
      Index           =   3
      Left            =   120
      TabIndex        =   3
      Top             =   2160
      Width           =   4215
      Begin VB.CommandButton cmdOpen 
         Caption         =   "click here to open a file..."
         Height          =   375
         Index           =   3
         Left            =   120
         TabIndex        =   7
         Top             =   240
         Width           =   3975
      End
   End
End
Attribute VB_Name = "frmSpeakers"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'//////////////////////////////////////////////////////////////////////
' frmSpeakers.frm - Copyright (c) 2003 JOBnik! [Arthur Aminov, ISRAEL]
'                                      e-mail: jobnik2k@hotmail.com
'
' BASS multi-speaker example
' Originally Translated from: - speakers.c - Example of Ian Luck
'//////////////////////////////////////////////////////////////////////

Dim flags(4) As Long
Dim chan(4) As Long


'Display error message
Sub Error_(ByVal Message As String)
    Call MsgBox(Message & vbCrLf & vbCrLf & "Error Code : " & BASS_ErrorGetCode & vbCrLf & _
                    BASS_GetErrorDescription(BASS_ErrorGetCode), vbExclamation, "Error")
End Sub

Private Sub cmdOpen_Click(Index As Integer)
    On Error Resume Next    'if Cancel pressed...
    
    Dim speaker As Long
    speaker = Index
    
    cmd.CancelError = True
    cmd.flags = cdlOFNExplorer Or cdlOFNFileMustExist Or cdlOFNHideReadOnly
    cmd.DialogTitle = "Open"
    cmd.Filter = "streamable files|*.mp3;*.mp2;*.mp1;*.ogg;*.wav|All files|*.*"
    cmd.ShowOpen
    
    'if cancel was pressed, exit the procedure
    If Err.Number = 32755 Then Exit Sub
    
    Call BASS_StreamFree(chan(speaker)) ' free old stream before opening new
    
    chan(speaker) = BASS_StreamCreateFile(BASSFALSE, cmd.FileName, 0, 0, flags(speaker))
    
    If (chan(speaker) = 0) Then
        Call Error_("Can't play the file")
        Exit Sub
    End If
    
    cmdOpen(speaker).Caption = cmd.FileName
    Call BASS_StreamPlay(chan(speaker), 0, BASS_SAMPLE_LOOP)
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
    
    'initialize BASS - default device
    If (BASS_Init(-1, 44100, 0, Me.hWnd) = 0) Then
        Call Error_("Can't initialize device")
        End
    End If
    
    flags(0) = BASS_SPEAKER_FRONT
    flags(1) = BASS_SPEAKER_REAR
    flags(2) = BASS_SPEAKER_CENLFE
    flags(3) = BASS_SPEAKER_REAR2

    'check how many speakers the device supports
    Dim i As BASS_INFO
    i.size = LenB(i)
    Call BASS_GetInfo(i)
    If (i.speakers < 4) Then 'no extra speakers detected, enable them anyway?
        If (MsgBox("Do you wish to enable ""speaker assignment"" anyway?", vbYesNo + vbQuestion, "No extra speakers detected") = vbYes) Then
            'reinitialize BASS - forcing speaker assignment
            Call BASS_Free
            If (BASS_Init(-1, 44100, BASS_DEVICE_SPEAKERS, Me.hWnd) = 0) Then
                Call Error_("Can't initialize device")
                End
            End If
            Call BASS_GetInfo(i) 'get info again
        End If
    End If
    If (i.speakers < 8) Then cmdOpen(3).Enabled = False
    If (i.speakers < 6) Then cmdOpen(2).Enabled = False
    If (i.speakers < 4) Then
        cmdOpen(1).Enabled = False
        'no multi-speaker support, so remove speaker flag for normal stereo output
        flags(0) = 0
    End If
    
    Call BASS_Start
        
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Call BASS_Free
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
