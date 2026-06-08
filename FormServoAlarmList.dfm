object ServoAlarmListForm: TServoAlarmListForm
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'SERVO Alarm List'
  ClientHeight = 713
  ClientWidth = 649
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Scaled = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 313
    Height = 345
    Caption = #49884#49828#53596' '#50508#46988
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    object ListView1: TListView
      Left = 16
      Top = 31
      Width = 281
      Height = 300
      Columns = <
        item
          Width = 0
        end
        item
          Caption = 'No.'
        end
        item
          Caption = 'Name'
          Width = 205
        end>
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      GridLines = True
      HideSelection = False
      ReadOnly = True
      RowSelect = True
      ParentFont = False
      TabOrder = 0
      ViewStyle = vsReport
    end
    object Memo_Ko1: TMemo
      Left = 110
      Top = 23
      Width = 200
      Height = 89
      Lines.Strings = (
        '35'
        '36'
        '37'
        '38'
        #50672#49328' '#51452#44592' '#50508#46988
        #54540#47000#49884' ROM '#50416#44592' '#54943#49688' '#50640#47084
        #54028#46972#48120#53552' '#51060#49345
        #47560#53356' '#44160#52636' '#49444#51221' '#48512#51221
        #49884#49828#53596' '#50508#46988)
      ScrollBars = ssBoth
      TabOrder = 1
      Visible = False
    end
    object Memo_En1: TMemo
      Left = 110
      Top = 118
      Width = 200
      Height = 89
      Lines.Strings = (
        '35'
        '36'
        '37'
        '38'
        'Operation cycle alarm'
        'Number of write accesses to flash ROM error'
        'Parameter error'
        'Mark detection setting error'
        'System alarm')
      ScrollBars = ssBoth
      TabOrder = 2
      Visible = False
    end
    object Memo_Hu1: TMemo
      Left = 110
      Top = 213
      Width = 200
      Height = 89
      Lines.Strings = (
        '35'
        '36'
        '37'
        '38'
        'Operation cycle alarm'
        'Number of write accesses to flash ROM error'
        'Parameter error'
        'Mark detection setting error'
        'System alarm')
      ScrollBars = ssBoth
      TabOrder = 3
      Visible = False
    end
  end
  object GroupBox2: TGroupBox
    Left = 327
    Top = 8
    Width = 313
    Height = 345
    Caption = #49436#48372' '#50508#46988
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
    object ListView2: TListView
      Left = 16
      Top = 31
      Width = 281
      Height = 300
      Columns = <
        item
          Width = 0
        end
        item
          Caption = 'No.'
        end
        item
          Caption = 'Name'
          Width = 227
        end>
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      GridLines = True
      HideSelection = False
      ReadOnly = True
      RowSelect = True
      ParentFont = False
      TabOrder = 0
      ViewStyle = vsReport
    end
    object Memo_Ko2: TMemo
      Left = 110
      Top = 23
      Width = 200
      Height = 89
      Lines.Strings = (
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '19'
        '1A'
        '1E'
        '1F'
        '20'
        '21'
        '24'
        '25'
        '27'
        '28'
        '2A'
        '2B'
        '30'
        '31'
        '32'
        '33'
        '34'
        '35'
        '36'
        '37'
        '3A'
        '3D'
        '3E'
        '42'
        '45'
        '46'
        '47'
        '50'
        '51'
        '52'
        '54'
        '56'
        '63'
        '70'
        '71'
        '72'
        '82'
        '8A'
        '8E'
        '888'
        '91'
        '92'
        '95'
        '96'
        '9F'
        'E0'
        'E1'
        'E2'
        'E3'
        'E4'
        'E6'
        'E7'
        'E8'
        'E9'
        'EB'
        'EC'
        'ED'
        'F0'
        'F2'
        'F3'
        #48512#51313' '#51204#50517
        #49828#50948#52824' '#49444#51221' '#51060#49345
        #47700#47784#47532' '#51060#49345' 1(RAM)'
        #53364#47197' '#51060#49345
        #51228#50612' '#52376#47532' '#51060#49345
        #47700#47784#47532' '#51060#49345' 2(EEP-ROM)'
        #50644#53076#45908' '#52488#44592' '#53685#49888' '#51060#49345' 1'
        #44592#54032' '#51060#49345
        #47700#47784#47532' '#51060#49345' 3(FLASH-ROM)'
        #49436#48372#47784#53552' '#51312#54633' '#51060#49345
        #50644#53076#45908' '#52488#44592' '#53685#49888' '#51060#49345' 2'
        #50644#53076#45908' '#52488#44592' '#53685#49888' '#51060#49345' 3'
        #50644#53076#45908' '#53685#49345' '#53685#49888' '#51060#49345' 1'
        #50644#53076#45908' '#53685#49345' '#53685#49888' '#51060#49345' 2'
        #51452#54924#47196' '#51060#49345
        #51208#45824#50948#52824' '#49548#49892
        #52488#44592' '#51088#44537' '#44160#52636' '#51060#49345
        #47532#45768#50612' '#50644#53076#45908' '#51060#49345' 2'
        #47532#45768#50612' '#50644#53076#45908' '#51060#49345' 1'
        #50644#53076#45908' '#52852#50868#53552' '#51060#49345
        #54924#49373' '#51060#49345
        #44284#49549#46020
        #44284#51204#47448
        #44284#51204#50517
        'SSCNET '#49688#49888' '#51060#49345' 1'
        #51648#47161' '#51452#54028#49688' '#51060#49345
        'SSCNET '#49688#49888' '#51060#49345' 2'
        #54028#46972#48120#53552' '#51060#49345
        #46028#51077#51204#47448' '#50613#51228#54924#47196' '#51060#49345
        #46300#46972#51060#48260#44036' '#53685#49888#50857' '#54028#46972#48120#53552' '#49444#51221' '#51060#49345
        #50868#51204' '#47784#46300' '#51060#49345
        #49436#48372' '#51228#50612' '#51060#49345
        #51452#54924#47196' '#49548#51088' '#44284#50676
        #49436#48372#47784#53552' '#44284#50676
        #45257#44033' '#54060' '#51060#49345
        #44284#48512#54616' 1'
        #44284#48512#54616' 2'
        #50724#52264' '#44284#45824
        #48156#51652' '#44160#51648
        #44053#51228' '#51221#51648' '#51060#49345
        'STO '#53440#51060#48141' '#51060#49345
        #44592#44228#45800' '#50644#53076#45908' '#52488#44592' '#53685#49888' '#51060#49345' 1'
        #44592#44228#45800' '#50644#53076#45908' '#53685#49345' '#53685#49888' '#51060#49345' 1'
        #44592#44228#45800' '#50644#53076#45908' '#53685#49345' '#53685#49888' '#51060#49345' 2'
        #47560#49828#53552' '#49836#47112#51060#48652' '#50868#51204' '#51060#49345' 1'
        'USB '#53685#49888' '#53440#51076' '#50500#50883' '#51060#49345
        'USB '#53685#49888' '#51060#49345
        #50892#52824' '#46020#44536
        #49436#48372#50544#54532' '#44284#50676' '#44221#44256
        #48176#53552#47532' '#45800#49440' '#44221#44256
        'STO '#44221#44256
        #50896#51216' '#49464#53944' '#48120#49828' '#44221#44256
        #48176#53552#47532' '#44221#44256
        #44284#54924#49373' '#44221#44256
        #44284#48512#54616' '#44221#44256
        #49436#48372#47784#53552' '#44284#50676' '#44221#44256
        #51208#45824#50948#52824' '#52852#50868#53552' '#44221#44256
        #54028#46972#48120#53552' '#44221#44256
        #49436#48372' '#44053#51228' '#51221#51648' '#44221#44256
        #52968#53944#47204#47084' '#44596#44553#51221#51648' '#44221#44256
        #45257#44033' '#54060' '#54924#51204#49549#46020' '#51200#54616' '#44221#44256
        #51452#54924#47196' OFF '#44221#44256
        #53440#52629' '#51060#49345' '#44221#44256
        #44284#48512#54616' '#44221#44256' 2'
        #52636#47141' '#50752#53944' '#50724#48260' '#44221#44256
        #53552#54532' '#46300#46972#51060#48652' '#44221#44256
        #46300#46972#51060#48652' '#47112#53076#45908' '#50416#44592' '#48120#49828' '#44221#44256
        #48156#51652' '#44160#51648' '#44221#44256
        #49436#48372' '#50508#46988)
      ScrollBars = ssBoth
      TabOrder = 1
      Visible = False
    end
    object Memo_En2: TMemo
      Left = 110
      Top = 118
      Width = 200
      Height = 89
      Lines.Strings = (
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '19'
        '1A'
        '1E'
        '1F'
        '20'
        '21'
        '24'
        '25'
        '27'
        '28'
        '2A'
        '2B'
        '30'
        '31'
        '32'
        '33'
        '34'
        '35'
        '36'
        '37'
        '3A'
        '3D'
        '3E'
        '42'
        '45'
        '46'
        '47'
        '50'
        '51'
        '52'
        '54'
        '56'
        '63'
        '70'
        '71'
        '72'
        '82'
        '8A'
        '8E'
        '888'
        '91'
        '92'
        '95'
        '96'
        '9F'
        'E0'
        'E1'
        'E2'
        'E3'
        'E4'
        'E6'
        'E7'
        'E8'
        'E9'
        'EB'
        'EC'
        'ED'
        'F0'
        'F2'
        'F3'
        'Undervoltage'
        'Switch setting error'
        'Memory error 1 (RAM)'
        'Clock error'
        'Control processing error'
        'Memory error 2 (EEP-ROM)'
        'Encoder initial communication error 1'
        'Board error'
        'Memory error 3 (FLASH-ROM)'
        'Servo motor combination error'
        'Encoder initial communication error 2'
        'Encoder initial communication error 3'
        'Encoder normal communication error 1'
        'Encoder normal communication error 2'
        'Main circuit error'
        'Absolute position erased'
        'Initial magnetic pole detection error'
        'Linear encoder error 2'
        'Linear encoder error 1'
        'Encoder counter error'
        'Regenerative error'
        'Overspeed'
        'Overcurrent'
        'Overvoltage'
        'SSCNET receive error 1'
        'Command frequency alarm'
        'SSCNET receive error 2'
        'Parameter error'
        'Inrush current suppression circuit error'
        'Parameter setting error for driver communication'
        'Operation mode error'
        'Servo control error'
        'Main circuit device overheat'
        'Servo motor overheat'
        'Cooling fan alarm'
        'Overload 1'
        'Overload 2'
        'Error excessive'
        'Oscillation detection'
        'Forced stop error'
        'STO timing error'
        'Load-side encoder initial communication error 1'
        'Load-side encoder normal communication error 1'
        'Load-side encoder normal communication error 2'
        'Master-slave operation error 1'
        'USB communication timeout'
        'USB communication error'
        'Watchdog'
        'Servo amplifier overheat warning'
        'Open battery cable warning'
        'STO warning'
        'Home position setting error'
        'Battery warning'
        'Excessive regeneration warning'
        'Overload warning 1'
        'Servo motor overheat warning'
        'Absolute position counter warning'
        'Parameter warning'
        'Servo forced stop warning'
        'Controller forced stop warning'
        'Cooling fan speed reduction warning'
        'Main circuit off warning'
        'Other axes error warning'
        'Overload warning 2'
        'Output watt excess warning'
        'Tough drive warning'
        'Drive recorder '#8211' Miswriting warning'
        'Oscillation detection warning'
        'Servo alarm')
      ScrollBars = ssBoth
      TabOrder = 2
      Visible = False
    end
    object Memo_Hu2: TMemo
      Left = 110
      Top = 213
      Width = 200
      Height = 89
      Lines.Strings = (
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '19'
        '1A'
        '1E'
        '1F'
        '20'
        '21'
        '24'
        '25'
        '27'
        '28'
        '2A'
        '2B'
        '30'
        '31'
        '32'
        '33'
        '34'
        '35'
        '36'
        '37'
        '3A'
        '3D'
        '3E'
        '42'
        '45'
        '46'
        '47'
        '50'
        '51'
        '52'
        '54'
        '56'
        '63'
        '70'
        '71'
        '72'
        '82'
        '8A'
        '8E'
        '888'
        '91'
        '92'
        '95'
        '96'
        '9F'
        'E0'
        'E1'
        'E2'
        'E3'
        'E4'
        'E6'
        'E7'
        'E8'
        'E9'
        'EB'
        'EC'
        'ED'
        'F0'
        'F2'
        'F3'
        'Undervoltage'
        'Switch setting error'
        'Memory error 1 (RAM)'
        'Clock error'
        'Control processing error'
        'Memory error 2 (EEP-ROM)'
        'Encoder initial communication error 1'
        'Board error'
        'Memory error 3 (FLASH-ROM)'
        'Servo motor combination error'
        'Encoder initial communication error 2'
        'Encoder initial communication error 3'
        'Encoder normal communication error 1'
        'Encoder normal communication error 2'
        'Main circuit error'
        'Absolute position erased'
        'Initial magnetic pole detection error'
        'Linear encoder error 2'
        'Linear encoder error 1'
        'Encoder counter error'
        'Regenerative error'
        'Overspeed'
        'Overcurrent'
        'Overvoltage'
        'SSCNET receive error 1'
        'Command frequency alarm'
        'SSCNET receive error 2'
        'Parameter error'
        'Inrush current suppression circuit error'
        'Parameter setting error for driver communication'
        'Operation mode error'
        'Servo control error'
        'Main circuit device overheat'
        'Servo motor overheat'
        'Cooling fan alarm'
        'Overload 1'
        'Overload 2'
        'Error excessive'
        'Oscillation detection'
        'Forced stop error'
        'STO timing error'
        'Load-side encoder initial communication error 1'
        'Load-side encoder normal communication error 1'
        'Load-side encoder normal communication error 2'
        'Master-slave operation error 1'
        'USB communication timeout'
        'USB communication error'
        'Watchdog'
        'Servo amplifier overheat warning'
        'Open battery cable warning'
        'STO warning'
        'Home position setting error'
        'Battery warning'
        'Excessive regeneration warning'
        'Overload warning 1'
        'Servo motor overheat warning'
        'Absolute position counter warning'
        'Parameter warning'
        'Servo forced stop warning'
        'Controller forced stop warning'
        'Cooling fan speed reduction warning'
        'Main circuit off warning'
        'Other axes error warning'
        'Overload warning 2'
        'Output watt excess warning'
        'Tough drive warning'
        'Drive recorder '#8211' Miswriting warning'
        'Oscillation detection warning'
        'Servo alarm')
      ScrollBars = ssBoth
      TabOrder = 3
      Visible = False
    end
  end
  object GroupBox3: TGroupBox
    Left = 8
    Top = 359
    Width = 313
    Height = 345
    Caption = #50868#51204' '#50508#46988
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    object ListView3: TListView
      Left = 16
      Top = 31
      Width = 281
      Height = 300
      Columns = <
        item
          Width = 0
        end
        item
          Caption = 'No.'
        end
        item
          Caption = 'Name'
          Width = 227
        end>
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      GridLines = True
      HideSelection = False
      ReadOnly = True
      RowSelect = True
      ParentFont = False
      TabOrder = 0
      ViewStyle = vsReport
    end
    object Memo_Ko3: TMemo
      Left = 89
      Top = 18
      Width = 200
      Height = 89
      Lines.Strings = (
        '10'
        '12'
        '16'
        '13'
        '1A'
        '20'
        '21'
        '22'
        '23'
        '24'
        '25'
        '26'
        '2D'
        '2E'
        '2F'
        '37'
        '38'
        '39'
        '3B'
        '40'
        '41'
        '42'
        '43'
        '44'
        '45'
        '4D'
        '50'
        '51'
        '52'
        '53'
        '54'
        '55'
        '56'
        '57'
        '58'
        '5B'
        '5C'
        '5D'
        '5E'
        '90'
        '91'
        '92'
        '94'
        '95'
        '96'
        '97'
        '98'
        '9C'
        '9D'
        'A0'
        'A1'
        'A2'
        'A4'
        'A5'
        'A6'
        'A7'
        'B0'
        'B1'
        'B2'
        'B3'
        #51221#51648' '#51648#47161#51473
        #44596#44553#51221#51648#51473
        #44536#47353' '#50640#47084
        #51064#53552#47197' '#51473
        #53580#49828#53944' '#47784#46300#51473
        #50868#51204' '#47784#46300' '#48512#51221
        #51648#47161' '#49549#46020' 0'
        #54252#51064#53944' '#48264#54840' '#48512#51221
        #50868#51204#51473' '#47784#46300' '#48320#44221
        #50948#52824#44208#51221' '#50948#52824' '#52488#44284
        #54252#51064#53944' '#53580#51060#48660' '#49444#51221' '#48512#51221
        #51064#53944#47532#47704#53560' '#51060#49569' '#51060#46041#47049' '#48512#51221
        #52572#49888' '#51648#47161' '#48260#54140' '#48264#54840' '#49444#51221' '#50640#47084
        #51228#50612' '#47784#46300' '#51204#54872' '#48512#51221
        #53664#53356' '#51228#50612' '#49444#51221' '#48512#51221
        #54028#46972#48120#53552' '#51060#49345
        #49884#49828#53596' '#49444#51221' '#48512#51221
        #51077#52636#47141' '#48264#54840' '#54624#45817' '#49444#51221' '#48512#51221
        #47560#53356' '#44160#52636' '#49444#51221' '#48512#51221
        #51649#49440' '#48372#44036' '#44592#46041' '#51312#44148' '#50640#47084
        #51649#49440' '#48372#44036' '#54252#51064#53944' '#45936#51060#53552' '#50640#47084
        #51649#49440' '#48372#44036' '#48372#51312#52629' '#44592#46041' '#48520#44032' '#50640#47084
        #44036#49453' '#52404#53356#52629' '#49444#51221' '#50640#47084
        #44036#49453' '#50689#50669#45236' '#51648#47161' '#50640#47084
        #44036#49453' '#50689#50669' '#46020#45804' '#50640#47084
        #53440#52629' '#44592#46041' '#49444#51221' '#50640#47084
        #48337#47148' '#44396#46041' '#47784#46300' '#51204#54872' '#48512#51221
        #48337#47148' '#44396#46041' '#48708#46041#44592' '#47784#46300#51473
        #48337#47148' '#44396#46041#52629' '#49444#51221' '#50640#47084
        #48337#47148' '#44396#46041' '#54200#52264' '#44284#45824
        #48337#47148' '#44396#46041' '#46041#44592' '#51221#50676' '#50976#54952#54253' '#48512#51221
        #48337#47148' '#44396#46041' '#46041#44592' '#51221#50676' '#49892#54665#51473
        #48337#47148' '#44396#46041' '#49836#47112#51060#48652#52629' '#51060#49345
        #48337#47148' '#44396#46041' '#50724#52264' '#48372#51221' '#50976#54952#54253' '#50724#48260
        #48337#47148' '#44396#46041' '#46041#44592' '#51221#50676' '#48512#51221
        #53440#52629' '#44592#46041' '#45936#51060#53552' '#49324#50857#51473
        #53685#44284' '#50948#52824' '#51064#53552#47101#53944' '#48512#51221
        #51221#50517' '#51228#50612' '#48512#51221
        #51221#50517' '#51228#50612' '#49444#51221' '#48512#51221
        #50896#51216#48373#44480' '#48120#50756#47308
        'Z'#49345' '#48120#53685#44284
        #44540#51217#46020#44536#44032' '#51687#51020
        #50896#51216#48373#44480' '#48169#54693' '#49444#51221' '#48512#51221
        #53664#53356' '#51228#54620' '#46041#51089#51473#51060' '#50500#45784
        #50896#51216' '#49444#51221' '#50640#47084
        #50896#51216#48373#44480' '#51221#51648' '#50640#47084
        #50896#51216' '#49436#52824' '#47532#48120#53944' '#50640#47084
        'Z'#49345' '#47560#49828#53356#47049' '#49444#51221' '#48512#51221
        #50896#51216#48373#44480' '#54028#46972#48120#53552' '#49444#51221' '#48512#51221
        #47532#48120#53944' '#49828#50948#52824
        #49548#54532#53944#50920#50612' '#47532#48120#53944' '#48276#50948' '#50808
        #49548#54532#53944#50920#50612' '#47532#48120#53944' '#48276#50948' '#46020#45804
        #49548#54532#53944#50920#50612' '#47532#48120#53944' '#54028#46972#48120#53552' '#51060#49345
        #54252#51648#49496' '#49828#50948#52824' '#54028#46972#48120#53552' '#51060#49345
        #47560#53356' '#44160#52636' '#50416#44592'/'#51069#44592' '#48512#51221
        #51648#47161' '#45936#51060#53552' '#48512#51221
        #49436#48372' '#51228#50612' '#48520#44032
        #49436#48372' '#50508#46988#51473
        #49436#48372' OFF'#51473
        #49436#48372' OFF '#51648#47161
        #50868#51204' '#50508#46988)
      ScrollBars = ssBoth
      TabOrder = 1
      Visible = False
    end
    object Memo_En3: TMemo
      Left = 89
      Top = 113
      Width = 200
      Height = 89
      Lines.Strings = (
        '10'
        '12'
        '13'
        '16'
        '1A'
        '20'
        '21'
        '22'
        '23'
        '24'
        '25'
        '26'
        '2D'
        '2E'
        '2F'
        '37'
        '38'
        '39'
        '3B'
        '40'
        '41'
        '42'
        '43'
        '44'
        '45'
        '4D'
        '50'
        '51'
        '52'
        '53'
        '54'
        '55'
        '56'
        '57'
        '58'
        '5B'
        '5C'
        '5D'
        '5E'
        '5F'
        '90'
        '91'
        '92'
        '94'
        '95'
        '96'
        '97'
        '98'
        '9C'
        '9D'
        'A0'
        'A1'
        'A2'
        'A4'
        'A5'
        'A6'
        'A7'
        'B0'
        'B1'
        'B2'
        'B3'
        'Stop command on'
        'During forced stop'
        'Interlock is on'
        'Group error'
        'In test mode'
        'Operation mode error'
        'Command speed zero'
        'Point number error'
        'Mode change during operation'
        'Position exceeded during positioning'
        'Point table Setting error'
        'Incremental feed movement amount error'
        'Latest command buffer number setting error'
        'Control mode switch error'
        'Torque control setting error'
        'Parameter error'
        'System setting error'
        'I/O No. assignment setting error'
        'Mark detection setting error'
        'Linear interpolation start up error'
        'Linear interpolation point data error'
        'Can'#39't start linear interpolation auxiliary axis error'
        'Interference check axis setting error'
        'Command error in interference area'
        'Entering interference area error'
        'Other axes start setting error'
        'Tandem drive mode change error'
        'While in tandem drive nonsynchronous mode'
        'Tandem drive axis setting error'
        'Tandem drive excessive deviation'
        'Tandem drive synchronous alignment valid width error'
        'Tandem drive while performing synchronization'
        'Tandem drive slave axis error'
        
          'Exceeding of valid width of tandem drive deviation compensation ' +
          'error'
        'Tandem drive synchronous alignment error'
        'Using other axes start data'
        'Pass position interrupt error'
        'Continuous operation to torque control error'
        'Continuous operation to torque control setting error'
        'Point table loop error'
        'Home position return not complete'
        'Z-phase not passed'
        'The proximity dog is short'
        'Home position return direction error'
        'Not limiting torque'
        'Home position setting error'
        'Home position stop error'
        'Home position search limit error'
        'Z-phase mask amount setting error'
        'Home position return parameter setting error'
        'Limit switch'
        'Out of software limit boundaries'
        'Reached software limit'
        'Software limit Parameter error'
        'Position switch parameter error'
        'Mark detection write/read error'
        'Command data error'
        'Servo is not controllable'
        'Servo alarm occurrence'
        'Servo is off'
        'Servo off command'
        'Operation alarm')
      ScrollBars = ssBoth
      TabOrder = 2
      Visible = False
    end
    object Memo_Hu3: TMemo
      Left = 89
      Top = 208
      Width = 200
      Height = 89
      Lines.Strings = (
        '10'
        '12'
        '13'
        '16'
        '1A'
        '20'
        '21'
        '22'
        '23'
        '24'
        '25'
        '26'
        '2D'
        '2E'
        '2F'
        '37'
        '38'
        '39'
        '3B'
        '40'
        '41'
        '42'
        '43'
        '44'
        '45'
        '4D'
        '50'
        '51'
        '52'
        '53'
        '54'
        '55'
        '56'
        '57'
        '58'
        '5B'
        '5C'
        '5D'
        '5E'
        '5F'
        '90'
        '91'
        '92'
        '94'
        '95'
        '96'
        '97'
        '98'
        '9C'
        '9D'
        'A0'
        'A1'
        'A2'
        'A4'
        'A5'
        'A6'
        'A7'
        'B0'
        'B1'
        'B2'
        'B3'
        'Stop command on'
        'During forced stop'
        'Interlock is on'
        'Group error'
        'In test mode'
        'Operation mode error'
        'Command speed zero'
        'Point number error'
        'Mode change during operation'
        'Position exceeded during positioning'
        'Point table Setting error'
        'Incremental feed movement amount error'
        'Latest command buffer number setting error'
        'Control mode switch error'
        'Torque control setting error'
        'Parameter error'
        'System setting error'
        'I/O No. assignment setting error'
        'Mark detection setting error'
        'Linear interpolation start up error'
        'Linear interpolation point data error'
        'Can'#39't start linear interpolation auxiliary axis error'
        'Interference check axis setting error'
        'Command error in interference area'
        'Entering interference area error'
        'Other axes start setting error'
        'Tandem drive mode change error'
        'While in tandem drive nonsynchronous mode'
        'Tandem drive axis setting error'
        'Tandem drive excessive deviation'
        'Tandem drive synchronous alignment valid width error'
        'Tandem drive while performing synchronization'
        'Tandem drive slave axis error'
        
          'Exceeding of valid width of tandem drive deviation compensation ' +
          'error'
        'Tandem drive synchronous alignment error'
        'Using other axes start data'
        'Pass position interrupt error'
        'Continuous operation to torque control error'
        'Continuous operation to torque control setting error'
        'Point table loop error'
        'Home position return not complete'
        'Z-phase not passed'
        'The proximity dog is short'
        'Home position return direction error'
        'Not limiting torque'
        'Home position setting error'
        'Home position stop error'
        'Home position search limit error'
        'Z-phase mask amount setting error'
        'Home position return parameter setting error'
        'Limit switch'
        'Out of software limit boundaries'
        'Reached software limit'
        'Software limit Parameter error'
        'Position switch parameter error'
        'Mark detection write/read error'
        'Command data error'
        'Servo is not controllable'
        'Servo alarm occurrence'
        'Servo is off'
        'Servo off command'
        'Operation alarm')
      ScrollBars = ssBoth
      TabOrder = 3
      Visible = False
    end
  end
  object GroupBox4: TGroupBox
    Left = 327
    Top = 359
    Width = 313
    Height = 345
    Caption = #49884#49828#53596' '#50640#47084
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
    object ListView4: TListView
      Left = 16
      Top = 31
      Width = 281
      Height = 300
      Columns = <
        item
          Width = 0
        end
        item
          Caption = 'No.'
        end
        item
          Caption = 'Name'
          Width = 227
        end>
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      GridLines = True
      HideSelection = False
      ReadOnly = True
      RowSelect = True
      ParentFont = False
      TabOrder = 0
      ViewStyle = vsReport
    end
    object Memo_Ko4: TMemo
      Left = 89
      Top = 26
      Width = 200
      Height = 89
      Lines.Strings = (
        'E001'
        'E002'
        'E003'
        'E004'
        'E006'
        'E007'
        'E008'
        'E1'#9633#9633
        'E200'
        'E301'
        'E302'
        'E400'
        'E401'
        'E403'
        'E405'
        'E407'
        'E40B'
        'E500'
        'E5E0'
        'E510'
        'E503'
        'E5E1'
        'EF01'
        'ROM '#51060#49345
        'RAM '#51060#49345'1'
        '2'#54252#53944' '#47700#47784#47532' '#51060#49345
        'RAM '#51060#49345'2'
        'SSCNET '#53685#49888' IC '#51060#49345'1'
        'SSCNET '#53685#49888' IC '#51060#49345'2'
        #44592#54032' '#51060#49345
        'CPU '#51060#49345
        #48512#51221' '#51064#53552#47101#53944
        #50892#52824' '#46020#44536' '#50640#47084' ('#51452'2)'
        'DC FAIL'
        #48120#49892#51109#52265#52629' '#51080#51020
        'CRC '#51060#49345
        #45936#51060#53552' ID '#50640#47084
        #46300#46972#51060#48260' '#44592#51333' '#53076#46300' '#48512#51221
        'SSCNET '#53440#51076' '#50500#50883
        #46300#46972#51060#48260' '#51228#50612' '#48520#44032
        #51204#51088' '#44592#50612' '#49444#51221' '#48512#51221
        'SSCNET '#53685#49888' '#49884#49828#53596' '#50640#47084
        #51077#52636#47141' '#48264#54840' '#54624#45817' '#50640#47084
        #48176#53440' '#51228#50612' '#48512#51221
        'SSCNET '#53685#49888' '#49884#49828#53596' '#50640#47084'2'
        #49884#49828#53596' '#51648#47161' '#53076#46300' '#48512#51221
        #49884#49828#53596' '#50640#47084)
      ScrollBars = ssBoth
      TabOrder = 1
      Visible = False
    end
    object Memo_En4: TMemo
      Left = 89
      Top = 121
      Width = 200
      Height = 89
      Lines.Strings = (
        'E001'
        'E002'
        'E003'
        'E004'
        'E006'
        'E007'
        'E008'
        'E1'#9633#9633
        'E200'
        'E301'
        'E302'
        'E400'
        'E401'
        'E403'
        'E405'
        'E407'
        'E40B'
        'E40E'#9
        'E500'
        'E503'
        'E510'
        'E511'
        'E5E0'
        'E5E1'
        'EF01'
        'ROM error'
        'RAM error 1'
        'Dual port memory error'
        'RAM error 2'
        'SSCNET communication IC error 1'
        'SSCNET communication IC error 2'
        'Board error'
        'CPU error'
        'Interrupt error'
        'Watchdog error(Note 2)'
        'DC FAIL'
        'An axis that has not been mounted exists'
        'CRC error'
        'Data ID error'
        'Driver type code error'
        'SSCNET time out'
        'Uncontrollable driver'
        'Communication cycle error'
        'Electronic gear setting error'
        'Exclusive control error'
        'I/O No. assignment error'
        'I/O table select error'
        'SSCNET communication system error'
        'SSCNET communication system error 2'
        'System command code error'
        'System error')
      ScrollBars = ssBoth
      TabOrder = 2
      Visible = False
    end
    object Memo_Hu4: TMemo
      Left = 89
      Top = 216
      Width = 200
      Height = 89
      Lines.Strings = (
        'E001'
        'E002'
        'E003'
        'E004'
        'E006'
        'E007'
        'E008'
        'E1'#9633#9633
        'E200'
        'E301'
        'E302'
        'E400'
        'E401'
        'E403'
        'E405'
        'E407'
        'E40B'
        'E40E'#9
        'E500'
        'E503'
        'E510'
        'E511'
        'E5E0'
        'E5E1'
        'EF01'
        'ROM error'
        'RAM error 1'
        'Dual port memory error'
        'RAM error 2'
        'SSCNET communication IC error 1'
        'SSCNET communication IC error 2'
        'Board error'
        'CPU error'
        'Interrupt error'
        'Watchdog error(Note 2)'
        'DC FAIL'
        'An axis that has not been mounted exists'
        'CRC error'
        'Data ID error'
        'Driver type code error'
        'SSCNET time out'
        'Uncontrollable driver'
        'Communication cycle error'
        'Electronic gear setting error'
        'Exclusive control error'
        'I/O No. assignment error'
        'I/O table select error'
        'SSCNET communication system error'
        'SSCNET communication system error 2'
        'System command code error'
        'System error')
      ScrollBars = ssBoth
      TabOrder = 3
      Visible = False
    end
  end
end
