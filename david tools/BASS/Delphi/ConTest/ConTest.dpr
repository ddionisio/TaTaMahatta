{
BASS Simple Console Test, copyright (c) 1999-2001 Ian Luck.
==============================================================
Delphi version by Titus Miloi (titus.a.m@t-online.de)
}
program ConTest;

uses Windows, SysUtils, BASS, MMSystem;

{$R *.RES}

var
  starttime: DWORD;

function IntToFixed(val, digits: Integer): string;
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

var
  mus: HMUSIC;
  str: HSTREAM;
  chn: DWORD;
  time, pos, level: DWORD;
  a: Integer;
  freq, other: DWORD;
  mono: Boolean;

begin
  mono := FALSE;
  WriteLn('Simple console mode BASS example : MOD/MPx/OGG/WAV player');
  Writeln('---------------------------------------------------------');

  // check that BASS 1.8 was loaded
  if (BASS_GetVersion <> MAKELONG(1, 8)) then
  begin
    Writeln('BASS version 1.8 was not loaded');
    Exit;
  end;
  if (ParamCount <> 1) then
  begin
    WriteLn(#9 + 'usage: contest <file>');
    Exit;
  end;

  // setup output - default device, 44100hz, stereo, 16 bits
  if not BASS_Init(-1, 44100, 0, 0) then
    Error('Can''t initialize device');

  // try streaming the file
  mus := 0;
  str := BASS_StreamCreateFile(FALSE, PChar(ParamStr(1)), 0, 0, 0);
  if (not str) then
    str := BASS_StreamCreateURL(PChar(ParamStr(1)), 0, BASS_STREAM_RESTRATE, 0);
  if (str <> 0) then
  begin
    // check if the stream is mono (for the level indicator)
    mono := (BASS_ChannelGetFlags(str) and BASS_SAMPLE_MONO) <> 0;
    // set a synchronizer for when the stream reaches the end
    BASS_ChannelSetSync(str, BASS_SYNC_END, 0, LoopSync, 0);
    WriteLn('streaming file [' + IntToStr(BASS_StreamGetLength(str)) + ' bytes]');
  end
  else
  begin
    // load the MOD (with looping and sensitive ramping)
    mus := BASS_MusicLoad(FALSE, PChar(ParamStr(1)), 0, 0, BASS_MUSIC_LOOP or BASS_MUSIC_RAMPS);
    if (mus = 0) then
      // not a MOD either
      Error('Can''t play the file');
    // set a synchronizer for when the MOD reaches the end
    BASS_ChannelSetSync(mus, BASS_SYNC_END, 0, LoopSync, 0);
    WriteLn('playing MOD music "' + BASS_MusicGetName(mus) + '" [' + IntToStr(BASS_MusicGetLength(mus,FALSE)) + ' orders]');
  end;

  BASS_Start;
  if (str <> 0) then
    BASS_StreamPlay(str, FALSE, BASS_SAMPLE_LOOP)
  else
    BASS_MusicPlayEx(mus, 0, -1, TRUE);
  starttime := timeGetTime;

  chn := str;
  if chn = 0 then chn := mus;
  while (*not KeyPressed and*) BASS_ChannelIsActive(chn) do
  begin
    // display some stuff and wait a bit
    time := timeGetTime() - starttime;
    level := BASS_ChannelGetLevel(chn);
    pos := BASS_ChannelGetPosition(chn);
    if (str <> 0) then
      Write('pos ' + IntToFixed(pos, 9) + ' - time ' + IntToStr(time div 60000) + ':' + IntToFixed((time div 1000) mod 60, 2) + ' - L ')
    else
      Write('pos ' + IntToFixed(LOWORD(pos), 3) + ':' + IntToFixed(HIWORD(pos), 3) + ' - time ' + IntToStr(time div 60000) + ':' + IntToFixed((time div 1000) mod 60, 2) + ' - L ');
    a := 93;
    while (a > 0) do
    begin
      if LOWORD(level) >= a then
        Write('*')
      else
        Write('-');
      a := a * 2 div 3;
    end;
    Write(' ');
    if mono then
    begin
      a := 1;
      while (a < 128) do
      begin
        if LOWORD(level) >= a then
          Write('*')
        else
          Write('-');
        a := 2 * a - a div 2;
      end;
    end
    else
    begin
      a := 1;
      while (a < 128) do
      begin
        if HIWORD(level) >= a then
          Write('*')
        else
          Write('-');
        a := 2 * a - a div 2;
      end;
    end;
    Write(' R - cpu ' + FloatToStrF(BASS_GetCPU(), ffFixed, 0, 1) + '%  ' + #13);
    Sleep(50);
  end;
  Writeln('                                                                   ');

  // wind the frequency down...
  BASS_ChannelSlideAttributes(chn,1000,-1,-101,500);
  Sleep(300);
  // ...and fade-out to avoid a "click"
  BASS_ChannelSlideAttributes(chn,-1,0,-101,200);
  Sleep(200);

  BASS_Free();
end.
