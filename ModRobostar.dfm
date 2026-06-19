object robostar: Trobostar
  OldCreateOrder = False
  OnCreate = DataModuleCreate
  Height = 227
  Width = 331
  object senTimer: TTimer
    Enabled = False
    Interval = 100
    OnTimer = senTimerTimer
    Left = 88
    Top = 56
  end
  object Timer_zUpTest: TTimer
    Enabled = False
    Interval = 100
    Left = 200
    Top = 120
  end
end
