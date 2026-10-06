#ifndef ManualCellRecoveryH
#define ManualCellRecoveryH
// 수동삽입 완료 1건의 진행 단계. Response=0 확인까지 동일한 셀/채널로 처리한다.
// 이 순서의 숫자를 복구파일 Phase에 저장하므로 순서/숫자를 임의로 바꾸지 않는다.
// 정상 흐름: Idle -> Prepared -> ClearForSend -> WaitResult -> Accepted -> Ready.
// 자동작업 복구: Ready -> Moving -> Idle. 직접 보고만 하는 경우: Ready -> 닫기 -> Idle.
enum TManualCellPhase {
    mcIdle,         // 진행 중 보고 없음. 입력/작업자 확인 대기.
    mcPrepared,     // 작업자 삽입 완료 확인을 저장함. 로컬 트레이 기록 반영 전/재시도 단계.
    mcClearForSend, // Request OFF 전송 후 Response=0 대기. 이전 응답을 지우고 새 요청 준비.
    mcWaitResult,   // Request ON 요청 단계. 전송 완료 및 Response=1(성공)/2(실패) 대기.
    mcAccepted,     // 성공 응답을 저장함. Request OFF 후 Response=0 대기. 성공 건 재보고 금지.
    mcReady,        // FMS 초기화까지 완료. 자동작업은 재개 버튼 대기, 직접 보고는 닫기 대기.
    mcMoving        // 재개 버튼으로 대기위치 이동 중. 정지/위치 확인 후 다음 셀 작업을 준비.
};
// 통신 응답의 판정 결과. mrReset은 장비 초기화가 아니라 FMS Response=0 확인이다.
enum TManualReply { mrWait, mrSuccess, mrFailure, mrReset, mrInvalid };
// 같은 Response=0이라도 결과 대기에서는 '아직 응답 없음', 초기화 대기에서는 '초기화 완료'.
inline TManualReply ManualCellReply(TManualCellPhase phase, int response)
{
    if(phase == mcWaitResult){
        if(response == 1) return mrSuccess;
        if(response == 2) return mrFailure;
        return response == 0 ? mrWait : mrInvalid;
    }
    if(phase == mcAccepted || phase == mcClearForSend)
        return response == 0 ? mrReset : mrWait;
    return mrWait;
}
#endif
