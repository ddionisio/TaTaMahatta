VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "mscomctl.ocx"
Begin VB.Form frmRecTest 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "BASS recording test"
   ClientHeight    =   990
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4965
   Icon            =   "frmRecTest.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   990
   ScaleWidth      =   4965
   StartUpPosition =   2  'CenterScreen
   Begin MSComctlLib.Slider sldInputLevel 
      Height          =   255
      Left            =   120
      TabIndex        =   6
      Top             =   720
      Width           =   1455
      _ExtentX        =   2566
      _ExtentY        =   450
      _Version        =   393216
      Max             =   100
      SelectRange     =   -1  'True
      TickStyle       =   3
   End
   Begin VB.ComboBox cmbInput 
      Height          =   315
      Left            =   120
      Style           =   2  'Dropdown List
      TabIndex        =   4
      Top             =   160
      Width           =   1455
   End
   Begin MSComDlg.CommonDialog cmd 
      Left            =   3960
      Top             =   600
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.Timer tmrRecTest 
      Enabled         =   0   'False
      Interval        =   100
      Left            =   3600
      Top             =   600
   End
   Begin VB.TextBox txtVR 
      Alignment       =   2  'Center
      BackColor       =   &H8000000F&
      Height          =   285
      Left            =   1680
      Locked          =   -1  'True
      TabIndex        =   3
      Top             =   600
      Width           =   3135
   End
   Begin VB.CommandButton btnSave 
      Caption         =   "Save"
      Enabled         =   0   'False
      Height          =   300
      Left            =   4080
      TabIndex        =   2
      Top             =   170
      Width           =   735
   End
   Begin VB.CommandButton btnPlay 
      Caption         =   "Play"
      Enabled         =   0   'False
      Height          =   300
      Left            =   3120
      TabIndex        =   1
      Top             =   170
      Width           =   855
   End
   Begin VB.CommandButton btnRecord 
      Caption         =   "Record"
      Height          =   300
      Left            =   1680
      TabIndex        =   0
      Top             =   170
      Width           =   1335
   End
   Begin VB.Label lblInputType 
      Alignment       =   2  'Center
      Height          =   195
      Left            =   120
      TabIndex        =   5
      Top             =   480
      Width           =   1440
   End
End
Attribute VB_Name = "frmRecTest"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'///////////////////////////////////////////////////////////////
' frmRecTest.frm - Copyright (c) 2002-2003
'                        JOBnik! [Arthur Aminov, ISRAEL]
'                        e-mail: jobnik2k@hotmail.com
'
' BASS Recording example
' Originally Translated from: - rectest.c - Example of Ian Luck
'
' Uses: Pure API! Types and Functions are Included as well!
'///////////////////////////////////////////////////////////////

'Display error message
Sub Error_(ByVal Message As String)
    Call MsgBox(Message & vbCrLf & vbCrLf & "Error Code : " & BASS_ErrorGetCode & vbCrLf & _
                    BASS_GetErrorDescription(BASS_ErrorGetCode), vbExclamation, "Error")
End Sub

Private Sub Form_Load()
    'change and set the current path
    'so it won't ever tell you that "bass.dll" isn't found
    ChDrive App.Path
    ChDir App.Path
    
    'check if "bass.dll" is exists
    If FileExists(RPP(App.Path) & "bass.dll") = False Then
        Call MsgBox("BASS.DLL does not exists", vbCritical, "BASS.DLL")
        End
    End If
    
    'Check that BASS 1.8 was loaded
    If BASS_GetStringVersion <> "1.8" Then
        Call Error_("BASS version 1.8 was not loaded")
        End
    End If
    
    'setup recording and output devices (using default devices)
    If (BASS_RecordInit(-1) = 0) Or (BASS_Init(-1, 44100, 0, Me.hwnd) = 0) Then
        Call Error_("Can't initialize device")
        End
    Else
        'get list of inputs
        Dim c As Integer
        input_ = -1
        While BASS_RecordGetInputName(c)
            cmbInput.AddItem VBStrFromAnsiPtr(BASS_RecordGetInputName(c))
            If (BASS_RecordGetInput(c) And BASS_INPUT_OFF) = 0 Then
                cmbInput.ListIndex = c  'this 1 is currently "on"
                input_ = c
                Call UpdateInputInfo    'display info
            End If
            c = c + 1
        Wend
    End If
    
    recPTR = 0
    reclen = 0
    BUFSTEP = 200000    'memory allocation unit
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Call GlobalFree(ByVal recPTR)
    Call BASS_RecordFree
    Call BASS_Free
End Sub

'input selection changed
Private Sub cmbInput_Click()
    If (input_ > -1) Then
        input_ = cmbInput.ListIndex
        'enable the selected input
        Dim i As Integer
        For i = 0 To cmbInput.ListCount - 1
            Call BASS_RecordSetInput(i, BASS_INPUT_OFF) '1st disable all inputs, then...
        Next i
        Call BASS_RecordSetInput(input_, BASS_INPUT_ON) 'enable the selected input
        Call UpdateInputInfo
    End If
End Sub

Private Sub btnPlay_Click()
    Call BASS_StreamPlay(chan, BASSTRUE, 0) 'play the recorded data
    tmrRecTest.Enabled = True 'timer to update the position display
End Sub

Private Sub btnRecord_Click()
    If (BASS_ChannelIsActive(RECORDCHAN) = 0) Then
        Call StartRecording
    Else
        Call StopRecording
    End If
End Sub

Private Sub btnSave_Click()
    Call WriteToDisk
End Sub

'set input source level
Private Sub sldInputLevel_Scroll()
    Dim level As Long
    level = sldInputLevel.value
    Call BASS_RecordSetInput(input_, BASS_INPUT_LEVEL Or level)
End Sub

Private Sub tmrRecTest_Timer()
    Call UpdateDisplay
    If (BASS_ChannelIsActive(chan) = 0) Then tmrRecTest.Enabled = False
End Sub

'---------------------------------
' some useful function :)
'---------------------------------

'check if any file exists
Public Function FileExists(ByVal FileName As String) As Boolean
  On Local Error Resume Next
  FileExists = (Dir$(FileName) <> "")
End Function

' RPP = Return Proper Path
Function RPP(ByVal fp As String) As String
    RPP = IIf(Mid(fp, Len(fp), 1) <> "\", fp & "\", fp)
End Function
