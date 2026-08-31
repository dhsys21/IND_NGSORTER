object DryRunForm: TDryRunForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'DRY RUN - INSPECTION ONLY'
  ClientHeight = 450
  ClientWidth = 680
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnCloseQuery = FormCloseQuery
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object lblTitle: TLabel
    Left = 16
    Top = 16
    Width = 291
    Height = 23
    Caption = 'DRY RUN - INSPECTION ONLY'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -19
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblStartChannel: TLabel
    Left = 18
    Top = 61
    Width = 82
    Height = 16
    Caption = 'Start Channel'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblEndChannel: TLabel
    Left = 190
    Top = 61
    Width = 76
    Height = 16
    Caption = 'End Channel'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblSpeed: TLabel
    Left = 18
    Top = 103
    Width = 136
    Height = 16
    Caption = 'Speed (Acc/Dec=300)'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblCurrentChannel: TLabel
    Left = 18
    Top = 143
    Width = 101
    Height = 16
    Caption = 'Current channel : -'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblCurrentStep: TLabel
    Left = 18
    Top = 169
    Width = 175
    Height = 16
    Caption = 'Current step : Waiting for START'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblInterlock: TLabel
    Left = 18
    Top = 195
    Width = 151
    Height = 14
    Caption = 'D10104 Source Centering=OFF'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object editStartChannel: TEdit
    Left = 108
    Top = 57
    Width = 61
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    Text = '1'
  end
  object editEndChannel: TEdit
    Left = 278
    Top = 57
    Width = 61
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
    Text = '96'
  end
  object editSpeed: TEdit
    Left = 164
    Top = 99
    Width = 61
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    Text = '1000'
  end
  object btnStart: TButton
    Left = 365
    Top = 52
    Width = 92
    Height = 36
    Caption = 'START'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
    OnClick = btnStartClick
  end
  object btnStop: TButton
    Left = 463
    Top = 52
    Width = 92
    Height = 36
    Caption = 'STOP'
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 4
    OnClick = btnStopClick
  end
  object btnClose: TButton
    Left = 561
    Top = 52
    Width = 92
    Height = 36
    Caption = 'CLOSE'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 5
    OnClick = btnCloseClick
  end
  object btnWaitPosition: TButton
    Left = 365
    Top = 96
    Width = 190
    Height = 32
    Caption = 'WAIT POSITION'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 6
    OnClick = btnWaitPositionClick
  end
  object pnlRunState: TPanel
    Left = 365
    Top = 12
    Width = 288
    Height = 30
    BevelOuter = bvLowered
    Caption = 'IDLE'
    Color = clSilver
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentBackground = False
    ParentFont = False
    TabOrder = 7
  end
  object memoDryRun: TMemo
    Left = 16
    Top = 222
    Width = 637
    Height = 209
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Consolas'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 8
    WordWrap = False
  end
  object dryRunTimer: TTimer
    Enabled = False
    Interval = 50
    OnTimer = dryRunTimerTimer
    Left = 632
    Top = 152
  end
end
