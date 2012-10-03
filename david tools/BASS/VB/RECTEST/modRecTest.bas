Attribute VB_Name = "modRecTest"
'///////////////////////////////////////////////////////////////
' modRecTest.bas - Copyright (c) 2002-2003
'                        JOBnik! [Arthur Aminov, ISRAEL]
'                        e-mail: jobnik2k@hotmail.com
'
' BASS Recording example
' Originally Translated from: - rectest.c - Example of Ian Luck
'
' Uses: Pure API! Types and Functions are Included as well!
'///////////////////////////////////////////////////////////////

'Memory allocation functions and constants
Public Const GMEM_FIXED = &H0
Public Const GMEM_MOVEABLE = &H2
Public Declare Function GlobalAlloc Lib "kernel32" (ByVal wFlags As Long, ByVal dwBytes As Long) As Long
Public Declare Function GlobalReAlloc Lib "kernel32" (ByVal hMem As Long, ByVal dwBytes As Long, ByVal wFlags As Long) As Long
Public Declare Function GlobalFree Lib "kernel32" (ByVal hMem As Long) As Long

'File handle functions, types and constants
Const OFS_MAXPATHNAME = 128
Const OF_CREATE = &H1000
Const OF_READ = &H0
Const OF_WRITE = &H1

Private Type OFSTRUCT
        cBytes As Byte
        fFixedDisk As Byte
        nErrCode As Integer
        Reserved1 As Integer
        Reserved2 As Integer
        szPathName(OFS_MAXPATHNAME) As Byte
End Type

Private Declare Function OpenFile Lib "kernel32" (ByVal lpFileName As String, lpReOpenBuff As OFSTRUCT, ByVal wStyle As Long) As Long
Private Declare Function WriteFile Lib "kernel32" (ByVal hFile As Long, lpBuffer As Any, ByVal nNumberOfBytesToWrite As Long, lpNumberOfBytesWritten As Long, ByVal lpOverlapped As Any) As Long
Private Declare Function CloseHandle Lib "kernel32" (ByVal hObject As Long) As Long

'WAV Header
Private Type WAVEHEADER_RIFF  '12 bytes
    RIFF As Long                '"RIFF" = &H46464952
    riffBlockSize As Long       'reclen - 8
    riffBlockType As Long       '"WAVE" = &H45564157
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
Private Type WAVEHEADER_data  '8 bytes
   dataBlockType As Long        '"data" = &H61746164
   dataBlockSize As Long        'reclen - 44
End Type

Dim wr As WAVEHEADER_RIFF
Dim wf As WAVEFORMAT
Dim wd As WAVEHEADER_data

Public Declare Sub CopyMemory Lib "kernel32" Alias "RtlMoveMemory" ( _
    lpvDest As Any, lpvSource As Any, ByVal cbCopy As Long)

Public Declare Function MessageBox Lib "user32" Alias "MessageBoxA" (ByVal hwnd As Long, ByVal lpText As String, ByVal lpCaption As String, ByVal wType As Long) As Long

Public input_ As Long       'current input source
Public BUFSTEP As Long      'memory allocation unit
Public reclen As Long       'buffer length
Public recPTR As Long       'a recording pointer to a memory location

Public chan As Long         'playback channel

'update the recording/playback counter
Public Sub UpdateDisplay()
    If (recPTR > 0) Then
        If (BASS_ChannelIsActive(chan)) Then 'playing
            frmRecTest.txtVR.Text = BASS_ChannelGetPosition(chan) & " / " & reclen
        Else
            frmRecTest.txtVR.Text = reclen
        End If
    End If
End Sub

'buffer the recorded data   [CALLBACK function]
Public Function RecordingCallback(ByVal buffer As Long, ByVal Length As Long, ByVal user As Long) As Integer
    'increase buffer size if needed
    If ((reclen Mod BUFSTEP) + Length >= BUFSTEP) Then
        recPTR = GlobalReAlloc(ByVal recPTR, ((reclen + Length) / BUFSTEP + 1) * BUFSTEP, GMEM_MOVEABLE)
        If recPTR = 0 Then
            Call MessageBox(frmRecTest.hwnd, "Error: Out of Memory!", "Memory overflow!", vbCritical)
            Call UpdateDisplay
            frmRecTest.btnPlay.Enabled = False
            frmRecTest.btnSave.Enabled = False
            RecordingCallback = BASSFALSE 'stop recording
            Exit Function
        End If
    End If
    'buffer the data
    Call CopyMemory(ByVal recPTR + reclen, ByVal buffer, Length)
    reclen = reclen + Length
    Call UpdateDisplay
    RecordingCallback = BASSTRUE 'continue recording
End Function

'start recording
Public Sub StartRecording()
    'clear any old recording
    If (recPTR) Then
        Call BASS_Stop
        Call BASS_StreamFree(chan)
        Call GlobalFree(ByVal recPTR)
        recPTR = 0
        Call UpdateDisplay
        frmRecTest.btnPlay.Enabled = False
        frmRecTest.btnSave.Enabled = False
    End If
    
    'allocate initial buffer and make space for WAVE header
    recPTR = GlobalAlloc(GMEM_FIXED, BUFSTEP)
    reclen = 44
     
    'fill the WAVE header
    wf.wFormatTag = 1
    wf.nChannels = 2
    wf.wBitsPerSample = 16
    wf.nSamplesPerSec = 44100
    wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign
    
    wf.wfBlockType = &H20746D66        '"fmt "
    wf.wfBlockSize = 16
        
    'Set WAV "RIFF" header
    wr.RIFF = &H46464952             '"RIFF"
    wr.riffBlockSize = 0      'after recording
    wr.riffBlockType = &H45564157    '"WAVE"
    
    'set WAV "data" header
    wd.dataBlockType = &H61746164     '"data"
    wd.dataBlockSize = 0       'after recording
    
    'copy WAV Header to Memory
    Call CopyMemory(ByVal recPTR, wr, LenB(wr))        '"RIFF"
    Call CopyMemory(ByVal recPTR + 12, wf, LenB(wf))   '"fmt "
    Call CopyMemory(ByVal recPTR + 36, wd, LenB(wd))   '"data"
            
    'start recording @ 44100hz 16-bit stereo
    If (BASS_RecordStart(44100, 0, AddressOf RecordingCallback, 0) <> 0) Then
        frmRecTest.btnRecord.Caption = "Stop"
    Else
        Call MessageBox(frmRecTest.hwnd, "Error: Couldn't start recording", "Recording...", vbCritical)
        Call GlobalFree(ByVal recPTR)
        recPTR = 0
    End If
End Sub

'stop recording
Public Sub StopRecording()
    Call BASS_ChannelStop(RECORDCHAN)
    frmRecTest.btnRecord.Caption = "Record"
    'complete the WAVE header
    wr.riffBlockSize = reclen - 8
    wd.dataBlockSize = reclen - 44
    
    Call CopyMemory(ByVal recPTR + 4, wr.riffBlockSize, LenB(wr.riffBlockSize))
    Call CopyMemory(ByVal recPTR + 40, wd.dataBlockSize, LenB(wd.dataBlockSize))
    
    'create a stream from the recording [from MEMORY!]
    chan = BASS_StreamCreateFile(BASSTRUE, recPTR, 0, reclen, 0)
    If chan Then
        'enable "play" & "save" buttons
        frmRecTest.btnPlay.Enabled = True
        frmRecTest.btnSave.Enabled = True
    End If
    Call BASS_Start
End Sub

'write the recorded data to disk    [from Memory location]
Public Sub WriteToDisk()
    On Error Resume Next    'if Cancel pressed...
    
    frmRecTest.cmd.CancelError = True
    frmRecTest.cmd.flags = cdlOFNExplorer Or cdlOFNFileMustExist Or cdlOFNHideReadOnly
    frmRecTest.cmd.DialogTitle = "Save As..."
    frmRecTest.cmd.Filter = "WAV files (*.wav)|*.wav|All files (*.*)|*.*"
    frmRecTest.cmd.DefaultExt = "wav"
    frmRecTest.cmd.ShowSave
   
    'if cancel was pressed, exit the procedure
    If Err.Number = 32755 Then Exit Sub
    
    'create a file .WAV, directly from Memory location
    Dim FileHandle As Long, ret As Long, OF As OFSTRUCT
    
    FileHandle = OpenFile(frmRecTest.cmd.FileName, OF, OF_CREATE)
    Call WriteFile(FileHandle, ByVal recPTR, reclen, ret, ByVal 0&)
    Call CloseHandle(FileHandle)
End Sub

Public Sub UpdateInputInfo()
    Dim it As Long
    it = BASS_RecordGetInput(input_)  ' get info on the input
    frmRecTest.sldInputLevel.value = GetLoWord(it)  'set the level slider
    
    Dim type_ As String
    Select Case (it And BASS_INPUT_TYPE_MASK)
        Case BASS_INPUT_TYPE_DIGITAL:
            type_ = "digital"
        Case BASS_INPUT_TYPE_LINE:
            type_ = "line-in"
        Case BASS_INPUT_TYPE_MIC:
            type_ = "microphone"
        Case BASS_INPUT_TYPE_SYNTH:
            type_ = "midi synth"
        Case BASS_INPUT_TYPE_CD:
            type_ = "analog cd"
        Case BASS_INPUT_TYPE_PHONE:
            type_ = "telephone"
        Case BASS_INPUT_TYPE_SPEAKER:
            type_ = "pc speaker"
        Case BASS_INPUT_TYPE_WAVE:
            type_ = "wave/pcm"
        Case BASS_INPUT_TYPE_AUX:
            type_ = "aux"
        Case BASS_INPUT_TYPE_ANALOG:
            type_ = "analog"
        Case Else:
            type_ = "undefined"
    End Select
    frmRecTest.lblInputType.Caption = type_ 'display the type
End Sub
