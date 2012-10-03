VERSION 5.00
Begin VB.Form frmNetRadio 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "BASS internet radio tuner"
   ClientHeight    =   2775
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4215
   Icon            =   "frmNetRadio.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2775
   ScaleWidth      =   4215
   StartUpPosition =   2  'CenterScreen
   Begin VB.Frame framePresents 
      Caption         =   "Presents"
      Height          =   1095
      Left            =   120
      TabIndex        =   0
      Top             =   0
      Width           =   3975
      Begin VB.CommandButton btnPresents 
         Caption         =   "5"
         Height          =   350
         Index           =   9
         Left            =   3360
         TabIndex        =   13
         Top             =   630
         Width           =   450
      End
      Begin VB.CommandButton btnPresents 
         Caption         =   "4"
         Height          =   350
         Index           =   8
         Left            =   2790
         TabIndex        =   12
         Top             =   630
         Width           =   450
      End
      Begin VB.CommandButton btnPresents 
         Caption         =   "3"
         Height          =   350
         Index           =   7
         Left            =   2220
         TabIndex        =   11
         Top             =   630
         Width           =   450
      End
      Begin VB.CommandButton btnPresents 
         Caption         =   "2"
         Height          =   350
         Index           =   6
         Left            =   1650
         TabIndex        =   10
         Top             =   630
         Width           =   450
      End
      Begin VB.CommandButton btnPresents 
         Caption         =   "1"
         Height          =   350
         Index           =   5
         Left            =   1080
         TabIndex        =   9
         Top             =   630
         Width           =   450
      End
      Begin VB.CommandButton btnPresents 
         Caption         =   "5"
         Height          =   350
         Index           =   4
         Left            =   3360
         TabIndex        =   8
         Top             =   240
         Width           =   450
      End
      Begin VB.CommandButton btnPresents 
         Caption         =   "4"
         Height          =   350
         Index           =   3
         Left            =   2790
         TabIndex        =   7
         Top             =   240
         Width           =   450
      End
      Begin VB.CommandButton btnPresents 
         Caption         =   "3"
         Height          =   350
         Index           =   2
         Left            =   2220
         TabIndex        =   6
         Top             =   240
         Width           =   450
      End
      Begin VB.CommandButton btnPresents 
         Caption         =   "2"
         Height          =   350
         Index           =   1
         Left            =   1650
         TabIndex        =   5
         Top             =   240
         Width           =   450
      End
      Begin VB.CommandButton btnPresents 
         Caption         =   "1"
         Height          =   350
         Index           =   0
         Left            =   1080
         TabIndex        =   4
         Top             =   240
         Width           =   450
      End
      Begin VB.Label lblModem 
         AutoSize        =   -1  'True
         Caption         =   "Modem"
         Height          =   195
         Left            =   120
         TabIndex        =   3
         Top             =   720
         Width           =   525
      End
      Begin VB.Label lblBroadband 
         AutoSize        =   -1  'True
         Caption         =   "Broadband"
         Height          =   195
         Left            =   120
         TabIndex        =   2
         Top             =   240
         Width           =   780
      End
   End
   Begin VB.Frame framePlaying 
      Caption         =   "Currently playing"
      Height          =   1455
      Left            =   120
      TabIndex        =   1
      Top             =   1200
      Width           =   3975
      Begin VB.Label lblBPS 
         Alignment       =   2  'Center
         Height          =   195
         Left            =   90
         TabIndex        =   16
         Top             =   1200
         Width           =   3795
         WordWrap        =   -1  'True
      End
      Begin VB.Label lblName 
         Alignment       =   2  'Center
         Caption         =   "not playing..."
         Height          =   435
         Left            =   105
         TabIndex        =   15
         Top             =   720
         Width           =   3765
         WordWrap        =   -1  'True
      End
      Begin VB.Label lblSong 
         Alignment       =   2  'Center
         Height          =   435
         Left            =   105
         TabIndex        =   14
         Top             =   240
         Width           =   3765
         WordWrap        =   -1  'True
      End
   End
End
Attribute VB_Name = "frmNetRadio"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'///////////////////////////////////////////////////////////////
' frmNetRadio.frm - Copyright (c) 2002
'                              JOBnik! [Arthur Aminov, ISRAEL]
'                              e-mail: jobnik2k@hotmail.com
'
' BASS Internet radio example
'
' Originally Translated from: - netradio.c - Example of Ian Luck
'///////////////////////////////////////////////////////////////

Public chan As Long
Public url As Variant

Private Sub Form_Load()
    'change and set the current path
    'so it won't ever tell you that "bass.dll" isn't found
    ChDrive App.Path
    ChDir App.Path
    
    'check if "bass.dll" is exists
    If FileExists(RPP(App.Path) & "bass.dll") = False Then
        MsgBox "BASS.DLL does not exists", vbCritical, "BASS.DLL"
        End
    End If
    
    'Check that BASS 1.8 was loaded
    If BASS_GetStringVersion <> "1.8" Then
        MsgBox "BASS version 1.8 was not loaded", vbCritical, "BASS.DLL"
        End
    End If
    
    'setup output device
    If (BASS_Init(-1, 44100, 0, Me.hWnd) = 0) Then
        MsgBox "Error: Can't initialize device", vbCritical
        End
    End If
    
    Call BASS_Start
    
    'Stream URLs
    url = Array("http://205.188.234.67:8048","http://207.111.214.243:8030", _
                "http://205.188.234.35:8034", "http://160.79.1.141:8406", _
                "http://205.188.234.68:8008", "http://160.79.1.141:8006", _
                "http://205.188.234.4:8016", "http://205.188.234.4:8014", _
                "http://62.95.108.25:8000", "http://62.95.108.25:8010")
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Call BASS_Free
End Sub

Private Sub btnPresents_Click(Index As Integer)
    Call BASS_StreamFree(chan) ' close old stream
    
    lblName.Caption = "connecting..."
    lblBPS.Caption = ""
    lblSong.Caption = ""
    
    chan = BASS_StreamCreateURL(url(IIf(Index < 5, Index * 2, (Index * 2) - 9)), 0, BASS_STREAM_META, vbNullString)
    
    If chan = 0 Then
        lblName.Caption = "not playing..."
        MsgBox "Error: Can't play the file", vbCritical
    Else
        Dim icyPTR As Long  'a pointer where ICY info is stored
        
        ' get the broadcast name and bitrate
        icyPTR = BASS_StreamGetTags(chan, BASS_TAG_ICY)
        
        If (icyPTR) Then
            Do
                icyPTR = icyPTR + Len(VBStrFromAnsiPtr(icyPTR)) + 1
                lblName.Caption = IIf(Mid(VBStrFromAnsiPtr(icyPTR), 1, 9) = "icy-name:", Mid(VBStrFromAnsiPtr(icyPTR), 10), lblName.Caption)
                lblBPS.Caption = IIf(Mid(VBStrFromAnsiPtr(icyPTR), 1, 7) = "icy-br:", "bitrate: " & Mid(VBStrFromAnsiPtr(icyPTR), 8), lblBPS.Caption)
                
                'NOTE: you can get more ICY info like: icy-genre:, icy-url:... :)
            Loop While (VBStrFromAnsiPtr(icyPTR) <> "")
        End If
        
        ' get the stream title and set sync for subsequent titles
        Call DoMeta(BASS_StreamGetTags(chan, BASS_TAG_META))
        Call BASS_ChannelSetSync(chan, BASS_SYNC_META, 0, AddressOf MetaSync, 0)
        
        ' play it!
        Call BASS_StreamPlay(chan, 0, 0)
    End If
End Sub

' some useful function :)

'check if any file exists
Public Function FileExists(ByVal FileName As String) As Boolean
  On Local Error Resume Next
  FileExists = (Dir$(FileName) <> "")
End Function

' RPP = Return Proper Path
Function RPP(ByVal fp As String) As String
    RPP = IIf(Mid(fp, Len(fp), 1) <> "\", fp & "\", fp)
End Function
