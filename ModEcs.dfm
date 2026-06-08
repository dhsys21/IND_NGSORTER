object EcsMod: TEcsMod
  OldCreateOrder = False
  Height = 265
  Width = 335
  object ecsSock: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Host = 'localhost'
    Port = 5010
    OnConnect = ecsSockConnect
    OnDisconnect = ecsSockDisconnect
    OnRead = ecsSockRead
    OnError = ecsSockError
    Left = 104
    Top = 64
  end
  object ReContactTimer: TTimer
    Enabled = False
    Interval = 5000
    OnTimer = ReContactTimerTimer
    Left = 104
    Top = 120
  end
end
