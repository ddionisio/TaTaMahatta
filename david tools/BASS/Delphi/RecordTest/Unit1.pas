unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Bass;

type
  TForm1 = class(TForm)
    ComboBox1: TComboBox;
    Button1: TButton;
    Button2: TButton;
    Label1: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormDestroy(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure ComboBox1Change(Sender: TObject);
  private
    { Private declarations }
    WaveStream: TFileStream;
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

function RecordCallback(Buffer: Pointer; Length, User: DWord): Bool; stdcall;
begin
  Form1.WaveStream.Write(Buffer^, Length);
  Result := True;
end;

procedure TForm1.FormCreate(Sender: TObject);
var
  i: Integer;
  dName: PChar;
begin
  if BASS_GetVersion <> DWord(MAKELONG(1,8)) then begin
    MessageDlg('BASS version 1.8 was not loaded', mtError, [mbOk], 0);
    Halt;
  end;
  if (not BASS_RecordInit(-1)) or (not BASS_Init(-1, 44100, 0, Handle)) then
  begin
    BASS_RecordFree;
    BASS_Free();
    MessageDlg('Can''t start default recording device', mtError, [mbOk], 0);
    Halt;
  end;
  i := 0;
  dName := BASS_RecordGetInputName(i);
  while dName <> nil do
  begin
    ComboBox1.Items.Add(StrPas(dName));
    i := i + 1;
    dName := BASS_RecordGetInputName(i);
  end;
end;

procedure TForm1.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  Action := caFree;
end;

procedure TForm1.FormDestroy(Sender: TObject);
begin
  BASS_Stop;
  BASS_Free;
end;

procedure TForm1.Button1Click(Sender: TObject);
type
  WAVHDR = packed record
    riff: array[0..3] of Char;
    len: DWord;
    cWavFmt: array[0..7] of Char;
    dwHdrLen: DWord;
    wFormat: Word;
    wNumChannels: Word;
    dwSampleRate: DWord;
    dwBytesPerSec: DWord;
    wBlockAlign: Word;
    wBitsPerSample: Word;
    cData: array[0..3] of Char;
    dwDataLen: DWord;
  end;
var
  i: Integer;
  WaveHdr: WAVHDR;
begin
  if ComboBox1.ItemIndex < 0 then
    Exit;
  if BASS_ChannelIsActive(RECORDCHAN) = 0 then
  begin
    Caption := 'Recording...';
    WaveHdr.riff := 'RIFF';
    WaveHdr.len := 36;
    WaveHdr.cWavFmt := 'WAVEfmt ';
    WaveHdr.dwHdrLen := 16;
    WaveHdr.wFormat := 1;
    WaveHdr.wNumChannels := 2;
    WaveHdr.dwSampleRate := 44100;
    WaveHdr.wBlockAlign := 4;
    WaveHdr.dwBytesPerSec := 176400;
    WaveHdr.wBitsPerSample := 16;
    WaveHdr.cData := 'data';
    WaveHdr.dwDataLen := 0;
    WaveStream := TFileStream.Create(ChangeFileExt(Application.ExeName, '.wav'), fmCreate);
    WaveStream.Write(WaveHdr, SizeOf(WAVHDR));
    i := 0;
    while BASS_RecordSetInput(i, BASS_INPUT_OFF) do
      i := i + 1;
    BASS_RecordSetInput(ComboBox1.ItemIndex, BASS_INPUT_ON);
    if not BASS_RecordStart(44100, 0, @RecordCallback, 0) then
    begin
      MessageDlg('Error', mtError, [mbOk], 0);
    end;
  end
  else
  begin
    Caption := 'Ready to record';
    BASS_ChannelStop(RECORDCHAN);
    BASS_RecordFree;
    WaveStream.Position := 4;
    i := WaveStream.Size - 8;
    WaveStream.Write(i, 4);
    i := i - $24;
    WaveStream.Position := 40;
    WaveStream.Write(i, 4);
    WaveStream.Free;
  end;
end;

procedure TForm1.Button2Click(Sender: TObject);
begin
  if BASS_ChannelIsActive(RECORDCHAN) = 1 then
  begin
    if not BASS_ChannelPause(RECORDCHAN) then
      ShowMessage('Error');
  end
  else if BASS_ChannelIsActive(RECORDCHAN) = 3 then
    if not BASS_ChannelResume(RECORDCHAN) then
      ShowMessage('Error');
end;

procedure TForm1.ComboBox1Change(Sender: TObject);
var
  i: Integer;
begin
  i := BASS_RecordGetInput(ComboBox1.ItemIndex);
  case (i and BASS_INPUT_TYPE_MASK) of
    BASS_INPUT_TYPE_DIGITAL: Label1.Caption := 'digital';
    BASS_INPUT_TYPE_LINE: Label1.Caption := 'line-in';
    BASS_INPUT_TYPE_MIC: Label1.Caption := 'microphone';
    BASS_INPUT_TYPE_SYNTH: Label1.Caption := 'midi synth';
    BASS_INPUT_TYPE_CD: Label1.Caption := 'analog cd';
    BASS_INPUT_TYPE_PHONE: Label1.Caption := 'telephone';
    BASS_INPUT_TYPE_SPEAKER: Label1.Caption := 'pc speaker';
    BASS_INPUT_TYPE_WAVE: Label1.Caption := 'wave/pcm';
    BASS_INPUT_TYPE_AUX: Label1.Caption := 'aux';
    BASS_INPUT_TYPE_ANALOG: Label1.Caption := 'analog';
  else
    Label1.Caption := 'undefined';
  end;
end;

end.
