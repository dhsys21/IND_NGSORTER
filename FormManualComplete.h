#ifndef FormManualCompleteH
#define FormManualCompleteH
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "ManualCellRecovery.h"
class TManualCompleteForm : public TForm
{
__published:
    TPanel *pnlTitle;
    TLabel *lblSource;
    TLabel *lblTarget;
    TLabel *lblSourceChannel;
    TLabel *lblChannel;
    TLabel *lblStatus;
    TLabel *lblFmsState;
    TEdit *editSource;
    TEdit *editTarget;
    TEdit *editChannel;
    TEdit *editSourceChannel;
    TCheckBox *chkInserted;
    TButton *btnReport;
    TButton *btnSourceReturn;
    TButton *btnRetry;
    TButton *btnResume;
    TButton *btnClose;
    TTimer *pollTimer;
    void __fastcall btnReportClick(TObject *Sender);
    void __fastcall btnRetryClick(TObject *Sender);
    void __fastcall btnResumeClick(TObject *Sender);
    void __fastcall btnCloseClick(TObject *Sender);
    void __fastcall pollTimerTimer(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:
    // 수동삽입 완료 처리 단계. 각 단계의 의미와 순서는 ManualCellRecovery.h 참고.
    // 보고 버튼/타이머/재개 버튼에서 변경하며, 정상 종료 후 mcIdle로 돌아간다.
    TManualCellPhase phase;
    // toolNo: 그리퍼 번호(1부터), sourceNo/targetNo: 실제 취출/삽입 채널(1부터).
    // reservedNo: 자동작업에서 원래 예약했던 대상 채널. 실제 삽입 채널과 다를 수 있다.
    int toolNo, sourceNo, targetNo, reservedNo;
    // 보고를 시작할 때 확정한 트레이/셀 정보. 보고 중에는 현재 화면값으로 덮어쓰지 않는다.
    // cellId 등은 작업자가 입력하지 않고 ReadApprovedSource()로 수신 원본에서 가져온다.
    AnsiString sourceId, targetId, cellId, lotId, ngCode, grade;
    // workFlag: FMS에서 받은 공정가능 여부. 수동작업 진행 여부가 아니다.
    // polling: 타이머가 응답/이동 완료를 확인 중. Retry/Resume에서 켜고 Fail/완료에서 끈다.
    // restored: LoadJournal에서 이전 실행의 미완료 기록을 읽었음. 현재 실행에서는 해제하지
    //   않으며, 이전 자동 시퀀스를 임의로 복원하지 않도록 Retry/Resume을 막는다.
    // journalOkay: 복구파일 정상 여부. 생성 시 true, 읽기/저장 실패 시 false,
    //   SaveJournal 성공 시 true. false이면 신규 보고와 자동 시작을 막는다.
    // requestClearQueued: Request OFF를 전송 요청했음(전송 완료는 아님).
    //   CancelRequest에서 true, Retry에서 false. 실제 전송 완료는 타이머가 별도 확인한다.
    bool workFlag, polling, restored, journalOkay, requestClearQueued;
    // 보고 화면을 사용할 기본 정보가 준비되었음. 모든 보고 조건이 통과했다는 뜻은 아니다.
    // OpenRecovery 진입 시 false, 정보 확보/OpenManualEntry에서 true,
    // 직접 보고 완료 후 닫으면 false. 실제 채널/셀/연결 검사는 보고 버튼에서 다시 한다.
    bool contextReady;
    // true: 자동작업을 특정할 수 없어 채널을 직접 입력하고 CellTrackOut만 보고한다.
    //       로컬 트레이 기록 변경/이동/자동 재개는 하지 않는다.
    // false: 중단된 자동작업의 셀을 수동 삽입 완료하고, 기록 반영 및 다음 작업을 준비한다.
    // OpenManualEntry에서 true, 새 OpenRecovery에서 false, Load/SaveJournal로 보존한다.
    bool manualEntry;
    // 현재 응답/이동 대기의 시작 시각(GetTickCount). 단계 시작마다 갱신하여 timeout 계산.
    DWORD started;
    // 실행파일 폴더의 ManualCellCompletion.ini: 일반 로그가 아닌 미완료 작업 복구파일.
    UnicodeString JournalPath() const;
    // 프로그램 시작 시 복구파일을 읽는다. 미완료 기록은 새 작업으로 덮어쓰지 않는다.
    void LoadJournal();
    // 현재 단계와 확정된 셀/채널을 파일에 보존. 실패하면 다음 처리를 진행하지 않는다.
    bool SaveJournal();
    // 보고 당시 트레이/채널/셀이 현재 작업과 같은지 확인(보고 도중 트레이 교체 방지).
    bool ContextMatches();
    // 작업자가 이미 삽입한 결과를 Source/Target 기록에 반영/저장. 실제 서보 이동은 없음.
    // manualEntry=true이면 작업의 자동 시퀀스를 알 수 없으므로 기록 변경도 하지 않는다.
    bool ApplyPhysicalCompletion();
    // 오류 표시/로그/알람 등록 후 타이머 진행을 중지. phase는 유지하여 재시도 판단에 사용.
    void Fail(const UnicodeString &message);
    // 현재 단계에 따라 입력/버튼 허용 상태와 트레이 ID 표시를 갱신한다.
    void RefreshControls();
    // Request/Response 초기화까지 남은 진행 상태를 별도 라벨에 표시한다.
    void RefreshFmsWaitLabel();
    // 자동작업 정보가 없으면 현재 트레이 ID 표시 + 양쪽 채널 직접 입력 화면을 연다.
    void OpenManualEntry();
    // 직접 입력 채널/현재 트레이/수신 셀 정보/연결 상태 검증 후 작업자에게 최종 확인.
    bool ReadManualEntry();
    // FMS Request를 OFF로 요청할 뿐, 물리 작업이나 완료 기록을 취소하지는 않는다.
    void CancelRequest();
    // 기존 자동작업과 동일한 CellTrackOut 요청을 보내고 응답 대기를 시작한다.
    void SendRequest();
public:
    __fastcall TManualCompleteForm(TComponent *Owner);
    // true: 미완료 처리 또는 복구파일 오류가 있어 자동 시작/작업 초기화를 막아야 한다.
    // 창이 보이는지 여부와 무관하다. FMS 완료 후에도 대기위치 복귀/닫기까지 true이다.
    bool IsBlocking() const { return phase != mcIdle || !journalOkay; }
    void ApplyLanguage();
    // 중단된 자동작업 정보를 우선 사용. 없으면 직접 보고 화면, 미완료 기록이 있으면 유지.
    void OpenRecovery(int ToolNo);
};
extern PACKAGE TManualCompleteForm *ManualCompleteForm;
#endif
