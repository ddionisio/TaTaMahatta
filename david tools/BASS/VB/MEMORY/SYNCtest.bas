Attribute VB_Name = "SYNCtest"
'///////////////////////////////////////////////////////////////////
' SYNCtest.bas - Copyright (c) 2001-2002
'                          JOBnik! [Arthur Aminov, ISRAEL]
'                          e-mail: jobnik2k@hotmail.com
'
' SYNC callback example...
'///////////////////////////////////////////////////////////////////

Public Declare Function MessageBox Lib "user32" Alias "MessageBoxA" (ByVal hwnd As Long, ByVal lpText As String, ByVal lpCaption As String, ByVal wType As Long) As Long

Public SyncEnd As Long

'using an API MessageBox works fine in compiled mode, but if you'll
'put there a VB MsgBox then in compiled mode it will crash!

Public Sub Stest(ByVal Handle As Long, ByVal channel As Long, ByVal data As Long, ByVal user As Long)
    Call MessageBox(frmMemory.hwnd, "End...", "SYNCtest", vbInformation)
End Sub

'Set: MusicTimer = Timer in the End, if Looping!
'using SYNC CALLBACK @ end
Public Sub SetMusicTimer(ByVal Handle As Long, ByVal channel As Long, ByVal data As Long, ByVal user As Long)
    frmMemory.MusicTimer = Timer
End Sub
