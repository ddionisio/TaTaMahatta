Attribute VB_Name = "modSpectrum"
'//////////////////////////////////////////////////////////////
' modSpectrum.bas - Copyright (c) 2002
'                        JOBnik! [Arthur Aminov, ISRAEL]
'                        e-mail: jobnik2k@hotmail.com
'
' Originally Translated from: - spectrum.c - Example of Ian Luck
'
' Uses Pure API: Types & Functions are Included as well
'//////////////////////////////////////////////////////////////

Public Const BI_RGB = 0&
Public Const DIB_RGB_COLORS = 0& '  color table in RGBs

Public Type BITMAPINFOHEADER '40 bytes
        biSize As Long
        biWidth As Long
        biHeight As Long
        biPlanes As Integer
        biBitCount As Integer
        biCompression As Long
        biSizeImage As Long
        biXPelsPerMeter As Long
        biYPelsPerMeter As Long
        biClrUsed As Long
        biClrImportant As Long
End Type

Public Type RGBQUAD
        rgbBlue As Byte
        rgbGreen As Byte
        rgbRed As Byte
        rgbReserved As Byte
End Type

Public Type BITMAPINFO
        bmiHeader As BITMAPINFOHEADER
        bmiColors(256) As RGBQUAD
End Type

Public Declare Function SetDIBitsToDevice Lib "gdi32" (ByVal hDC As Long, ByVal X As Long, ByVal Y As Long, ByVal dx As Long, ByVal dy As Long, ByVal SrcX As Long, ByVal SrcY As Long, ByVal Scan As Long, ByVal NumScans As Long, Bits As Any, BitsInfo As BITMAPINFO, ByVal wUsage As Long) As Long

'NOTE: Using an API timer will sometimes Crash your program
'Public Declare Function timeSetEvent Lib "winmm.dll" (ByVal uDelay As Long, ByVal uResolution As Long, ByVal lpFunction As Long, ByVal dwUser As Long, ByVal uFlags As Long) As Long
'Public Declare Function timeKillEvent Lib "winmm.dll" (ByVal uID As Long) As Long
'Public Const TIME_PERIODIC = 1  ' program for continuous periodic event
'Public timing As Long           ' an API timer Handle

Public SPECWIDTH As Long, SPECHEIGHT As Long
Public specmode As Boolean, specpos As Integer  ' spectrum mode (and marker pos for 2nd mode)
Public specbuf() As Byte    'a pointer

Public chan As Long         'stream/music handle

Public bh As BITMAPINFO     'bitmap header

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

Public Function Sqrt(ByVal num As Double) As Double
    Sqrt = num ^ 0.5
End Function

'update the spectrum display - the interesting bit :)
'function Variables are if using an API timer
Public Sub UpdateSpectrum(ByVal uTimerID As Long, ByVal uMsg As Long, ByVal dwUser As Long, ByVal dw1 As Long, ByVal dw2 As Long)
    Dim X As Long, Y As Long, y1 As Long
    Dim fft(1024) As Single     'get the FFT data
    Call BASS_ChannelGetData(chan, fft(0), BASS_DATA_FFT2048)
    
    If Not specmode Then
        ReDim specbuf(SPECWIDTH * (SPECHEIGHT + 1)) As Byte 'clear display
        For X = 0 To (SPECWIDTH / 2) - 1
            Y = Sqrt(fft(X + 1)) * 3 * SPECHEIGHT - 4 ' scale it (sqrt to make low values more visible)
            'y = fft(x + 1) * 10 * SPECHEIGHT 'scale it (linearly)
            If (Y > SPECHEIGHT) Then Y = SPECHEIGHT ' cap it
            If (X) Then  'interpolate from previous to make the display smoother
                y1 = (Y + y1) / 2
                While (y1 >= 0)
                    specbuf(y1 * SPECWIDTH + X * 2 - 1) = y1 + 1
                    y1 = y1 - 1
                Wend
            End If
            y1 = Y
            While (Y >= 0)
                specbuf(Y * SPECWIDTH + X * 2) = Y + 1 ' draw level
                Y = Y - 1
            Wend
        Next X
    Else
        For X = 0 To SPECHEIGHT - 1
            Y = Sqrt(fft(X + 1)) * 3 * 127 ' scale it (sqrt to make low values more visible)
            If (Y > SPECHEIGHT) Then Y = 127 ' cap it
            specbuf(X * SPECWIDTH + specpos) = 128 + Y ' plot it
        Next X
        'move marker onto next position
        specpos = (specpos + 1) Mod SPECWIDTH
        For X = 0 To SPECHEIGHT - 1
            specbuf(X * SPECWIDTH + specpos) = 255
        Next X
    End If
    
    'display the update
    'to display in a PictureBox, simply change the .hDC to Picture1.hDC :)
    Call SetDIBitsToDevice(frmSpectrum.hDC, 0, 0, SPECWIDTH, SPECHEIGHT, 0, 0, 0, SPECHEIGHT, specbuf(0), bh, 0)
End Sub
