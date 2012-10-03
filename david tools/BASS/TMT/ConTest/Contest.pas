(*
BASS Simple Console Test, copyright (c) 1999-2001 Ian Luck.
TMT Pascal version by Vadim Bodrov, TMT Development Corp. (support@tmt.com)
*)

uses Strings, Windows, MMSystem, BASS, Keyboard;

var
  _mod: HMUSIC;
  _str: HSTREAM;
  time, pos, level: DWORD;
  a, freq: DWORD;
  act: Longint;
  starttime: DWORD;
  temp: array[0..MAX_PATH] of char;
  volume: DWORD;
  pan: Longint;

function IntToFixed(val, digits: Longint): string;
var
  s: string;
begin
  s := IntToStr(val);
  while Length(s) < digits do s := '0' + s;
  Result := s;
end;

// display error messages
procedure Error(text: string);
begin
  WriteLn('Error(' + IntToStr(BASS_ErrorGetCode) + '): ' + text);
  BASS_Free;
  Halt(0);
end;

// looping synchronizer, resets the clock
procedure LoopSync(handle: HSYNC; channel, data, user: DWORD); stdcall;
begin
  starttime := timeGetTime;
end;

function strmod(_str, _mod: DWORD): DWORD;
begin
  if _str <> 0 then
    Result := _str
  else
    Result := _mod;
end;

begin
  Writeln('Simple console mode BASS example : MOD/MPx/OGG/WAV player');
  Writeln('---------------------------------------------------------');

  (* check that BASS 1.8 was loaded *)
  if BASS_GetVersion <> MAKELONG(1, 8) then
  begin
    Writeln('BASS version 1.8 was not loaded');
    exit;
  end;

  if ParamCount <> 1 then
  begin
    Writeln(#9 + 'usage: contest <file>');
    exit;
  end;

  (* setup output - default device, 44100hz, stereo, 16 bits *)
  if (not BASS_Init(-1, 44100, 0, 0)) then
      Error('Can''t initialize device');

  (* try streaming the file *)
  _str := BASS_StreamCreateFile(LONGFALSE, StrPCopy(temp, ParamStr(1)), 0, 0, 0);
  if _str = 0 then
    _str := BASS_StreamCreateURL(StrPCopy(temp, ParamStr(1)), 0, BASS_STREAM_RESTRATE, nil);
  if _str <> 0 then
  begin
    (* set a synchronizer for when the stream reaches the end *)
    BASS_ChannelSetSync(_str, BASS_SYNC_END, 0, @LoopSync, 0);
    pos := BASS_StreamGetLength(_str);
    if BASS_StreamGetFilePosition(_str, 1) <> DWORD(-1) then
    begin
      (* streaming from the internet *)
      if pos <> 0 then
        Write('streaming internet file [', pos, ' bytes]')
      else
        Write('streaming internet file');
    end else
      Write('streaming file [', pos, ' bytes]');
  end else
  begin
    (* try loading the MOD (with looping, sensitive ramping, surround sound and calculate the duration) *)
    _mod := BASS_MusicLoad(LONGFALSE, StrPCopy(temp, ParamStr(1)), 0, 0, BASS_MUSIC_LOOP or BASS_MUSIC_RAMPS or BASS_MUSIC_CALCLEN);
    if _mod = 0 then
      (* not a MOD either *)
      Error('Can''t play the file');
    (* set a synchronizer for when the MOD reaches the end *)
    BASS_ChannelSetSync(_mod, BASS_SYNC_END, 0, @LoopSync, 0);
    Write('playing MOD music "', BASS_MusicGetName(_mod), '" [', BASS_MusicGetLength(_mod, LONGFALSE), ' orders]');
    pos := BASS_MusicGetLength(_mod, LONGTRUE);
  end;

  (* calulate and display the time length *)
  if pos <> 0 then
  begin
    pos := Trunc(BASS_ChannelBytes2Seconds(strmod(_str, _mod), pos);
    Writeln(' ', pos div 60, ':', pos mod 60);
  end else (* no time length available *)
    Writeln('');

  BASS_Start;
  if _str <> 0 then
    BASS_StreamPlay(_str, FALSE, BASS_SAMPLE_LOOP)
  else
    BASS_MusicPlay(_mod);
  starttime := timeGetTime;

  (* NOTE: KbHit is included into the Keyboard unit *)
  while (not KbHit) and (BASS_ChannelIsActive(strmod(_str, _mod)) <> 0) do
  begin
    (* display some stuff and wait a bit *)
    time := timeGetTime - starttime;
    level := BASS_ChannelGetLevel(strmod(_str, _mod));
    pos := BASS_ChannelGetPosition(strmod(_str, _mod));
    if _str <> 0 then
      Write('pos ' + IntToFixed(pos, 9) + ' - time ' + IntToStr(time div 60000) + ':' + IntToFixed((time div 1000) mod 60, 2) + ' - L ')
    else
      Write('pos ' + IntToFixed(LOWORD(pos), 3) + ':' + IntToFixed(HIWORD(pos), 3) + ' - time ' + IntToStr(time div 60000) + ':' + IntToFixed((time div 1000) mod 60, 2) + ' - L ');
    if BASS_ChannelIsActive(strmod(_str, _mod)) = 2 then
      (* internet file stream playback has stalled *)
      Write('-- buffering : ', BASS_StreamGetFilePosition(_str, 1) -
        BASS_StreamGetFilePosition(_str, 0), ' --')
    else
    begin
      a := 93;
      while a > 0 do
      begin
        if LOWORD(level) >= a then
          Write('*')
        else
          Write('-');
        a := a * 2 div 3;
      end;
      Write(' ');
      a := 1;
      while (a < 128) do
      begin
        if HIWORD(level) >= a then
          Write('*')
        else
          Write('-');
        a +:= a - (a shr 1);
      end;
    end;
    Write(' R - cpu ' + Fix(BASS_GetCPU, 2, 1) + '%  ' + #13);
    Sleep(50);
  end;

  (* wind the frequency down... *)
  BASS_ChannelSlideAttributes(strmod(_str, _mod),1000,-1,-101,500);
  Sleep(300);
  (* ...and fade-out to avoid a "click" *)
  BASS_ChannelSlideAttributes(strmod(_str, _mod),-1,0,-101,200);
  Sleep(200);

  BASS_Free;
end.
