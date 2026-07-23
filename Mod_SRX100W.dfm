object Mod_Bcr: TMod_Bcr
  OldCreateOrder = False
  Height = 350
  Width = 509
  object ClientSocketBcr: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Port = 0
    OnConnect = ClientSocketBcrConnect
    OnDisconnect = ClientSocketBcrDisconnect
    OnRead = ClientSocketBcrRead
    OnError = ClientSocketBcrError
    Left = 192
    Top = 96
  end
  object Timer1: TTimer
    Left = 280
    Top = 184
  end
  object Timer_AutoConnect: TTimer
    Enabled = False
    OnTimer = Timer_AutoConnectTimer
    Left = 120
    Top = 240
  end
end
