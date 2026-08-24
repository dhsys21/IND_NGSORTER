object AlarmForm_fms: TAlarmForm_fms
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = 'FMS Alarm'
  ClientHeight = 360
  ClientWidth = 720
  Color = 15132390
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  Scaled = False
  OnHide = FormHide
  PixelsPerInch = 96
  TextHeight = 16
  object pnlTitle: TPanel
    Left = 0
    Top = 0
    Width = 720
    Height = 56
    Align = alTop
    BevelOuter = bvNone
    Color = 4210816
    ParentBackground = False
    TabOrder = 0
    object lblTitle: TLabel
      Left = 20
      Top = 15
      Width = 111
      Height = 25
      Caption = 'FMS ALARM'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWhite
      Font.Height = -21
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
  end
  object memoDetail: TMemo
    Left = 20
    Top = 72
    Width = 680
    Height = 130
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 1
  end
  object lblRequestTitle: TLabel
    Left = 20
    Top = 218
    Width = 57
    Height = 16
    Caption = 'Request:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblRequest: TLabel
    Left = 92
    Top = 218
    Width = 53
    Height = 16
    Caption = 'Request'
  end
  object lblResponseTitle: TLabel
    Left = 520
    Top = 218
    Width = 65
    Height = 16
    Caption = 'Response:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblResponse: TLabel
    Left = 600
    Top = 218
    Width = 7
    Height = 16
    Caption = '0'
  end
  object lblStatus: TLabel
    Left = 20
    Top = 248
    Width = 680
    Height = 40
    AutoSize = False
    Caption = 'Correct the FMS condition, then press Retry.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 4210816
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    WordWrap = True
  end
  object btnBuzzerStop: TButton
    Left = 20
    Top = 300
    Width = 140
    Height = 44
    Caption = 'Buzzer stop'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    OnClick = btnBuzzerStopClick
  end
  object btnRetry: TButton
    Left = 560
    Top = 300
    Width = 140
    Height = 44
    Caption = 'Retry'
    Default = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
    OnClick = btnRetryClick
  end
end
