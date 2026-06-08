object mes: Tmes
  OldCreateOrder = False
  OnCreate = DataModuleCreate
  OnDestroy = DataModuleDestroy
  Height = 150
  Width = 215
  object ServerSocket: TServerSocket
    Active = False
    Port = 7000
    ServerType = stNonBlocking
    OnClientConnect = ServerSocketClientConnect
    OnClientDisconnect = ServerSocketClientDisconnect
    OnClientRead = ServerSocketClientRead
    OnClientError = ServerSocketClientError
    Left = 56
    Top = 32
  end
  object rxTimer: TTimer
    Interval = 100
    OnTimer = rxTimerTimer
    Left = 136
    Top = 80
  end
end
