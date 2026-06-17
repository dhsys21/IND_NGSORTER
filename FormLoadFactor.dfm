object loadfactorForm: TloadfactorForm
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'LoadFactor Information'
  ClientHeight = 234
  ClientWidth = 394
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Scaled = False
  PixelsPerInch = 96
  TextHeight = 13
  object pnlXAxis: TPanel
    Left = 12
    Top = 10
    Width = 75
    Height = 24
    BevelKind = bkFlat
    BevelOuter = bvNone
    Caption = 'X '#52629
    Color = 15269887
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 13996080
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentBackground = False
    ParentFont = False
    ParentShowHint = False
    ShowHint = False
    TabOrder = 0
  end
  object px1: TPanel
    Left = 88
    Top = 10
    Width = 100
    Height = 24
    BevelKind = bkFlat
    BevelOuter = bvNone
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentBackground = False
    ParentFont = False
    ParentShowHint = False
    ShowHint = False
    TabOrder = 1
  end
  object Edit_LimitValue: TEdit
    Left = 270
    Top = 10
    Width = 100
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ImeName = 'Microsoft IME 2010'
    ParentFont = False
    TabOrder = 2
    Text = '100'
  end
  object pnlLimitValue: TPanel
    Left = 194
    Top = 10
    Width = 75
    Height = 24
    BevelOuter = bvNone
    Caption = #54620#44228' '#44050
    Color = 13996080
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentBackground = False
    ParentFont = False
    ParentShowHint = False
    ShowHint = False
    TabOrder = 3
  end
  object pnlYAxis: TPanel
    Left = 12
    Top = 46
    Width = 75
    Height = 24
    BevelKind = bkFlat
    BevelOuter = bvNone
    Caption = 'Y '#52629
    Color = 15269887
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 13996080
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentBackground = False
    ParentFont = False
    ParentShowHint = False
    ShowHint = False
    TabOrder = 4
  end
  object py: TPanel
    Left = 88
    Top = 46
    Width = 100
    Height = 24
    BevelKind = bkFlat
    BevelOuter = bvNone
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentBackground = False
    ParentFont = False
    ParentShowHint = False
    ShowHint = False
    TabOrder = 5
  end
  object pnlZAxis: TPanel
    Left = 12
    Top = 82
    Width = 75
    Height = 24
    BevelKind = bkFlat
    BevelOuter = bvNone
    Caption = 'Z '#52629
    Color = 15269887
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 13996080
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentBackground = False
    ParentFont = False
    ParentShowHint = False
    ShowHint = False
    TabOrder = 6
  end
  object pz: TPanel
    Left = 88
    Top = 82
    Width = 100
    Height = 24
    BevelKind = bkFlat
    BevelOuter = bvNone
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentBackground = False
    ParentFont = False
    ParentShowHint = False
    ShowHint = False
    TabOrder = 7
  end
  object AdvSmoothButton_Save: TAdvSmoothButton
    Left = 194
    Top = 38
    Width = 85
    Height = 68
    Appearance.PictureAlignment = taCenter
    Appearance.Font.Charset = DEFAULT_CHARSET
    Appearance.Font.Color = clBlack
    Appearance.Font.Height = -13
    Appearance.Font.Name = 'Tahoma'
    Appearance.Font.Style = [fsBold]
    Appearance.Spacing = 0
    Appearance.Rounding = 3
    Appearance.WordWrapping = False
    Status.Caption = '0'
    Status.Appearance.Fill.Color = 16744448
    Status.Appearance.Fill.ColorMirror = clNone
    Status.Appearance.Fill.ColorMirrorTo = clNone
    Status.Appearance.Fill.GradientType = gtSolid
    Status.Appearance.Fill.GradientMirrorType = gtSolid
    Status.Appearance.Fill.BorderColor = clGray
    Status.Appearance.Fill.Rounding = 8
    Status.Appearance.Fill.ShadowOffset = 0
    Status.Appearance.Fill.Glow = gmNone
    Status.Appearance.Font.Charset = DEFAULT_CHARSET
    Status.Appearance.Font.Color = clWhite
    Status.Appearance.Font.Height = -11
    Status.Appearance.Font.Name = 'Tahoma'
    Status.Appearance.Font.Style = []
    BevelColor = clBlack
    Caption = #51200#51109
    Color = clWhite
    ParentFont = False
    TabOrder = 8
    Version = '2.1.1.5'
    OnClick = AdvSmoothButton_SaveClick
    TMSStyle = 8
  end
  object AdvSmoothButton_Cancel: TAdvSmoothButton
    Left = 285
    Top = 38
    Width = 85
    Height = 68
    Appearance.PictureAlignment = taCenter
    Appearance.Font.Charset = DEFAULT_CHARSET
    Appearance.Font.Color = clBlack
    Appearance.Font.Height = -13
    Appearance.Font.Name = 'Tahoma'
    Appearance.Font.Style = [fsBold]
    Appearance.Spacing = 0
    Appearance.Rounding = 3
    Appearance.WordWrapping = False
    Status.Caption = '0'
    Status.Appearance.Fill.Color = 16744448
    Status.Appearance.Fill.ColorMirror = clNone
    Status.Appearance.Fill.ColorMirrorTo = clNone
    Status.Appearance.Fill.GradientType = gtSolid
    Status.Appearance.Fill.GradientMirrorType = gtSolid
    Status.Appearance.Fill.BorderColor = clGray
    Status.Appearance.Fill.Rounding = 8
    Status.Appearance.Fill.ShadowOffset = 0
    Status.Appearance.Fill.Glow = gmNone
    Status.Appearance.Font.Charset = DEFAULT_CHARSET
    Status.Appearance.Font.Color = clWhite
    Status.Appearance.Font.Height = -11
    Status.Appearance.Font.Name = 'Tahoma'
    Status.Appearance.Font.Style = []
    BevelColor = clBlack
    Caption = #52712#49548
    Color = clWhite
    ParentFont = False
    TabOrder = 9
    Version = '2.1.1.5'
    OnClick = AdvSmoothButton_CancelClick
    TMSStyle = 8
  end
  object Memo_Ko: TMemo
    Left = 31
    Top = 151
    Width = 76
    Height = 89
    Lines.Strings = (
      'X1'#52629
      'X2'#52629
      'Y'#52629
      'Z'#52629' '
      #54620#44228' '#44050
      #51200#51109
      #52712#49548)
    ScrollBars = ssBoth
    TabOrder = 10
    Visible = False
  end
  object Memo_En: TMemo
    Left = 153
    Top = 151
    Width = 84
    Height = 89
    Lines.Strings = (
      'X1 Axis'
      'X2 Axis'
      'Y Axis'
      'Z Axis'
      'Limit value'
      'Save'
      'Cancel')
    ScrollBars = ssBoth
    TabOrder = 11
    Visible = False
  end
  object Memo_Hu: TMemo
    Left = 294
    Top = 151
    Width = 76
    Height = 89
    Lines.Strings = (
      'X1 tengely'
      'X2 tengely'
      'Y tengely'
      'Z tengely'
      'Limit value'
      'Save'
      'Cancel')
    ScrollBars = ssBoth
    TabOrder = 12
    Visible = False
  end
end
