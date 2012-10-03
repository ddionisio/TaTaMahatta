program samplevis;

uses
  Forms,
  main in 'main.pas' {FormPlayer},
  circle_vis in 'circle_vis.pas',
  CommonTypes in 'CommonTypes.pas',
  Bass in 'Bass.pas',
  osc_vis in 'osc_vis.pas',
  spectrum_vis in 'spectrum_vis.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TFormPlayer, FormPlayer);
  Application.Run;
end.
