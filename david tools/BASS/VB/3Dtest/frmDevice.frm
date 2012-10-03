VERSION 5.00
Begin VB.Form frmDevice 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "BASS output device selector"
   ClientHeight    =   1500
   ClientLeft      =   5160
   ClientTop       =   4530
   ClientWidth     =   3855
   ControlBox      =   0   'False
   Icon            =   "frmDevice.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1500
   ScaleWidth      =   3855
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton btnOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   300
      Left            =   2280
      TabIndex        =   2
      Top             =   1080
      Width           =   1335
   End
   Begin VB.CheckBox chkLowQ 
      Caption         =   "low quality"
      Height          =   255
      Left            =   120
      TabIndex        =   1
      Top             =   1080
      Width           =   1095
   End
   Begin VB.ListBox lstDevices 
      Height          =   840
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   3615
   End
End
Attribute VB_Name = "frmDevice"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////////
' frmDevice.frm (c) 2003 JOBnik! [Arthur Aminov, ISRAEL]
'                        e-mail: jobnik2k@hotmail.com
'
' BASS output device selector
' originally translated from - 3dtest.c - example of Ian Luck
'/////////////////////////////////////////////////////////////

Public device As Long       'selected device
Public lowqual As Integer   'low quality option

Private Sub btnOK_Click()
    Unload Me
End Sub

Private Sub chkLowQ_Click()
    lowqual = chkLowQ.value
End Sub

Private Sub Form_Load()
    Dim text As String
    Dim c As Integer
    
    c = 0
    While BASS_GetDeviceDescription(c)
        text = VBStrFromAnsiPtr(BASS_GetDeviceDescription(c))
        'Check if the device supports 3D - don't bother with update thread */
        If (BASS_Init(c, 44100, BASS_DEVICE_3D Or BASS_DEVICE_NOTHREAD, Me.hWnd)) Then
            If (BASS_GetEAXParameters(0, 0#, 0#, 0#)) Then
                text = text & " [EAX]"  'it has EAX
            End If
            Call BASS_Free
        
            lstDevices.AddItem text
        End If
        c = c + 1
    Wend
    
    If (lstDevices.ListCount) Then lstDevices.Selected(0) = True
    device = 0
    lowqual = 0
End Sub

Private Sub lstDevices_Click()
    device = lstDevices.ListIndex
End Sub

Private Sub lstDevices_DblClick()
    Unload Me
End Sub
