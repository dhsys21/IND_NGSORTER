object Mod_Fms: TMod_Fms
  OldCreateOrder = False
  Height = 498
  Width = 736
  object TcpServer: TIdTCPServer
    Bindings = <>
    DefaultPort = 18080
    OnConnect = TcpServerConnect
    OnDisconnect = TcpServerDisconnect
    OnExecute = TcpServerExecute
    Left = 48
    Top = 32
  end
  object Timer_Alive: TTimer
    Enabled = False
    Interval = 1000
    OnTimer = Timer_AliveTimer
    Left = 128
    Top = 32
  end
end
