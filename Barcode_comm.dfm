object Barcode: TBarcode
  OldCreateOrder = False
  Height = 211
  Width = 215
  object Comm: TComPort
    BaudRate = br9600
    Port = 'COM1'
    Parity.Bits = prNone
    StopBits = sbOneStopBit
    DataBits = dbEight
    Events = [evRxChar, evTxEmpty, evRxFlag, evRing, evBreak, evCTS, evDSR, evError, evRLSD, evRx80Full]
    FlowControl.OutCTSFlow = False
    FlowControl.OutDSRFlow = False
    FlowControl.ControlDTR = dtrDisable
    FlowControl.ControlRTS = rtsDisable
    FlowControl.XonXoffOut = False
    FlowControl.XonXoffIn = False
    StoredProps = [spBasic]
    TriggersOnRxChar = True
    OnRxFlag = CommRxFlag
    Left = 88
    Top = 88
  end
  object chkTimer: TTimer
    Enabled = False
    Interval = 30000
    OnTimer = chkTimerTimer
    Left = 40
    Top = 152
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 30000
    OnTimer = chkTimerTimer
    Left = 104
    Top = 152
  end
end
