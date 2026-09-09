# Servo Alarm FMS Codes

Generated from FormServoAlarmList.dfm English memo lists. Native codes are hexadecimal. Alarm subcodes and axis numbers remain in the existing diagnostic screen; identical main codes are reported once until all affected axes recover.

| Group | Native Code (Hex) | FMS ErrorNo (UInt32) | Existing Description |
| --- | --- | --- | --- |
| 10 | 35 | 10053 | Operation cycle alarm |
| 10 | 36 | 10054 | Number of write accesses to flash ROM error |
| 10 | 37 | 10055 | Parameter error |
| 10 | 38 | 10056 | Mark detection setting error |
| 20 | 10 | 20016 | Undervoltage |
| 20 | 11 | 20017 | Switch setting error |
| 20 | 12 | 20018 | Memory error 1 (RAM) |
| 20 | 13 | 20019 | Clock error |
| 20 | 14 | 20020 | Control processing error |
| 20 | 15 | 20021 | Memory error 2 (EEP-ROM) |
| 20 | 16 | 20022 | Encoder initial communication error 1 |
| 20 | 17 | 20023 | Board error |
| 20 | 19 | 20025 | Memory error 3 (FLASH-ROM) |
| 20 | 1A | 20026 | Servo motor combination error |
| 20 | 1E | 20030 | Encoder initial communication error 2 |
| 20 | 1F | 20031 | Encoder initial communication error 3 |
| 20 | 20 | 20032 | Encoder normal communication error 1 |
| 20 | 21 | 20033 | Encoder normal communication error 2 |
| 20 | 24 | 20036 | Main circuit error |
| 20 | 25 | 20037 | Absolute position erased |
| 20 | 27 | 20039 | Initial magnetic pole detection error |
| 20 | 28 | 20040 | Linear encoder error 2 |
| 20 | 2A | 20042 | Linear encoder error 1 |
| 20 | 2B | 20043 | Encoder counter error |
| 20 | 30 | 20048 | Regenerative error |
| 20 | 31 | 20049 | Overspeed |
| 20 | 32 | 20050 | Overcurrent |
| 20 | 33 | 20051 | Overvoltage |
| 20 | 34 | 20052 | SSCNET receive error 1 |
| 20 | 35 | 20053 | Command frequency alarm |
| 20 | 36 | 20054 | SSCNET receive error 2 |
| 20 | 37 | 20055 | Parameter error |
| 20 | 3A | 20058 | Inrush current suppression circuit error |
| 20 | 3D | 20061 | Parameter setting error for driver communication |
| 20 | 3E | 20062 | Operation mode error |
| 20 | 42 | 20066 | Servo control error |
| 20 | 45 | 20069 | Main circuit device overheat |
| 20 | 46 | 20070 | Servo motor overheat |
| 20 | 47 | 20071 | Cooling fan alarm |
| 20 | 50 | 20080 | Overload 1 |
| 20 | 51 | 20081 | Overload 2 |
| 20 | 52 | 20082 | Error excessive |
| 20 | 54 | 20084 | Oscillation detection |
| 20 | 56 | 20086 | Forced stop error |
| 20 | 63 | 20099 | STO timing error |
| 20 | 70 | 20112 | Load-side encoder initial communication error 1 |
| 20 | 71 | 20113 | Load-side encoder normal communication error 1 |
| 20 | 72 | 20114 | Load-side encoder normal communication error 2 |
| 20 | 82 | 20130 | Master-slave operation error 1 |
| 20 | 8A | 20138 | USB communication timeout |
| 20 | 8E | 20142 | USB communication error |
| 20 | 888 | 202184 | Watchdog |
| 20 | 91 | 20145 | Servo amplifier overheat warning |
| 20 | 92 | 20146 | Open battery cable warning |
| 20 | 95 | 20149 | STO warning |
| 20 | 96 | 20150 | Home position setting error |
| 20 | 9F | 20159 | Battery warning |
| 20 | E0 | 20224 | Excessive regeneration warning |
| 20 | E1 | 20225 | Overload warning 1 |
| 20 | E2 | 20226 | Servo motor overheat warning |
| 20 | E3 | 20227 | Absolute position counter warning |
| 20 | E4 | 20228 | Parameter warning |
| 20 | E6 | 20230 | Servo forced stop warning |
| 20 | E7 | 20231 | Controller forced stop warning |
| 20 | E8 | 20232 | Cooling fan speed reduction warning |
| 20 | E9 | 20233 | Main circuit off warning |
| 20 | EB | 20235 | Other axes error warning |
| 20 | EC | 20236 | Overload warning 2 |
| 20 | ED | 20237 | Output watt excess warning |
| 20 | F0 | 20240 | Tough drive warning |
| 20 | F2 | 20242 | Drive recorder - Miswriting warning |
| 20 | F3 | 20243 | Oscillation detection warning |
| 30 | 10 | 30016 | Stop command on |
| 30 | 12 | 30018 | During forced stop |
| 30 | 13 | 30019 | Interlock is on |
| 30 | 16 | 30022 | Group error |
| 30 | 1A | 30026 | In test mode |
| 30 | 20 | 30032 | Operation mode error |
| 30 | 21 | 30033 | Command speed zero |
| 30 | 22 | 30034 | Point number error |
| 30 | 23 | 30035 | Mode change during operation |
| 30 | 24 | 30036 | Position exceeded during positioning |
| 30 | 25 | 30037 | Point table Setting error |
| 30 | 26 | 30038 | Incremental feed movement amount error |
| 30 | 2D | 30045 | Latest command buffer number setting error |
| 30 | 2E | 30046 | Control mode switch error |
| 30 | 2F | 30047 | Torque control setting error |
| 30 | 37 | 30055 | Parameter error |
| 30 | 38 | 30056 | System setting error |
| 30 | 39 | 30057 | I/O No. assignment setting error |
| 30 | 3B | 30059 | Mark detection setting error |
| 30 | 40 | 30064 | Linear interpolation start up error |
| 30 | 41 | 30065 | Linear interpolation point data error |
| 30 | 42 | 30066 | Can't start linear interpolation auxiliary axis error |
| 30 | 43 | 30067 | Interference check axis setting error |
| 30 | 44 | 30068 | Command error in interference area |
| 30 | 45 | 30069 | Entering interference area error |
| 30 | 4D | 30077 | Other axes start setting error |
| 30 | 50 | 30080 | Tandem drive mode change error |
| 30 | 51 | 30081 | While in tandem drive nonsynchronous mode |
| 30 | 52 | 30082 | Tandem drive axis setting error |
| 30 | 53 | 30083 | Tandem drive excessive deviation |
| 30 | 54 | 30084 | Tandem drive synchronous alignment valid width error |
| 30 | 55 | 30085 | Tandem drive while performing synchronization |
| 30 | 56 | 30086 | Tandem drive slave axis error |
| 30 | 57 | 30087 | Exceeding of valid width of tandem drive deviation compensation error |
| 30 | 58 | 30088 | Tandem drive synchronous alignment error |
| 30 | 5B | 30091 | Using other axes start data |
| 30 | 5C | 30092 | Pass position interrupt error |
| 30 | 5D | 30093 | Continuous operation to torque control error |
| 30 | 5E | 30094 | Continuous operation to torque control setting error |
| 30 | 5F | 30095 | Point table loop error |
| 30 | 90 | 30144 | Home position return not complete |
| 30 | 91 | 30145 | Z-phase not passed |
| 30 | 92 | 30146 | The proximity dog is short |
| 30 | 94 | 30148 | Home position return direction error |
| 30 | 95 | 30149 | Not limiting torque |
| 30 | 96 | 30150 | Home position setting error |
| 30 | 97 | 30151 | Home position stop error |
| 30 | 98 | 30152 | Home position search limit error |
| 30 | 9C | 30156 | Z-phase mask amount setting error |
| 30 | 9D | 30157 | Home position return parameter setting error |
| 30 | A0 | 30160 | Limit switch |
| 30 | A1 | 30161 | Out of software limit boundaries |
| 30 | A2 | 30162 | Reached software limit |
| 30 | A4 | 30164 | Software limit Parameter error |
| 30 | A5 | 30165 | Position switch parameter error |
| 30 | A6 | 30166 | Mark detection write/read error |
| 30 | A7 | 30167 | Command data error |
| 30 | B0 | 30176 | Servo is not controllable |
| 30 | B1 | 30177 | Servo alarm occurrence |
| 30 | B2 | 30178 | Servo is off |
| 30 | B3 | 30179 | Servo off command |
| 40 | E001 | 40001 | ROM error |
| 40 | E002 | 40002 | RAM error 1 |
| 40 | E003 | 40003 | Dual port memory error |
| 40 | E004 | 40004 | RAM error 2 |
| 40 | E006 | 40006 | SSCNET communication IC error 1 |
| 40 | E007 | 40007 | SSCNET communication IC error 2 |
| 40 | E008 | 40008 | Board error |
| 40 | E1?? | 40256..40511 | CPU error |
| 40 | E200 | 40512 | Interrupt error |
| 40 | E301 | 40769 | Watchdog error(Note 2) |
| 40 | E302 | 40770 | DC FAIL |
| 40 | E400 | 401024 | An axis that has not been mounted exists |
| 40 | E401 | 401025 | CRC error |
| 40 | E403 | 401027 | Data ID error |
| 40 | E405 | 401029 | Driver type code error |
| 40 | E407 | 401031 | SSCNET time out |
| 40 | E40B | 401035 | Uncontrollable driver |
| 40 | E40E | 401038 | Communication cycle error |
| 40 | E500 | 401280 | Electronic gear setting error |
| 40 | E503 | 401283 | Exclusive control error |
| 40 | E510 | 401296 | I/O No. assignment error |
| 40 | E511 | 401297 | I/O table select error |
| 40 | E5E0 | 401504 | SSCNET communication system error |
| 40 | E5E1 | 401505 | SSCNET communication system error 2 |
| 40 | EF01 | 403841 | System command code error |
