object ManualCompleteForm: TManualCompleteForm
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Manual work complete'
  ClientHeight = 544
  ClientWidth = 660
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -16
  Font.Name = 'Tahoma'
  Font.Style = []
  Position = poMainFormCenter
  Scaled = False
  OnCloseQuery = FormCloseQuery
  PixelsPerInch = 96
  TextHeight = 19
  object pnlTitle: TPanel
    Left = 0
    Top = 0
    Width = 660
    Height = 48
    Align = alTop
    BevelOuter = bvNone
    Caption = 'Manual work complete'
    Color = clGray
    Font.Color = clWhite
    Font.Height = -20
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentBackground = False
    ParentFont = False
    TabOrder = 0
  end
  object lblSource: TLabel
    Left = 20
    Top = 66
    Width = 170
    Height = 26
    AutoSize = False
    Caption = 'Source Tray'
  end
  object lblTarget: TLabel
    Left = 20
    Top = 106
    Width = 170
    Height = 26
    AutoSize = False
    Caption = 'Target Tray'
  end
  object lblCell: TLabel
    Left = 20
    Top = 146
    Width = 170
    Height = 26
    AutoSize = False
    Caption = 'Cell ID'
  end
  object lblChannel: TLabel
    Left = 20
    Top = 186
    Width = 170
    Height = 26
    AutoSize = False
    Caption = 'Target Channel'
  end
  object lblFmsState: TLabel
    Left = 20
    Top = 282
    Width = 620
    Height = 50
    AutoSize = False
    Caption = 'FMS Request / Response pending'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clMaroon
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    WordWrap = True
  end
  object lblStatus: TLabel
    Left = 20
    Top = 338
    Width = 620
    Height = 104
    AutoSize = False
    WordWrap = True
  end
  object editSource: TEdit
    Left = 200
    Top = 62
    Width = 440
    Height = 27
    ReadOnly = True
    TabOrder = 1
  end
  object editTarget: TEdit
    Left = 200
    Top = 102
    Width = 440
    Height = 27
    ReadOnly = True
    TabOrder = 2
  end
  object editCell: TEdit
    Left = 200
    Top = 142
    Width = 440
    Height = 27
    ReadOnly = True
    TabOrder = 3
  end
  object editChannel: TEdit
    Left = 200
    Top = 182
    Width = 100
    Height = 27
    MaxLength = 2
    NumbersOnly = True
    TabOrder = 4
  end
  object chkInserted: TCheckBox
    Left = 20
    Top = 226
    Width = 620
    Height = 48
    Caption = 'I confirmed the cell is inserted at this Target channel and the gripper is empty.'
    WordWrap = True
    TabOrder = 5
  end
  object btnReport: TButton
    Left = 20
    Top = 450
    Width = 300
    Height = 38
    Caption = 'Confirm and report'
    TabOrder = 6
    OnClick = btnReportClick
  end
  object btnRetry: TButton
    Left = 332
    Top = 450
    Width = 148
    Height = 38
    Caption = 'Retry'
    TabOrder = 7
    OnClick = btnRetryClick
  end
  object btnClose: TButton
    Left = 492
    Top = 450
    Width = 148
    Height = 38
    Caption = 'Close'
    TabOrder = 8
    OnClick = btnCloseClick
  end
  object btnResume: TButton
    Left = 20
    Top = 496
    Width = 620
    Height = 38
    Caption = 'Return to standby and resume'
    TabOrder = 9
    OnClick = btnResumeClick
  end
  object pollTimer: TTimer
    Interval = 100
    OnTimer = pollTimerTimer
    Left = 594
    Top = 172
  end
end
