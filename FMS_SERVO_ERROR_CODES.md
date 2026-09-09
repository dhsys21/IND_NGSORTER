# Servo Alarm FMS Codes

Generated from FormServoAlarmList.dfm English memo lists. Native codes are hexadecimal. Alarm subcodes and axis numbers remain in the existing diagnostic screen; identical main codes are reported once until all affected axes recover.

| Group | Native Code (Hex) | FMS ErrorNo (UInt32) | Existing Description |
| --- | --- | --- | --- |
| 10 | 35 | 10000053 | Operation cycle alarm |
| 10 | 36 | 10000054 | Number of write accesses to flash ROM error |
| 10 | 37 | 10000055 | Parameter error |
| 10 | 38 | 10000056 | Mark detection setting error |
| 20 | 10 | 20000016 | Undervoltage |
| 20 | 11 | 20000017 | Switch setting error |
| 20 | 12 | 20000018 | Memory error 1 (RAM) |
| 20 | 13 | 20000019 | Clock error |
| 20 | 14 | 20000020 | Control processing error |
| 20 | 15 | 20000021 | Memory error 2 (EEP-ROM) |
| 20 | 16 | 20000022 | Encoder initial communication error 1 |
| 20 | 17 | 20000023 | Board error |
| 20 | 19 | 20000025 | Memory error 3 (FLASH-ROM) |
| 20 | 1A | 20000026 | Servo motor combination error |
| 20 | 1E | 20000030 | Encoder initial communication error 2 |
| 20 | 1F | 20000031 | Encoder initial communication error 3 |
| 20 | 20 | 20000032 | Encoder normal communication error 1 |
| 20 | 21 | 20000033 | Encoder normal communication error 2 |
| 20 | 24 | 20000036 | Main circuit error |
| 20 | 25 | 20000037 | Absolute position erased |
| 20 | 27 | 20000039 | Initial magnetic pole detection error |
| 20 | 28 | 20000040 | Linear encoder error 2 |
| 20 | 2A | 20000042 | Linear encoder error 1 |
| 20 | 2B | 20000043 | Encoder counter error |
| 20 | 30 | 20000048 | Regenerative error |
| 20 | 31 | 20000049 | Overspeed |
| 20 | 32 | 20000050 | Overcurrent |
| 20 | 33 | 20000051 | Overvoltage |
| 20 | 34 | 20000052 | SSCNET receive error 1 |
| 20 | 35 | 20000053 | Command frequency alarm |
| 20 | 36 | 20000054 | SSCNET receive error 2 |
| 20 | 37 | 20000055 | Parameter error |
| 20 | 3A | 20000058 | Inrush current suppression circuit error |
| 20 | 3D | 20000061 | Parameter setting error for driver communication |
| 20 | 3E | 20000062 | Operation mode error |
| 20 | 42 | 20000066 | Servo control error |
| 20 | 45 | 20000069 | Main circuit device overheat |
| 20 | 46 | 20000070 | Servo motor overheat |
| 20 | 47 | 20000071 | Cooling fan alarm |
| 20 | 50 | 20000080 | Overload 1 |
| 20 | 51 | 20000081 | Overload 2 |
| 20 | 52 | 20000082 | Error excessive |
| 20 | 54 | 20000084 | Oscillation detection |
| 20 | 56 | 20000086 | Forced stop error |
| 20 | 63 | 20000099 | STO timing error |
| 20 | 70 | 20000112 | Load-side encoder initial communication error 1 |
| 20 | 71 | 20000113 | Load-side encoder normal communication error 1 |
| 20 | 72 | 20000114 | Load-side encoder normal communication error 2 |
| 20 | 82 | 20000130 | Master-slave operation error 1 |
| 20 | 8A | 20000138 | USB communication timeout |
| 20 | 8E | 20000142 | USB communication error |
| 20 | 888 | 20002184 | Watchdog |
| 20 | 91 | 20000145 | Servo amplifier overheat warning |
| 20 | 92 | 20000146 | Open battery cable warning |
| 20 | 95 | 20000149 | STO warning |
| 20 | 96 | 20000150 | Home position setting error |
| 20 | 9F | 20000159 | Battery warning |
| 20 | E0 | 20000224 | Excessive regeneration warning |
| 20 | E1 | 20000225 | Overload warning 1 |
| 20 | E2 | 20000226 | Servo motor overheat warning |
| 20 | E3 | 20000227 | Absolute position counter warning |
| 20 | E4 | 20000228 | Parameter warning |
| 20 | E6 | 20000230 | Servo forced stop warning |
| 20 | E7 | 20000231 | Controller forced stop warning |
| 20 | E8 | 20000232 | Cooling fan speed reduction warning |
| 20 | E9 | 20000233 | Main circuit off warning |
| 20 | EB | 20000235 | Other axes error warning |
| 20 | EC | 20000236 | Overload warning 2 |
| 20 | ED | 20000237 | Output watt excess warning |
| 20 | F0 | 20000240 | Tough drive warning |
| 20 | F2 | 20000242 | Drive recorder - Miswriting warning |
| 20 | F3 | 20000243 | Oscillation detection warning |
| 30 | 10 | 30000016 | Stop command on |
| 30 | 12 | 30000018 | During forced stop |
| 30 | 13 | 30000019 | Interlock is on |
| 30 | 16 | 30000022 | Group error |
| 30 | 1A | 30000026 | In test mode |
| 30 | 20 | 30000032 | Operation mode error |
| 30 | 21 | 30000033 | Command speed zero |
| 30 | 22 | 30000034 | Point number error |
| 30 | 23 | 30000035 | Mode change during operation |
| 30 | 24 | 30000036 | Position exceeded during positioning |
| 30 | 25 | 30000037 | Point table Setting error |
| 30 | 26 | 30000038 | Incremental feed movement amount error |
| 30 | 2D | 30000045 | Latest command buffer number setting error |
| 30 | 2E | 30000046 | Control mode switch error |
| 30 | 2F | 30000047 | Torque control setting error |
| 30 | 37 | 30000055 | Parameter error |
| 30 | 38 | 30000056 | System setting error |
| 30 | 39 | 30000057 | I/O No. assignment setting error |
| 30 | 3B | 30000059 | Mark detection setting error |
| 30 | 40 | 30000064 | Linear interpolation start up error |
| 30 | 41 | 30000065 | Linear interpolation point data error |
| 30 | 42 | 30000066 | Can't start linear interpolation auxiliary axis error |
| 30 | 43 | 30000067 | Interference check axis setting error |
| 30 | 44 | 30000068 | Command error in interference area |
| 30 | 45 | 30000069 | Entering interference area error |
| 30 | 4D | 30000077 | Other axes start setting error |
| 30 | 50 | 30000080 | Tandem drive mode change error |
| 30 | 51 | 30000081 | While in tandem drive nonsynchronous mode |
| 30 | 52 | 30000082 | Tandem drive axis setting error |
| 30 | 53 | 30000083 | Tandem drive excessive deviation |
| 30 | 54 | 30000084 | Tandem drive synchronous alignment valid width error |
| 30 | 55 | 30000085 | Tandem drive while performing synchronization |
| 30 | 56 | 30000086 | Tandem drive slave axis error |
| 30 | 57 | 30000087 | Exceeding of valid width of tandem drive deviation compensation error |
| 30 | 58 | 30000088 | Tandem drive synchronous alignment error |
| 30 | 5B | 30000091 | Using other axes start data |
| 30 | 5C | 30000092 | Pass position interrupt error |
| 30 | 5D | 30000093 | Continuous operation to torque control error |
| 30 | 5E | 30000094 | Continuous operation to torque control setting error |
| 30 | 5F | 30000095 | Point table loop error |
| 30 | 90 | 30000144 | Home position return not complete |
| 30 | 91 | 30000145 | Z-phase not passed |
| 30 | 92 | 30000146 | The proximity dog is short |
| 30 | 94 | 30000148 | Home position return direction error |
| 30 | 95 | 30000149 | Not limiting torque |
| 30 | 96 | 30000150 | Home position setting error |
| 30 | 97 | 30000151 | Home position stop error |
| 30 | 98 | 30000152 | Home position search limit error |
| 30 | 9C | 30000156 | Z-phase mask amount setting error |
| 30 | 9D | 30000157 | Home position return parameter setting error |
| 30 | A0 | 30000160 | Limit switch |
| 30 | A1 | 30000161 | Out of software limit boundaries |
| 30 | A2 | 30000162 | Reached software limit |
| 30 | A4 | 30000164 | Software limit Parameter error |
| 30 | A5 | 30000165 | Position switch parameter error |
| 30 | A6 | 30000166 | Mark detection write/read error |
| 30 | A7 | 30000167 | Command data error |
| 30 | B0 | 30000176 | Servo is not controllable |
| 30 | B1 | 30000177 | Servo alarm occurrence |
| 30 | B2 | 30000178 | Servo is off |
| 30 | B3 | 30000179 | Servo off command |
| 40 | E001 | 40000001 | ROM error |
| 40 | E002 | 40000002 | RAM error 1 |
| 40 | E003 | 40000003 | Dual port memory error |
| 40 | E004 | 40000004 | RAM error 2 |
| 40 | E006 | 40000006 | SSCNET communication IC error 1 |
| 40 | E007 | 40000007 | SSCNET communication IC error 2 |
| 40 | E008 | 40000008 | Board error |
| 40 | E1?? | 40000256..40000511 | CPU error |
| 40 | E200 | 40000512 | Interrupt error |
| 40 | E301 | 40000769 | Watchdog error(Note 2) |
| 40 | E302 | 40000770 | DC FAIL |
| 40 | E400 | 40001024 | An axis that has not been mounted exists |
| 40 | E401 | 40001025 | CRC error |
| 40 | E403 | 40001027 | Data ID error |
| 40 | E405 | 40001029 | Driver type code error |
| 40 | E407 | 40001031 | SSCNET time out |
| 40 | E40B | 40001035 | Uncontrollable driver |
| 40 | E40E | 40001038 | Communication cycle error |
| 40 | E500 | 40001280 | Electronic gear setting error |
| 40 | E503 | 40001283 | Exclusive control error |
| 40 | E510 | 40001296 | I/O No. assignment error |
| 40 | E511 | 40001297 | I/O table select error |
| 40 | E5E0 | 40001504 | SSCNET communication system error |
| 40 | E5E1 | 40001505 | SSCNET communication system error 2 |
| 40 | EF01 | 40003841 | System command code error |
