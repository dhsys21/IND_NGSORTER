object plc: Tplc
  OldCreateOrder = False
  OnCreate = DataModuleCreate
  Height = 150
  Width = 215
  object reConnectTimer: TTimer
    Enabled = False
    Interval = 5000
    OnTimer = reConnectTimerTimer
    Left = 152
    Top = 64
  end
  object scanTimer: TTimer
    Enabled = False
    Interval = 300
    OnTimer = scanTimerTimer
    Left = 32
    Top = 80
  end
  object client: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Host = '17.91.233.121'
    Port = 8197
    OnConnect = clientConnect
    OnDisconnect = clientDisconnect
    OnRead = clientRead
    OnError = clientError
    Left = 88
    Top = 56
  end
end
