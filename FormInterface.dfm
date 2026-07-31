object InterfaceForm: TInterfaceForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'PLC / MES INTERFACE'
  ClientHeight = 846
  ClientWidth = 1364
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox_PLC_PC: TGroupBox
    Left = 5
    Top = 10
    Width = 950
    Height = 343
    Caption = 'PLC INTERFACE'
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentBackground = False
    ParentColor = False
    ParentFont = False
    TabOrder = 0
    OnDblClick = GroupBox_PLC_PCDblClick
    object ListView_PLC: TListView
      Left = 10
      Top = 32
      Width = 460
      Height = 300
      Columns = <
        item
          Caption = 'PLC_Address'
          Width = 150
        end
        item
          Caption = 'PLC_Name'
          Width = 150
        end
        item
          Caption = 'PLC_Value'
          Width = 150
        end>
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      GridLines = True
      ReadOnly = True
      RowSelect = True
      ParentFont = False
      TabOrder = 0
      ViewStyle = vsReport
    end
    object ListView_PC: TListView
      Left = 480
      Top = 32
      Width = 460
      Height = 300
      Columns = <
        item
          Caption = 'PC_Address'
          Width = 150
        end
        item
          Caption = 'PC_Name'
          Width = 150
        end
        item
          Caption = 'PC_Value'
          Width = 150
        end>
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      GridLines = True
      ReadOnly = True
      RowSelect = True
      ParentFont = False
      TabOrder = 1
      ViewStyle = vsReport
    end
  end
  object GroupBox_FMS: TGroupBox
    Left = 5
    Top = 363
    Width = 950
    Height = 480
    Caption = 'MES INTERFACE'
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentBackground = False
    ParentColor = False
    ParentFont = False
    TabOrder = 1
    OnDblClick = GroupBox_PLC_PCDblClick
    object ListView_FMS_TAG: TListView
      Left = 10
      Top = 32
      Width = 460
      Height = 440
      Columns = <
        item
          Caption = 'FMS_TAG'
          Width = 200
        end
        item
          Caption = 'FMS_Value'
          Width = 250
        end>
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      GridLines = True
      ReadOnly = True
      RowSelect = True
      ParentFont = False
      TabOrder = 0
      ViewStyle = vsReport
    end
    object ListView_PC_TAG: TListView
      Left = 480
      Top = 32
      Width = 460
      Height = 440
      Columns = <
        item
          Caption = 'PC_TAG'
          Width = 200
        end
        item
          Caption = 'PC_Value'
          Width = 250
        end>
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      GridLines = True
      ReadOnly = True
      RowSelect = True
      ParentFont = False
      TabOrder = 1
      ViewStyle = vsReport
    end
  end
  object pnlPlcTest: TPanel
    Left = 970
    Top = 10
    Width = 380
    Height = 343
    Color = clWhite
    ParentBackground = False
    TabOrder = 2
    object lblPlcTest: TLabel
      Left = 7
      Top = 7
      Width = 78
      Height = 19
      Caption = 'PLC TEST'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = [fsBold, fsUnderline]
      ParentFont = False
    end
    object gbPlcTest: TGroupBox
      Left = 25
      Top = 64
      Width = 330
      Height = 242
      Caption = 'Write Value'
      TabOrder = 0
      object lblPlcAddr: TLabel
        Left = 16
        Top = 34
        Width = 55
        Height = 16
        Caption = 'ADDRESS'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object lblPlcValue: TLabel
        Left = 16
        Top = 98
        Width = 37
        Height = 16
        Caption = 'VALUE'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object cbAddress: TComboBox
        Left = 16
        Top = 56
        Width = 225
        Height = 21
        TabOrder = 0
        Text = '10052'
        Items.Strings = (
          '10050'
          '10051'
          '10052'
          '10053'
          '10054'
          '10055'
          '10056'
          '10057'
          '10058'
          '')
      end
      object editPcValue: TEdit
        Left = 16
        Top = 121
        Width = 225
        Height = 21
        TabOrder = 1
        Text = '1'
      end
      object btnPlcWriteValue: TButton
        Left = 223
        Top = 168
        Width = 83
        Height = 54
        Caption = 'Write Value'
        TabOrder = 2
        OnClick = btnPlcWriteValueClick
      end
    end
  end
  object pnlMesTest: TPanel
    Left = 970
    Top = 363
    Width = 380
    Height = 480
    Color = clWhite
    ParentBackground = False
    TabOrder = 3
    object lblFmsTest: TLabel
      Left = 15
      Top = 15
      Width = 81
      Height = 19
      Caption = 'MES TEST'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = [fsBold, fsUnderline]
      ParentFont = False
    end
    object gbFmsWriteValues: TGroupBox
      Left = 25
      Top = 59
      Width = 330
      Height = 150
      Caption = 'Write Value'
      TabOrder = 0
      object lblFmsTag: TLabel
        Left = 16
        Top = 28
        Width = 24
        Height = 16
        Caption = 'TAG'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object lblFmsValue: TLabel
        Left = 16
        Top = 91
        Width = 37
        Height = 16
        Caption = 'VALUE'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object cbMesTag: TComboBox
        Left = 16
        Top = 50
        Width = 225
        Height = 21
        TabOrder = 0
        Text = '38000'
        Items.Strings = (
          '38000'
          '38001'
          '38002'
          '38003'
          '38004'
          '38005'
          '38006'
          '38007'
          '38010'
          '38011'
          '38013'
          '38015'
          '38017'
          '')
      end
      object editFmsValue: TEdit
        Left = 16
        Top = 114
        Width = 185
        Height = 21
        TabOrder = 1
        Text = '1'
      end
      object btnWriteMesValue: TButton
        Left = 235
        Top = 93
        Width = 83
        Height = 42
        Caption = 'Write Value'
        TabOrder = 2
      end
    end
    object gbFmsIrOcvValue: TGroupBox
      Left = 25
      Top = 221
      Width = 330
      Height = 145
      Caption = 'TRAY LOAD TEST'
      TabOrder = 1
      object lblIrValue: TLabel
        Left = 16
        Top = 24
        Width = 97
        Height = 16
        Caption = 'SOURCE TRAY ID'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object lblOcvValue: TLabel
        Left = 16
        Top = 82
        Width = 96
        Height = 16
        Caption = 'TARGET TRAY ID'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object editIR: TEdit
        Left = 16
        Top = 42
        Width = 190
        Height = 21
        TabOrder = 0
        Text = 'SOURCE_TEST'
      end
      object editOCV: TEdit
        Left = 16
        Top = 100
        Width = 190
        Height = 21
        TabOrder = 1
        Text = 'TARGET_TEST'
      end
      object btnSourceTrayLoad: TButton
        Left = 214
        Top = 36
        Width = 100
        Height = 32
        Caption = 'Source Load'
        TabOrder = 2
      end
      object btnTargetTrayLoad: TButton
        Left = 214
        Top = 94
        Width = 100
        Height = 32
        Caption = 'Target Load'
        TabOrder = 3
      end
    end
    object gbIrocvNgValue: TGroupBox
      Left = 25
      Top = 376
      Width = 330
      Height = 82
      Caption = 'TARGET NG CELL LIST'
      TabOrder = 2
      object editNgList: TEdit
        Left = 16
        Top = 32
        Width = 190
        Height = 21
        TabOrder = 0
        Text = '1,13,25,37,49,61,73,85'
      end
      object btnWriteTargetData: TButton
        Left = 214
        Top = 24
        Width = 100
        Height = 36
        Caption = 'Write Track Out'
        TabOrder = 1
      end
    end
  end
  object Timer_PLC_Update: TTimer
    Enabled = False
    Interval = 500
    OnTimer = Timer_PLC_UpdateTimer
    Left = 250
    Top = 112
  end
  object Timer_MES_Update: TTimer
    Enabled = False
    Interval = 500
    Left = 234
    Top = 464
  end
end
