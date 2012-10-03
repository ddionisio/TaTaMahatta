object FormPlayer: TFormPlayer
  Left = 285
  Top = 168
  Width = 520
  Height = 353
  Caption = 'Visualyzations Example [by Alessandro Cappellozza]'
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
  object BackImageRes: TImage
    Left = 8
    Top = 8
    Width = 160
    Height = 104
    AutoSize = True
    Visible = False
  end
  object PaintFrame: TPaintBox
    Left = 8
    Top = 8
    Width = 161
    Height = 105
  end
  object Button1: TButton
    Left = 176
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Open song ...'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 176
    Top = 40
    Width = 75
    Height = 25
    Caption = 'Background'
    TabOrder = 1
    OnClick = Button2Click
  end
  object RadioMode: TRadioGroup
    Left = 112
    Top = 120
    Width = 113
    Height = 81
    Caption = 'Draw mode'
    ItemIndex = 0
    Items.Strings = (
      'Line'
      'Solid'
      'Pixel')
    TabOrder = 2
    OnClick = RadioModeClick
  end
  object RadioDraw: TRadioGroup
    Left = 8
    Top = 120
    Width = 97
    Height = 81
    Caption = 'Visualyzation'
    ItemIndex = 0
    Items.Strings = (
      'Circle'
      'Oscilloscope'
      'Spectrum')
    TabOrder = 3
  end
  object GroupBox1: TGroupBox
    Left = 264
    Top = 8
    Width = 241
    Height = 193
    Caption = 'Common properties'
    TabOrder = 4
    object Label1: TLabel
      Left = 10
      Top = 24
      Width = 7
      Height = 13
      Caption = 'X'
    end
    object Label2: TLabel
      Left = 10
      Top = 56
      Width = 7
      Height = 13
      Caption = 'Y'
    end
    object Label3: TLabel
      Left = 13
      Top = 88
      Width = 28
      Height = 13
      Caption = 'Offset'
    end
    object Label4: TLabel
      Left = 14
      Top = 120
      Width = 19
      Height = 13
      Caption = 'Res'
    end
    object Label6: TLabel
      Left = 8
      Top = 160
      Width = 47
      Height = 13
      Caption = 'Bacl color'
    end
    object Label7: TLabel
      Left = 120
      Top = 160
      Width = 45
      Height = 13
      Caption = 'Pen color'
    end
    object TrackOff: TTrackBar
      Left = 48
      Top = 80
      Width = 169
      Height = 33
      Max = 120
      Orientation = trHorizontal
      Frequency = 5
      Position = 20
      SelEnd = 0
      SelStart = 0
      TabOrder = 0
      ThumbLength = 12
      TickMarks = tmBoth
      TickStyle = tsAuto
      OnChange = TrackOffChange
    end
    object TrackRes: TTrackBar
      Left = 48
      Top = 112
      Width = 169
      Height = 33
      Max = 2
      Min = 1
      Orientation = trHorizontal
      Frequency = 1
      Position = 2
      SelEnd = 0
      SelStart = 0
      TabOrder = 1
      ThumbLength = 12
      TickMarks = tmBoth
      TickStyle = tsAuto
      OnChange = TrackResChange
    end
    object TrackY: TTrackBar
      Left = 48
      Top = 48
      Width = 169
      Height = 33
      Max = 100
      Orientation = trHorizontal
      Frequency = 10
      Position = 50
      SelEnd = 0
      SelStart = 0
      TabOrder = 2
      ThumbLength = 12
      TickMarks = tmBoth
      TickStyle = tsAuto
    end
    object TrackX: TTrackBar
      Left = 48
      Top = 16
      Width = 169
      Height = 33
      Max = 100
      Orientation = trHorizontal
      Frequency = 10
      Position = 50
      SelEnd = 0
      SelStart = 0
      TabOrder = 3
      ThumbLength = 12
      TickMarks = tmBoth
      TickStyle = tsAuto
    end
    object PanelBkg: TPanel
      Left = 64
      Top = 160
      Width = 33
      Height = 25
      BevelOuter = bvLowered
      Color = clBlack
      TabOrder = 4
      OnClick = PanelBkgClick
    end
    object PanelPen: TPanel
      Left = 176
      Top = 160
      Width = 33
      Height = 25
      BevelOuter = bvLowered
      Color = clWhite
      TabOrder = 5
      OnClick = PanelPenClick
    end
  end
  object GroupBox2: TGroupBox
    Left = 8
    Top = 208
    Width = 185
    Height = 105
    Caption = 'Circle Property'
    TabOrder = 5
    object Label5: TLabel
      Left = 8
      Top = 20
      Width = 33
      Height = 13
      Caption = 'Radius'
    end
    object TrackRad: TTrackBar
      Left = 8
      Top = 40
      Width = 169
      Height = 33
      Max = 50
      Min = 10
      Orientation = trHorizontal
      Frequency = 1
      Position = 30
      SelEnd = 0
      SelStart = 0
      TabOrder = 0
      ThumbLength = 12
      TickMarks = tmBoth
      TickStyle = tsAuto
      OnChange = TrackRadChange
    end
  end
  object GroupBox3: TGroupBox
    Left = 200
    Top = 208
    Width = 305
    Height = 105
    Caption = 'Spectrum Propeties'
    TabOrder = 6
    object Label8: TLabel
      Left = 72
      Top = 72
      Width = 28
      Height = 13
      Caption = 'Width'
    end
    object Label9: TLabel
      Left = 8
      Top = 40
      Width = 57
      Height = 13
      Caption = 'Peack color'
    end
    object Label10: TLabel
      Left = 136
      Top = 16
      Width = 48
      Height = 13
      Caption = 'Line falloff'
    end
    object Label11: TLabel
      Left = 208
      Top = 16
      Width = 53
      Height = 13
      Caption = 'Peak falloff'
    end
    object SpecTrackWidth: TTrackBar
      Left = 112
      Top = 64
      Width = 169
      Height = 33
      Max = 12
      Min = 1
      Orientation = trHorizontal
      Frequency = 1
      Position = 5
      SelEnd = 0
      SelStart = 0
      TabOrder = 0
      ThumbLength = 12
      TickMarks = tmBoth
      TickStyle = tsAuto
      OnChange = SpecTrackWidthChange
    end
    object PanelPeakColor: TPanel
      Left = 72
      Top = 40
      Width = 33
      Height = 25
      BevelOuter = bvLowered
      Color = clYellow
      TabOrder = 1
      OnClick = PanelPeakColorClick
    end
    object SpinLineFall: TSpinEdit
      Left = 136
      Top = 32
      Width = 49
      Height = 22
      MaxValue = 10
      MinValue = 1
      TabOrder = 2
      Value = 3
      OnChange = SpinLineFallChange
    end
    object SpinPeakFall: TSpinEdit
      Left = 208
      Top = 32
      Width = 49
      Height = 22
      MaxValue = 10
      MinValue = 1
      TabOrder = 3
      Value = 1
      OnChange = SpinPeakFallChange
    end
    object CheckSpecPeaks: TCheckBox
      Left = 8
      Top = 16
      Width = 81
      Height = 17
      Caption = 'Show Peaks'
      Checked = True
      State = cbChecked
      TabOrder = 4
      OnClick = CheckSpecPeaksClick
    end
  end
  object Button3: TButton
    Left = 176
    Top = 72
    Width = 75
    Height = 25
    Caption = 'No backg'
    TabOrder = 7
    OnClick = Button3Click
  end
  object OpenDialog: TOpenDialog
    Left = 288
  end
  object TimerRender: TTimer
    Interval = 33
    OnTimer = TimerRenderTimer
    Left = 256
  end
  object ColorDialog: TColorDialog
    Ctl3D = True
    Left = 320
  end
  object OpenPictureDialog: TOpenPictureDialog
    Left = 352
  end
end
