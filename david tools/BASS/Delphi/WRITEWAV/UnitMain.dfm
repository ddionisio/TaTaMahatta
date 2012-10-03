object Form1: TForm1
  Left = 216
  Top = 147
  Width = 536
  Height = 259
  Caption = 'MP3/OGG to  Wave Write (by Alessandro Cappellozza)'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel2: TBevel
    Left = 16
    Top = 152
    Width = 497
    Height = 57
    Shape = bsFrame
  end
  object Bevel1: TBevel
    Left = 16
    Top = 8
    Width = 497
    Height = 137
    Shape = bsFrame
  end
  object LabelOp: TLabel
    Left = 24
    Top = 160
    Width = 31
    Height = 13
    Caption = 'Ready'
  end
  object Label1: TLabel
    Left = 24
    Top = 16
    Width = 34
    Height = 13
    Caption = 'Source'
  end
  object Label2: TLabel
    Left = 24
    Top = 64
    Width = 53
    Height = 13
    Caption = 'Destination'
  end
  object Label3: TLabel
    Left = 24
    Top = 120
    Width = 107
    Height = 13
    Caption = 'acappellizza@ieee.org'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlue
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsUnderline]
    ParentFont = False
  end
  object EditFileName: TEdit
    Left = 24
    Top = 32
    Width = 473
    Height = 21
    TabOrder = 0
  end
  object EditDest: TEdit
    Left = 24
    Top = 80
    Width = 473
    Height = 21
    TabOrder = 1
  end
  object btnOpen: TButton
    Left = 424
    Top = 112
    Width = 75
    Height = 25
    Caption = 'Open'
    TabOrder = 2
    OnClick = btnOpenClick
  end
  object BtnDecode: TButton
    Left = 352
    Top = 168
    Width = 75
    Height = 25
    Caption = 'Decode'
    TabOrder = 3
    OnClick = BtnDecodeClick
  end
  object btnCancel: TButton
    Left = 432
    Top = 168
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 4
    OnClick = btnCancelClick
  end
  object ProgressBar: TProgressBar
    Left = 24
    Top = 176
    Width = 321
    Height = 17
    Min = 0
    Max = 100
    TabOrder = 5
  end
  object OpenDialog: TOpenDialog
    Filter = 'mp3/ogg|*.mp3;*.ogg'
    Left = 456
    Top = 8
  end
end
