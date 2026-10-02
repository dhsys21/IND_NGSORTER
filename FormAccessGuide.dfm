object AccessGuideForm: TAccessGuideForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Access permissions'
  ClientHeight = 724
  ClientWidth = 760
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'Tahoma'
  Font.Style = []
  Position = poScreenCenter
  Scaled = False
  PixelsPerInch = 96
  TextHeight = 16
  object pnlPermissions: TPanel
    Left = 0
    Top = 0
    Width = 760
    Height = 724
    Align = alClient
    BevelOuter = bvNone
    Color = clWhite
    ParentBackground = False
    TabOrder = 0
    object lblTitle: TLabel
      Left = 16
      Top = 12
      Width = 728
      Height = 28
      AutoSize = False
      Caption = 'Access permissions'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -20
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object gridPermissions: TStringGrid
      Left = 16
      Top = 48
      Width = 728
      Height = 532
      ColCount = 4
      DefaultColWidth = 108
      DefaultRowHeight = 23
      FixedCols = 1
      RowCount = 22
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSelect]
      ScrollBars = ssVertical
      TabOrder = 0
      ColWidths = (
        380
        108
        108
        108)
    end
    object lblNotes: TLabel
      Left = 16
      Top = 590
      Width = 728
      Height = 72
      AutoSize = False
      Caption = 'Level permission does not bypass safety interlocks. Viewing and stopping do not require login.'
      WordWrap = True
    end
    object btnClose: TButton
      Left = 648
      Top = 674
      Width = 96
      Height = 34
      Cancel = True
      Caption = 'Close'
      Default = True
      ModalResult = 2
      TabOrder = 1
    end
  end
end
