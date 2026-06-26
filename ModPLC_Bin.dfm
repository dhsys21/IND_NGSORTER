object PlcBin: TPlcBin
  OldCreateOrder = False
  Height = 323
  Width = 808
  object Timer_PLC_WriteMsg: TTimer
    Enabled = False
    Interval = 200
    OnTimer = Timer_PLC_WriteMsgTimer
    Left = 376
    Top = 120
  end
  object ClientSocket_PLC: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Port = 0
    OnConnect = ClientSocket_PLCConnect
    OnDisconnect = ClientSocket_PLCDisconnect
    OnRead = ClientSocket_PLCRead
    OnError = ClientSocket_PLCError
    Left = 248
    Top = 120
  end
  object ClientSocket_PC: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Port = 0
    OnConnect = ClientSocket_PCConnect
    OnDisconnect = ClientSocket_PCDisconnect
    OnRead = ClientSocket_PCRead
    OnError = ClientSocket_PCError
    Left = 256
    Top = 40
  end
  object Timer_PLC_AutoConnect: TTimer
    Enabled = False
    Interval = 300
    OnTimer = Timer_PLC_AutoConnectTimer
    Left = 104
    Top = 120
  end
  object Timer_PC_AutoConnect: TTimer
    Enabled = False
    Interval = 300
    OnTimer = Timer_PC_AutoConnectTimer
    Left = 112
    Top = 48
  end
  object Timer_PC_WriteMsg: TTimer
    Enabled = False
    Interval = 200
    OnTimer = Timer_PC_WriteMsgTimer
    Left = 376
    Top = 32
  end
end
