(*
BASS simple DSP test, copyright (c) 2000 Ian Luck.
TMT Pascal version by Vadim Bodrov, TMT Development Corp. (support@tmt.com)
*)

uses Windows, Messages, CommDlg, Strings, BASS;

{$r dsptest.res}

const
  ECHBUFLEN = 1200;        // buffer length
  FLABUFLEN = 350;         // buffer length
  ID_OPEN   = 10;
  ID_ROTA   = 11;
  ID_ECHO   = 12;
  ID_FLAN   = 13;

var
  rotdsp:  HDSP;            // DSP handle
  rotpos:  FLOAT;           // cur.pos
  echdsp:  HDSP;            // DSP handle
  echbuf:  array[0..ECHBUFLEN-1, 0..1] of SmallInt;  // buffer
  echpos:  Longint;         // cur.pos
  fladsp:  HDSP;            // DSP handle
  flabuf:  array[0..FLABUFLEN-1, 0..2] of SmallInt;  // buffer
  flapos:  Longint;         // cur.pos
  flas:    FLOAT;
  flasinc: FLOAT;           // sweep pos/min/max/inc
  _file:   array[0..MAX_PATH] of char;
  temp:    array[0..127] of char;
  win:     hWND := 0;
  chan:    DWORD;           // the channel... HMUSIC (mod) or HSTREAM (wav/mp3)
  ofn:     TOpenFileName;
  path:    array [0..MAX_PATH] of char;

function MESS(id, m: DWORD; w: WParam; l: lParam): Longint;
begin
  Result := SendDlgItemMessage(win, id, m, w, l);
end;

/* display error messages */
procedure Error(es: String);
var
  mes: array [0..199] of char;
begin
  es := es + #13 + '(error code: ' + IntToStr(BASS_ErrorGetCode) + ')';
  MessageBox(win, StrPCopy(mes, es), 'Error', 0);
end;

function Clip(a: Longint): Longint;
begin
  if a <= -32768 then
    Result := -32768
  else
  if a >= 32767 then
    Result := 32767
  else
    Result := a;
end;

function fmod(a, b: FLOAT): FLOAT;
begin
  Result := a - (b * Trunc(a / b));
end;

procedure Rotate conv arg_stdcall (handle: HSYNC; channel: DWORD; buffer: Pointer; length, user: DWORD);
var
  d: Pointer;
begin
  d := buffer;
  while (length > 0) do
  begin
    Short(d^) := Trunc(Short(d^) * Abs(Sin(rotpos)));
    inc(d, 2);
    Short(d^) := Trunc(Short(d^) * Abs(Cos(rotpos)));
    inc(d, 2);
    rotpos := fmod(rotpos + 0.00003, 3.1415927);
    length -:= 4;
  end;
end;

procedure Echo conv arg_stdcall (handle: HSYNC; channel: DWORD; buffer: Pointer; length, user: DWORD);
var
  d: Pointer;
  l, r: Longint;
begin
  d := buffer;
  while (length > 0) do
  begin
    l := Short(d^) + (echbuf[echpos, 1] shr 1);
    inc(d, 2);
    r := Short(d^) + (echbuf[echpos, 0] shr 1);
    dec(d, 2);
{$ifdef echo} // echo
    Short(d^) := clip(l);
    echbuf[echpos, 0] := Short(d^);
    inc(d, 2);
    Short(d^) := clip(r);
    echbuf[echpos, 1] := Short(d^);
    inc(d, 2);
{$else} // basic "bathroom" reverb
    echbuf[echpos, 0] := Short(d^);
    Short(d^) := clip(l);
    inc(d, 2);
    echbuf[echpos, 1] := Short(d^);
    Short(d^) := clip(r);
    inc(d, 2);
{$endif}
    echpos +:= 1;
    if echpos = ECHBUFLEN then echpos := 0;
    length -:= 4;
  end;
end;

procedure Flange conv arg_stdcall (handle: HSYNC; channel: DWORD; buffer: Pointer; length, user: DWORD);
var
  d: Pointer;
  s, p1, p2: Longint;
  f: FLOAT;
begin
  d := buffer;
  while (length > 0) do
  begin
    p1 := (flapos + Trunc(flas)) mod FLABUFLEN;
    p2 := (p1 + 1) mod FLABUFLEN;
    f := fmod(flas, 1.0);

    s := Short(d^) + Trunc(((1.0 - f) * flabuf[p1, 0]) + (f * flabuf[p2, 0]));
    flabuf[flapos, 0] := Short(d^);
    Short(d^) := clip(s);
    inc(d, 2);

    s := Short(d^) + Trunc(((1.0 - f) * flabuf[p1, 1]) + (f * flabuf[p2, 1]));
    flabuf[flapos, 1] := Short(d^);
    Short(d^) := clip(s);
    inc(d, 2);

    flapos +:= 1;
    if flapos = FLABUFLEN then flapos := 0;
    flas +:= flasinc;
    if (flas < 0) or (flas > FLABUFLEN) then flasinc := -flasinc;

    length -:= 4;
  end;
end;

function DialogProc conv arg_stdcall(h: HWND; m: UINT; w: WPARAM; l: LPARAM): Longint;
begin
  case m of
    WM_COMMAND:
     begin
       case LOWORD(w) of
        IDCANCEL:
        begin
          DestroyWindow(h);
          Result := 1;
          exit;
        end;
        ID_OPEN:
        begin
          ofn.lpstrFilter := 'playable files' + #0 +
                             '*.mo3;*.xm;*.mod;*.s3m;*.it;*.mtm;*.mp3;*.wav;*.ogg' + #0 +
                             'All files' + #0 + '*.*' + #0#0;
          ofn.lpstrFile := _file;
          if GetOpenFileName(ofn) then
          begin
            Move(_file, path, ofn.nFileOffset);
            path[ofn.nFileOffset - 1] := #0;
            // free both MOD and stream, it must be one of them! :)
            BASS_MusicFree(chan);
            BASS_StreamFree(chan);
            chan := BASS_StreamCreateFile(LONGFALSE, @_file, 0, 0, 0);
            if chan = 0 then
            begin
              chan := BASS_MusicLoad(LONGFALSE, @_file, 0, 0, BASS_MUSIC_LOOP or BASS_MUSIC_RAMP);
              if chan = 0 then
              begin
                // not a WAV/MP3 or MOD
                MESS(ID_OPEN,WM_SETTEXT, 0, Longint(StrPCopy(temp, 'click here to open a file...')));
                Error('Can''t play the file');
                exit;
              end;
            end;
            if (BASS_ChannelGetFlags(chan) and (BASS_SAMPLE_MONO or BASS_SAMPLE_8BITS)) <> 0 then
            begin
              /* not 16-bit stereo */
              MESS(ID_OPEN,WM_SETTEXT, 0, Longint(StrPCopy(temp, 'click here to open a file...')));
              BASS_MusicFree(chan);
              BASS_StreamFree(chan);
              Error('16-bit stereo sources only');
              exit;
            end;
            MESS(ID_OPEN, WM_SETTEXT, 0, Longint(@_file));
            // setup DSPs on new channel
            SendMessage(h, WM_COMMAND, ID_ROTA,0);
            SendMessage(h, WM_COMMAND, ID_ECHO,0);
            SendMessage(h, WM_COMMAND, ID_FLAN,0);
            // play both MOD and stream, it must be one of them! :)
            BASS_MusicPlay(chan);
            BASS_StreamPlay(chan, LONGFALSE, BASS_SAMPLE_LOOP);
          end;
          Result := 1;
          exit;
        end; // WM_COMMAND
        ID_ROTA:
        if MESS(ID_ROTA, BM_GETCHECK, 0, 0) <> 0 then
        begin
          rotpos := 0.7853981;
          rotdsp := BASS_ChannelSetDSP(chan, @Rotate, 0);
        end else
          BASS_ChannelRemoveDSP(chan, rotdsp);
        ID_ECHO:
        if MESS(ID_ECHO, BM_GETCHECK, 0, 0) <> 0 then
        begin
          FillChar(echbuf, sizeof(echbuf), 0);
          echpos := 0;
          echdsp := BASS_ChannelSetDSP(chan, @Echo, 0);
        end else
          BASS_ChannelRemoveDSP(chan, echdsp);
        ID_FLAN:
        if MESS(ID_FLAN, BM_GETCHECK, 0, 0) <> 0 then
        begin
          FillChar(flabuf, sizeof(flabuf), 0);
          flapos := 0;
          flas := FLABUFLEN / 2;
          flasinc := 0.002;
          fladsp := BASS_ChannelSetDSP(chan, @Flange, 0);
        end else
          BASS_ChannelRemoveDSP(chan, fladsp);
      end;
    end;

    WM_INITDIALOG:
    begin
      _file[0] := #0;
      win := h;
      GetCurrentDirectory(MAX_PATH, path);
      FillChar(ofn, sizeof(ofn), 0);
      ofn.lStructSize := sizeof(ofn);
      ofn.hwndOwner := h;
      ofn.hInstance := hInstance;
      ofn.nMaxFile := MAX_PATH;
      ofn.lpstrInitialDir := path;
      ofn.Flags := OFN_HIDEREADONLY or OFN_EXPLORER;
      // setup output - default device, 44100hz, stereo, 16 bits
      if BASS_Init(-1, 44100, 0, h) = LONGFALSE then
      begin
        Error('Can''t initialize device');
        DestroyWindow(h);
      end else
        BASS_Start;
      Result := 0;
    end;
  end;
  Result := 0;
end;

var
  Msg: TMsg;
begin
  // Check that BASS 1.8 was loaded
  if (BASS_GetVersion <> MAKELONG(1, 8)) then
  begin
    Error('BASS version 1.8 was not loaded');
    halt(0);
  end;

  DialogBoxParam(hInstance, MakeIntResource(1000), 0, @DialogProc, 0);

  BASS_Free;
end.