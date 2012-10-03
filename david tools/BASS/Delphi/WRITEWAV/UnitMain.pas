{
 Source code under Bass license
 by Alessandro Cappellozza

 http://digilander.libero.it/Kappe
 mail acappellozza@ieee.org

 Notice
  It is designed for mp3 but work on other streams (ogg, and so on)
 }

unit UnitMain;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, Bass, StdCtrls, ComCtrls, ExtCtrls;

type
  TForm1 = class(TForm)
    EditFileName: TEdit;
    EditDest: TEdit;
    btnOpen: TButton;
    BtnDecode: TButton;
    OpenDialog: TOpenDialog;
    btnCancel: TButton;
    ProgressBar: TProgressBar;
    LabelOp: TLabel;
    Label1: TLabel;
    Label2: TLabel;
    Bevel1: TBevel;
    Bevel2: TBevel;
    Label3: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure BtnDecodeClick(Sender: TObject);
    procedure btnOpenClick(Sender: TObject);
    procedure btnCancelClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
     procedure DecodeFile(OutPath, SourceFileName : String);
  end;

var
  Form1: TForm1;
  PercentDone : Integer;
  CancelOp    : Boolean;
  
implementation

{$R *.dfm}

procedure TForm1.DecodeFile(OutPath, SourceFileName : String);
 var chan, flags, frq, vl : DWORD; fp : integer; Tmp, pos : Integer;
     cbuff : PChar; buf : array [0..10000] of DWORD;
   ProgressRatio : integer;
   BytesBuffer, BytesRead : integer;
   StartSector, EndSector : integer;
   temp : string;
   i : longint;
   RecStream : TFileStream;
   nChannels       : Word;   // number of channels (i.e. mono, stereo, etc.)
   nSamplesPerSec  : DWORD;  // sample rate
   nAvgBytesPerSec : DWORD;
   nBlockAlign     : Word;
   wBitsPerSample  : Word;   // number of bits per sample of mono data
   FileName : String;
begin

    chan := BASS_StreamCreateFile(FALSE, PChar(SourceFileName), 0, 0, BASS_STREAM_DECODE);
    pos := BASS_StreamGetLength(chan);

    CancelOp := False;
    LabelOp.Caption      := 'Opening file ...';

	flags := BASS_ChannelGetFlags(chan);
	if (flags and BASS_SAMPLE_MONO > 0) then nChannels := 1 else nChannels :=2;
        if nChannels = 2 then
           nBlockAlign := 4
        else
           nBlockAlign := 2;
        if (flags and BASS_SAMPLE_8BITS > 0) then wBitsPerSample := 8 else wBitsPerSample := 16;
	BASS_ChannelGetAttributes(chan, frq, vl, Tmp);
        nSamplesPerSec := frq;
	nAvgBytesPerSec := nSamplesPerSec * nBlockAlign;

    FileName := ExtractFileName(SourceFileName);
    FileName := Copy(FileName, 1, Length(FileName) - Length(ExtractFileExt(FileName)));
    RecStream := TFileStream.Create(OutPath + FileName + '.wav', fmCreate);

 // Write header portion of wave file
    temp := 'RIFF'; RecStream.write(temp[1], length(temp));
    temp := #0#0#0#0; RecStream.write(temp[1], length(temp));   // File size: to be updated
    temp := 'WAVE'; RecStream.write(temp[1], length(temp));
    temp := 'fmt '; RecStream.write(temp[1], length(temp));
    temp := #$10#0#0#0; RecStream.write(temp[1], length(temp)); // Fixed
    temp := #1#0; RecStream.write(temp[1], length(temp));       // PCM format
    if nChannels = 1 then
       temp := #1#0
    else
       temp := #2#0;
    RecStream.write(temp[1], length(temp));
    RecStream.write(nSamplesPerSec, 2);
    temp := #0#0; RecStream.write(temp[1], length(temp));   // SampleRate is given as dWord
    RecStream.write(nAvgBytesPerSec, 4);
    RecStream.write(nBlockAlign, 2);
    RecStream.write(wBitsPerSample, 2);
    temp := 'data'; RecStream.write(temp[1],length(temp));
    temp := #0#0#0#0; RecStream.write(temp[1],length(temp)); // Data size: to be updated
    BytesRead := 1024 * 10; // 10kBytes
	while (BASS_ChannelIsActive(chan) > 0) do
         begin
                BASS_ChannelGetData(chan, @buf, BytesRead);
                RecStream.Write(buf, BytesRead);
                Application.ProcessMessages;
                if CancelOp then Break;
                PercentDone := Trunc(100 * (BASS_ChannelGetPosition(Chan) / BASS_StreamGetLength(chan)));
                ProgressBar.Position := PercentDone;
                LabelOp.Caption      := 'Done ' + IntToStr(PercentDone) + '%';
	end;

   LabelOp.Caption      := 'Closing file ...';
// complete WAV header
// Rewrite some fields of header
   i := RecStream.Size - 8;    // size of file
   RecStream.Position := 4;
   RecStream.write(i, 4);
   i := i - $24;               // size of data
   RecStream.Position := 40;
   RecStream.write(i, 4);
   RecStream.Free;
   LabelOp.Caption      := 'Done';
end;

procedure TForm1.FormCreate(Sender: TObject);
begin
     BASS_Init(-2, 44100, 0, Application.Handle);
end;

procedure TForm1.FormClose(Sender: TObject; var Action: TCloseAction);
begin
Bass_Free;
end;

procedure TForm1.BtnDecodeClick(Sender: TObject);
begin
 DecodeFile(EditDest.Text, EditFileName.Text);
end;

procedure TForm1.btnOpenClick(Sender: TObject);
begin
 if not OpenDialog.Execute then exit;
  EditFileName.text := OpenDialog.FileName;
  EditDest.Text     := ExtractFileDir(OpenDialog.FileName);
  if EditDest.Text[Length(EditDest.Text)] <> '\' then EditDest.Text := EditDest.Text + '\';
end;

procedure TForm1.btnCancelClick(Sender: TObject);
begin
 CancelOp := True;
end;

end.
 