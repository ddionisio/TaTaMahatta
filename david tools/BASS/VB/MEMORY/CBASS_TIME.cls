VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "CBASS_TIME"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
'///////////////////////////////////////////////////////////////////
' CBASS_TIME.cls - Copyright (c) 2001-2002
'                            JOBnik! [Arthur Aminov, ISRAEL]
'                            e-mail: jobnik2k@hotmail.com
'
' This VB Class Module, shows how to Get:
'   - Total Playing Time, in Seconds, of any Stream/Music
'   - Playing Position in Seconds
'   - a Function that will Convert Total Seconds into:
'       Hours:Minutes:Seconds   =   01:25:50
'   - and much more... :)
'   * Some Functions are Based on Ian Lucks 'C' Examples!
'
'   * You may NOT use some of this functions, if you're using
'     latest BASS.DLL versions [I think since v1.3?]
'///////////////////////////////////////////////////////////////////

'Convert total duration into time format: HH:MM:SS
Public Function GetTime(ByVal Seconds As Long) As String
    If Seconds <= 0 Then
        GetTime = "00:00:00"
        Exit Function
    End If
    
    Dim Hour As Single, Min As Single, Sec As Single
    
    Hour = Seconds / 60 / 60
    Sec = Seconds Mod 60
    Min = (Hour - Int(Hour)) * 60
  
    GetTime = Format(Int(Hour), "00") & ":" & Format(Int(Min), "00") & ":" & Format(Int(Sec), "00")
End Function

'Get playing position in seconds of a Stream [NOT 4 MOD]
Public Function GetPlayingPos(ByVal Handle As Long) As Integer
    On Error GoTo err_GetPlayingPos
        GetPlayingPos = BASS_ChannelBytes2Seconds(Handle, BASS_ChannelGetPosition(Handle))
        'next shows how to get playing position without bass built function
        'GetPlayingPos = BASS_ChannelGetPosition(Handle) / GetBytesPerSecond(Handle)
    Exit Function
err_GetPlayingPos:
    GetPlayingPos = 0
End Function

'Get total Duration in Seconds of Stream/Music
Public Function GetDuration(ByVal Handle As Long) As Long
    On Error GoTo err_GetDuration
    Dim Lens As Long, Freq As Long, Flags As Long
        Lens = BASS_StreamGetLength(Handle) 'Stream Length
        If Lens = -1 Then Lens = BASS_MusicGetLength(Handle, BASSTRUE)  'Music Length
        Call BASS_ChannelGetAttributes(Handle, Freq, 0, 0)  'sample rate
        Flags = BASS_ChannelGetFlags(Handle)
    GetDuration = Lens / (Freq * (IIf(Flags And BASS_SAMPLE_MONO, 1, 2) * _
                         (IIf(Flags And BASS_SAMPLE_8BITS, 1, 2))))
    Exit Function
err_GetDuration:
    GetDuration = 0
End Function

'Returns - Bytes Per Second
Public Function GetBytesPerSecond(ByVal Handle As Long) As Long
    On Error GoTo err_BPS
        Dim Flags As Long, BPS As Long
        Call BASS_ChannelGetAttributes(Handle, BPS, 0, 0)  ' sample rate
        Flags = BASS_ChannelGetFlags(Handle) 'stereo/mono, 8/16 bit flags
        If Not (Flags And BASS_SAMPLE_MONO) Then BPS = BPS * 2
        If Not (Flags And BASS_SAMPLE_8BITS) Then BPS = BPS * 2
    GetBytesPerSecond = BPS
    Exit Function
err_BPS:
    GetBytesPerSecond = 0
End Function

'Returns - Kilo Bits Per Second
Public Function GetBitsPerSecond(ByVal Handle As Long, FileLength As Long) As Long
    On Error GoTo err_KBPS
        GetBitsPerSecond = CInt(((FileLength * 8) / GetDuration(Handle)) / 1000)
    Exit Function
err_KBPS:
    GetBitsPerSecond = 0
End Function

'Returns - 'Stereo'/'Mono'
Public Function GetMode(ByVal Handle As Long) As String
    GetMode = IIf(BASS_ChannelGetFlags(Handle) And BASS_SAMPLE_MONO, "Mono", "Stereo")
End Function

'Returns - 8/16 bits
Public Function GetBits(ByVal Handle As Long) As Byte
    GetBits = IIf(BASS_ChannelGetFlags(Handle) And BASS_SAMPLE_8BITS, 8, 16)
End Function

'Returns - Sample Rate [Frequency]
Public Function GetFrequency(ByVal Handle As Long) As Long
    Call BASS_ChannelGetAttributes(Handle, GetFrequency, 0, 0)
End Function

'Returns - DirectX version
Public Function GetDXver() As Byte
    Dim bi As BASS_INFO
    bi.size = LenB(bi)      'LenB(..) returns a byte data
    Call BASS_GetInfo(bi)
    GetDXver = bi.dsver
End Function
