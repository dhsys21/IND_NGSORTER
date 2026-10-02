object ProductionForm: TProductionForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Production'
  ClientHeight = 900
  ClientWidth = 860
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'Malgun Gothic'
  Font.Style = []
  Position = poScreenCenter
  OnHide = FormHide
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 17
  object pnlHeader: TPanel
    Left = 0
    Top = 0
    Width = 860
    Height = 56
    Align = alTop
    BevelOuter = bvNone
    Color = 7494432
    ParentBackground = False
    TabOrder = 0
    object lblTitle: TLabel
      Left = 20
      Top = 15
      Width = 610
      Height = 27
      AutoSize = False
      Caption = 'Production'
      Font.Color = clWhite
      Font.Height = -22
      Font.Name = 'Malgun Gothic'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object btnClose: TButton
      Left = 750
      Top = 11
      Width = 90
      Height = 34
      Caption = 'Close'
      TabOrder = 0
      OnClick = btnCloseClick
    end
  end
  object tabsPeriod: TTabControl
    Left = 20
    Top = 72
    Width = 820
    Height = 34
    TabOrder = 1
    Tabs.Strings = (
      'Daily'
      'Weekly (Mon - Sun)'
      'Monthly')
    TabIndex = 0
    TabWidth = 265
    OnChange = PeriodChange
  end
  object dateProduction: TDateTimePicker
    Left = 64
    Top = 118
    Width = 150
    Height = 30
    Date = 46296.000000000000000000
    Time = 0.000000000000000000
    Format = 'yyyy-MM-dd'
    TabOrder = 3
    OnChange = PeriodChange
  end
  object btnPrevious: TButton
    Left = 20
    Top = 118
    Width = 36
    Height = 30
    Caption = '<'
    ShowHint = True
    TabOrder = 2
    OnClick = btnPreviousClick
  end
  object btnNext: TButton
    Left = 222
    Top = 118
    Width = 36
    Height = 30
    Caption = '>'
    ShowHint = True
    TabOrder = 4
    OnClick = btnNextClick
  end
  object btnToday: TButton
    Left = 270
    Top = 118
    Width = 80
    Height = 30
    Caption = 'Today'
    TabOrder = 5
    OnClick = btnTodayClick
  end
  object btnRefresh: TButton
    Left = 750
    Top = 118
    Width = 90
    Height = 30
    Caption = 'Refresh'
    TabOrder = 6
    OnClick = btnRefreshClick
  end
  object lblRange: TLabel
    Left = 368
    Top = 123
    Width = 370
    Height = 22
    AutoSize = False
    Caption = '2026-10-01'
  end
  object lblTotal: TLabel
    Left = 20
    Top = 163
    Width = 820
    Height = 45
    AutoSize = False
    Caption = 'Total   Trays: 0    Cells: 0'#13#10'OK: 0    NG: 0'
    Font.Color = 7494432
    Font.Height = -16
    Font.Name = 'Malgun Gothic'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object gridProduction: TStringGrid
    Left = 20
    Top = 216
    Width = 820
    Height = 580
    ColCount = 5
    DefaultColWidth = 230
    DefaultDrawing = False
    DefaultRowHeight = 22
    FixedCols = 0
    RowCount = 25
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect, goThumbTracking]
    TabOrder = 7
    OnDrawCell = gridProductionDrawCell
    ColWidths = (
      250
      130
      140
      135
      135)
  end
  object lblStatus: TLabel
    Left = 20
    Top = 808
    Width = 820
    Height = 34
    AutoSize = False
    Caption = 'Updated: -'
    Font.Color = clGrayText
    Font.Height = -13
    Font.Name = 'Malgun Gothic'
    Font.Style = []
    ParentFont = False
    WordWrap = True
  end
  object lblBasis: TLabel
    Left = 20
    Top = 848
    Width = 820
    Height = 43
    AutoSize = False
    Caption = 'Automatic completion (FMS ProcessEnd OK); occupied cells include final NG.'#13#10'Retests count once. Manual / bypass / CYCLE tests excluded. Earlier history is not imported.'
    Font.Color = clGrayText
    Font.Height = -13
    Font.Name = 'Malgun Gothic'
    Font.Style = []
    ParentFont = False
    WordWrap = True
  end
  object timerRefresh: TTimer
    Enabled = False
    Interval = 30000
    OnTimer = timerRefreshTimer
    Left = 704
    Top = 16
  end
end
