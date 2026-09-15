# 비상정지 복구 흐름 검토

## 007 보완 결과

아래는 006 검토 당시 발견 기록이며, 2026-09-11 007에서 3개 항목을 보완했다.
HOME 오류/timeout은 HOME 재요청 가능한 Pause로 전환하며 새 요청 시 대기 시간을 갱신한다.
복구 대기 중에는 메인 트레이 스텝을 보존한다. FMS 알람이 겹치면 첫 START/Restart/팝업 Retry는 알람 확인만 수행한다.
리뷰 테스트를 수정 후 정상 동작을 요구하는 회귀 테스트로 전환했고, 실패 재현 조건에서 정상 복구를 확인했다. 실장비 검증은 별도 필요하다.

## 006 검토 기록

검토 대상: codex-improve, 화면 버전 2026-09-11 006의 미커밋 변경본.
이번 검토에서는 제어 소스/설정/표시 버전을 수정하지 않았다.

## 발견 사항

### 1. [P1] HOME 실패/시간 초과 후 seqHome에서 빠져나오지 못함

- `ModRobostar.cpp:192`: req_Pause(true)는 기존 pauseStatus가 true이면 시퀀스를 변경하지 않는다.
- EMG에서 pauseStatus=true가 되고, req_Home는 이를 유지한 채 seqHome를 실행한다.
- `ModRobostar.cpp:865`: 새 HOME 오류 검사는 MotionFault를 호출한다. MotionFault는 축 정지는 호출하지만, 위 조건 때문에 seqHome를 seqPause/Idle로 바꾸지 못한다.
- 명령 실패가 지속되면 HOME step 0에서 같은 HOME 요청을 반복한다.
- `ModRobostar.cpp:3105`: 120초 watchdog이 발생하면 같은 seqHome/step에서 매 타이머마다 timeout 분기로 return한다. 상태 입력이 나중에 정상으로 돌아와도 Home()까지 도달하지 못한다.
- `FormTeaching.cpp:298`: HOME 버튼은 Idle/Pause만 허용한다. 새 EMG Restart는 HOME 완료를 요구하므로 이 경로로도 풀리지 않는다.

재현: 실제 Home/req_Home/req_Pause/MotionFault/StopAxes와 watchdog 코드를 추출해 실행했다. HOME 요청 오류를 5회 주면 seqHome/step0 유지 및 요청 5회 재전송을 확인했다. HOME 상태 대기를 120초 초과시킨 후 입력을 정상화해도 seqHome/step3에 남았다. 축 정지 호출 자체는 수행되었다.

수정 방향: 생산 Pause와 복구 HOME 실행 상태를 분리하거나, 복구 HOME 오류에서 명시적으로 현재 HOME를 취소하고 재요청 가능한 정지 상태로 전환한다. emgHomeDone=false를 유지하고, HOME 재요청 때 watchdog을 새로 시작한다. 이전 생산 목표를 복원해서는 안 된다.

### 2. [P1] 대상 센터링 일시 OFF가 메인 트레이 스텝을 초기화함

- `FormMain.cpp:2970`: 메인 stepTimer는 AUTO이면 Gripper/Robot Pause 검사보다 먼저 D10106 OFF를 보고 step[1]을 0으로 만든다.
- 로봇 센서 타이머가 먼저 EMG를 읽어 Pause한 뒤 메인 화면이 수동모드로 전환하기 전까지 이 실행 순서는 가능하다. 이때 대상 센터링 신호가 일시 OFF인 경우가 대상이다.
- 트레이가 실제로 빠지지 않고 센터링만 복구되면 EMG 기록의 Tray ID/셀 내용과 TrayLoaded는 일치하므로 AUTO/Restart 검사를 통과한다.
- ResumeEmergencyCheckpoint는 Robot/Gripper만 복구하고 메인의 step[1]은 복구하지 않는다.
- `FormMain.cpp:3057`: 재개 후 메인 타이머가 step[1]==0을 처리하면서 기존 Target Tray ID를 지우고 바코드를 재요청한다.
- 같은 바코드가 빨리 수신되면 기존 중복 TrayLoad 방어가 작동할 수 있다. 그러나 재독 실패/지연으로 Tray ID가 비어 있으면 저장 또는 CellTrackOut 보고가 막힐 수 있다(`Stage_mes.cpp:947`). 따라서 항상 TrayLoad가 다시 전송된다고 단정할 수는 없지만, 불필요한 재독과 ID 소실은 재현되었다.

재현: AUTO/Pause 상태에서 D10106 OFF로 실제 메인 stepTimer를 한 번 호출한 뒤, D10106 ON 및 HOME/Restart를 실행했다. 다음 메인 타이머에서 Target ID가 빈 문자열로 바뀌고 ReadTargetTrayBarcode가 1회 호출되었다. 정상적으로 step[1]==1을 유지한 비교 사례에서는 재독하지 않았다.

수정 방향: 복구 대기 중 메인 트레이 사이클 스텝도 보존/검증한다. 실제 Tray In OFF는 복구 무효화하되, 동일 트레이의 센터링 일시 해제를 신규 트레이 입고로 취급하지 않는다. 재개 시 이미 승인된 Target 상태를 명시적으로 이어받는다.

### 3. [P2] FMS Trouble 확인 버튼이 EMG 복구 분기에 막힘

- `FormMain.cpp:961`, `FormMain.cpp:2576`: Restart/START의 EMG 분기가 AcknowledgeFmsTrouble보다 먼저 실행되고 return한다.
- `FormMain.cpp:2420`: EMG 준비 검사는 IsFmsTroubleBlocking이면 실패한다.
- `Stage_mes.cpp:135`: FMS가 Trouble OFF를 보내도 미확인 알람 래치는 작업자 확인까지 유지한다.
- `FormAlarm_fms.cpp:67`: 팝업 Retry 역시 Main Restart를 호출하므로 같은 분기로 들어간다.
- 결과적으로 EMG 복구 옵션이 켜진 상태에서는 AUTO/START/Restart/팝업 Retry로 이 알람을 확인하는 정상 경로가 막힌다. 옵션을 꺼서 기존 경로로 나가는 것은 별도 우회이며, 새 복구 절차 자체의 해결이 아니다.

재현: HOME 완료와 나머지 준비 조건을 만족시킨 뒤 미확인 FMS Trouble을 주면 EMG 준비 및 START가 반복 실패했다. 실제 버튼 코드에서 확인 함수보다 EMG 분기가 앞서는 것과 팝업 Retry의 위임도 테스트로 검사했다.

수정 방향: FMS 알람을 작업자가 확인하는 동작과 물리적 복구/운전 시작을 분리한다. 명시적 확인 후에도 EMG/HOME/센서/트레이 인터락은 모두 다시 검사한다. 안전 인터락을 해제하는 변경으로 해결해서는 안 된다.

## 정상 경로에서 확인한 점

- HOME 정상 완료 후 erSource는 seqSorting/step0, erTarget은 seqInserting/step0으로 전환된다.
- Robot req_Stop는 이전 acceptedMove, seq_save, zDownProfileStage와 pauseStatus를 초기화한다. Gripper Pause도 복구 완료 시 해제된다.
- erReport는 삽입을 반복하지 않고 step4 보고 대기로 들어간다.
- FMS 완료 신호가 없을 때 step4 대기는 정상이다. 완료 신호를 공급하면 step7 저장을 거쳐 다음 사이클 호출로 빠져나온다.
- EMG 전에 이미 미완료 CellTrackOut/FAT 보고가 있는 경우는 현재 구현에서 의도적으로 자동 복구 제외다. 이 제한은 위 3개 결함과 별도로 현장 절차를 정해야 한다.

## 테스트 범위

`Tests/EmergencyRecoveryFlow_review.ps1`은 현재 소스의 함수 본문을 추출해 C++Builder bcc32로 별도 콘솔 프로그램을 빌드/실행한다. 정상 전환 3개, 결함 재현 4개(HOME 명령 오류/timeout은 같은 원인)를 확인했다. 이전 EmergencyWorkRecovery 통합 검사도 통과했다.

실제 구동 함수가 아닌 SSC 보드, PLC 값, FMS 전송/응답, 파일 저장, 화면 및 이미 유효한 복구 기록 검사는 테스트 대역이다. FMS 전체 응답 처리나 실제 XYZ 이동을 완료했다는 의미가 아니다. 실장비를 연결하거나 운전 프로그램을 실행하지 않았다. 전체 앱은 이번에 다시 빌드하지 않았으며 테스트 실행 파일만 빌드했다.

이 테스트는 현재 결함을 재현하는 리뷰용이다. 결함 수정 후에는 REPRODUCED 기대값을 정상 동작 기대값으로 바꿔 회귀 테스트로 전환해야 한다.
