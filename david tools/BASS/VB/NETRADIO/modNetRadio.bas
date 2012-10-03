Attribute VB_Name = "modNetRadio"
'///////////////////////////////////////////////////////////////
' modNetRadio.bas - Copyright (c) 2002
'                              JOBnik! [Arthur Aminov, ISRAEL]
'                              e-mail: jobnik2k@hotmail.com
'
' BASS Internet radio example
'
' Originally Translated from: - netradio.c - Example of Ian Luck
'///////////////////////////////////////////////////////////////

' update stream title from metadata
Sub DoMeta(ByVal meta As Long)
    Dim p As String
    If meta = 0 Then Exit Sub
    If ((Mid(VBStrFromAnsiPtr(meta), 1, 13) = "StreamTitle='")) Then
        p = Mid(VBStrFromAnsiPtr(meta), 14)
        frmNetRadio.lblSong.Caption = Mid(p, 1, InStr(p, ";") - 2)
    End If
End Sub

Sub MetaSync(ByVal handle As Long, ByVal channel As Long, ByVal data As Long, ByVal user As Long)
    Call DoMeta(data)
End Sub

