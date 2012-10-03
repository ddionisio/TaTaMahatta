object Form1: TForm1
  Left = 193
  Top = 107
  Width = 169
  Height = 168
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 32
    Width = 3
    Height = 13
  end
  object ComboBox1: TComboBox
    Left = 8
    Top = 8
    Width = 145
    Height = 21
    ItemHeight = 13
    TabOrder = 0
    OnChange = ComboBox1Change
  end
  object Button1: TButton
    Left = 8
    Top = 64
    Width = 145
    Height = 25
    Caption = 'Record / Stop'
    TabOrder = 1
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 8
    Top = 96
    Width = 145
    Height = 25
    Caption = 'Pause / Resume'
    TabOrder = 2
    OnClick = Button2Click
  end
end
