object gripper: Tgripper
  OldCreateOrder = False
  Height = 150
  Width = 215
  object stepTimer: TTimer
    Interval = 50
    OnTimer = stepTimerTimer
    Left = 88
    Top = 56
  end
  object waitTimer: TTimer
    Enabled = False
    Interval = 120000
    OnTimer = waitTimerTimer
    Left = 144
    Top = 88
  end
end
