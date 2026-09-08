# 2026-09-08 병합 기록

## 보존한 기준

- 사용자 검증 완료본: `Ver. 2026-09-08 001`, `033709c5f73d491c26f2f9177350bae180dd5d02`.
- 보존 브랜치: `codex/verified-fms-20260908`.
- 사용자가 이번 병합 직전까지의 수정은 실설비 테스트 완료·정상 동작이라고 확인했다. 세부 시험 조합을 모두 검증했다고 확대 해석하지 않는다.
- 변경된 파일 및 비추적 파일 26개를 `C:\Users\IBC_NG~1\AppData\Local\Temp\NGSORTER_merge_backup_20260908_095658`에 복사하고 각각 SHA256 일치를 확인했다.
- 기존 생성 파일도 stash `3427c776e7a1f8c4b4a1251560cf102c7803a165`에 보존했다. 기존 GitHub Desktop stash는 삭제하지 않았다.

## 병합 내용

- 공통 기준: `b58e864`.
- 노트북 커밋: `ca3914cce9a7d551d162ed8184edef8dbd62c743` (`2026 09 08 001`).
- 노트북 수정: 자동 TrayLoad 시작/재시도/응답/완료 시 실제 트레이 신호를 재검증한다. 트레이가 사라지거나 PLC 상태가 유효하지 않으면 이전 요청·결과·Retry·대기 바코드를 폐기한다. Init Work는 이전 TrayLoad/ProcessStart를 초기화하되 완료 보고 중에는 차단한다.
- 로컬 수정: 명시적 수동 Scan은 독립된 데이터 전용 TrayLoad를 수행한다. FMS Trouble은 자동·수동 모두 별도 감시하며 Pause와 알람을 유지하고 명시적 Retry/Restart를 요구한다.
- 텍스트 충돌 없이 병합됐다. 의미상 겹치는 공용 알람창 처리는 보완했다: 자동 TrayLoad 또는 ProcessStart를 폐기해도 독립 FMS Trouble 알람창을 숨기지 않는다.
- 수동 요청에는 자동 트레이 존재 검사를 섞지 않는다. 대신 실제 자동/서보 작업 중에는 수동 데이터 덮어쓰기를 차단한다. 수동 완료는 자동 ProcessStart, 센터링, 트레이 배출, 서보 이동으로 이어지지 않는다.
- 병합 표시 버전: `Ver. 2026-09-08 002`.

## 검증

- `tmp/MergeReview20260908/test_merge.ps1`: 자동/수동/독립 알람 소유권 분리와 프로젝트·DFM·USEFORM 등록 일치.
- `tmp/TrayLoadPresence/test_presence.ps1`: 트레이 제거 후 기존 요청 폐기, 재삽입 시 이전 요청 부활 방지, PLC 상태 검사, 독립 FMS Trouble 알람 보존.
- `tmp/ManualFmsService/test_service.ps1`: 양쪽 수동 Scan/요청/데이터 표시/응답 초기화, 타임아웃·Retry, 독립 Trouble의 모든 모드 Pause 및 명시적 해제.
- `tmp/TrayWaitFix/test_tray_restart.ps1`: Pause 중 트레이 완료/바코드 보존과 다음 단계 순서.
- `tmp/FmsCurrentResponseFix/test_current_response.ps1`: 현재 응답값 판정 및 최종 초기화 대기/재시도.
- `tmp/FmsAlarmPauseClose/test_alarm_ui.ps1`: 자동 FMS 트랜잭션의 Pause/Close/Retry 및 반복 알람 억제.
- `tmp/ProcessOrderFix/test_process_order.ps1`: 16단계 순서 및 작업 시작의 실제 4개 신호 검사.
- 위 회귀/구조 검사는 모두 통과했다. 하네스는 실제 함수 본문과 모의 PLC/FMS/UI 경계를 사용한다.
- Win32 Debug 전체 빌드: 오류 0, 기존 경고 11.
- Win32 Release 전체 빌드: 오류 0, 기존 경고 38.
- 두 빌드는 각각 새 OBJ/출력 폴더를 사용했으며 설비 프로그램을 실행하지 않았다.

병합본 002 자체의 실설비 검증은 아직 아니다. 특히 자동 TrayLoad 오류 후 트레이 제거 → Init Work/Restart에서 요청이 재발행되지 않는지, 명시적 수동 Scan은 정상 처리되는지, 이때 독립 FMS Trouble 알람이 유지되는지 재확인해야 한다. 원격 push는 이 작업 범위에서 수행하지 않는다.
