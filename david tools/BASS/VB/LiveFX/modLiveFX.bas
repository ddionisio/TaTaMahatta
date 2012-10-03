Attribute VB_Name = "modLiveFX"
'////////////////////////////////////////////////////////////////////
' modLiveFX.bas - Copyright (c) 2002 JOBnik! [Arthur Aminov, ISRAEL]
'                                    e-mail: jobnik2k@hotmail.com
'
' BASS full-duplex recording test with effects
'
' Originally translated from - livefx.c - Example of Ian Luck
'////////////////////////////////////////////////////////////////////

Declare Sub FillMemory Lib "kernel32.dll" Alias "RtlFillMemory" (Destination As Any, ByVal Length As Long, ByVal Fill As Byte)

Public chan As Long     'playback stream
Public fx(4) As Long    'FX handles
Public chunk As Long    'recording chunk size
Public input_ As Long   'current input source
Public latency As Long  'current latency

' stream function - play the recording data
Public Function stream(ByVal handle As Long, ByVal buffer As Long, ByVal Length As Long, ByVal user As Long) As Long
    Dim c As Long
    ' check how much recorded data is buffered
    c = BASS_ChannelGetData(RECORDCHAN, ByVal 0&, BASS_DATA_AVAILABLE)
    c = c - Length
    If (c > 2 * chunk + 1764) Then ' buffer has gotten pretty large so remove some
        c = c - chunk   'leave a single 'chunk'
        c = c - (c Mod 4)
        Call BASS_ChannelGetData(RECORDCHAN, ByVal 0&, ByVal c)  ' remove it
     End If
     ' fetch recorded data into stream
     c = BASS_ChannelGetData(RECORDCHAN, ByVal buffer, Length)
     If (c < Length) Then Call FillMemory(ByVal buffer + c, Length - c, 0)         'short of data
     stream = Length
End Function

Static Function Initialize() As Boolean

    Dim i As BASS_INFO
    
    ' setup output - 10ms update period, get device latency
    If (BASS_Init(-1, 44100, MakeLong(BASS_DEVICE_LATENCY, 10), frmLiveFX.hWnd) = 0) Then
        Call MsgBox("Can't initialize device", vbCritical)
        Initialize = False
        Exit Function
    End If
    
    Call BASS_Start

    i.size = LenB(i)
    Call BASS_GetInfo(i)
    ' buffer size = update period + 'minbuf'
    Call BASS_SetBufferLength((10 + i.minbuf) / 1000#)

    If (i.dsver < 8) Then ' no DX8, so disable effect buttons
        With frmLiveFX
            .chkChorus.Enabled = False
            .chkFlanger.Enabled = False
            .chkGargle.Enabled = False
            .chkReverb.Enabled = False
        End With
    End If

    ' start recording - default device, 44100hz, stereo, 16 bits, no callback function
    If (BASS_RecordInit(-1) = 0 Or BASS_RecordStart(44100, 0, 0, 0) = 0) Then
        Call BASS_Free
        Call MsgBox("Can't initialize recording device", vbCritical)
        Initialize = False
        Exit Function
    End If
    
    ' wait for recorded data to start arriving (calculate the latency)
    Do
        chunk = BASS_ChannelGetData(RECORDCHAN, 0, BASS_DATA_AVAILABLE)
    Loop While chunk = 0

    ' create a stream to play the recording data, and start it
    chan = BASS_StreamCreate(44100, 0, AddressOf stream, 0)
    Call BASS_StreamPlay(chan, 0, BASS_SAMPLE_LOOP)

    ' get list of inputs
    Dim c As Integer
    While BASS_RecordGetInputName(c) <> 0
        frmLiveFX.cmbSelChange.AddItem VBStrFromAnsiPtr(BASS_RecordGetInputName(c))
        If (BASS_RecordGetInput(c) And BASS_INPUT_OFF) = 0 Then
            frmLiveFX.cmbSelChange.ListIndex = c  'this 1 is currently "on"
            input_ = c
            frmLiveFX.sLevel.SelStart = GetLoWord(BASS_RecordGetInput(input_))  'get level
        End If
        c = c + 1
    Wend
    
    Initialize = True

End Function

Public Function max(ByVal a As Long, ByVal b As Long) As Long
    max = IIf(a > b, a, b)
End Function

Public Function min(ByVal a As Long, ByVal b As Long) As Long
    min = IIf(a < b, a, b)
End Function
