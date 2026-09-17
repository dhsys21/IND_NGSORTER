from pathlib import Path
import re
import json
import hashlib

ROOT = Path(__file__).resolve().parents[1]
SOURCE = Path('C:/Users/gsinm/.codex/attachments/1f470676-68e4-4ba0-937b-900e1cb69da3/pasted-text.txt')
OUT = ROOT/'tmp/NGSORTER_Error_FMS_Codes_KR_20260917 002.txt'
old = SOURCE.read_text(encoding='utf-8-sig')
header = (ROOT/'NGSorterErrors.h').read_text(encoding='cp949')
dfm = (ROOT/'FormServoAlarmList.dfm').read_text(encoding='cp949')
catalog = (ROOT/'FMS_SERVO_ERROR_CODES.md').read_text(encoding='utf-8')
assert 'scale=1000UL' in header

def encode(category, raw):
    if not raw:
        return 0
    scale = 1000
    while raw >= scale:
        scale *= 10
    return category * scale + raw

def compact(m):
    n = int(m.group())
    category, raw = divmod(n, 1000000)
    return str(encode(category, raw))

def convert(text):
    return re.sub(r'(?<!\d)(?:10|20|30|40|50)\d{6}(?!\d)', compact, text)

def section(number):
    # Retain the supplied detailed occurrence/clear descriptions without claiming
    # to have re-audited every recovery path in this documentation-only update.
    start = re.search(r'^' + str(number) + r'\. .*\n=+\n', old, re.M).end()
    end = re.search(r'^=+\n' + str(number+1) + r'\. ', old[start:], re.M)
    return old[start:start+end.start()].strip() if end else old[start:].strip()

local = convert(section(3))
local = local.replace('발생 및 해제는 현재 등록 코드 경로 기준입니다.',
    '발생·해제 설명은 기존 정리의 등록 경로 기준이며, 이번 개정은 보고번호 단축 체계를 반영합니다.')
local += '''

401 | 50401 | PowerMeterData | PLC 전력 데이터 수신 이상
  발생: 최초 정상 PLC 수신 후 D10100~D10117 데이터가 끊기거나 1,000 ms를 초과
  해제: 정상 PLC 전체 데이터 수신 복구
  주의: PLC에 파워미터 고장 비트가 없어 물리 파워미터 고장을 판정하는 코드는 아님.
        PLC 통신 오류 50152와 함께 발생 가능. 0값·동일값 반복은 오류로 처리하지 않음.

411 | 50411 | SmokeCommunication | 연기감지기 통신 이상
  발생: 설정된 COM 열기/폴링 송신 실패, 연결 해제 또는 정상 측정 응답 3회 연속 타임아웃
  해제: 유효한 측정 데이터 수신. 포트 재오픈 성공만으로는 해제하지 않음
  주의: COM 미설정은 감시하지 않음. 연결 해제 상태에서도 마지막 장치 경보는 유지

412 | 50412 | SmokeProtocol | 연기감지기 Modbus 오류 응답
  발생: 유효한 Modbus Exception 응답(83h/86h/88h)
  해제: 읽기 오류는 정상 측정 수신, 쓰기 오류는 정상 06h 응답으로 각각 해제
  주의: 읽기·쓰기 오류가 함께 있으면 둘 다 복구되어야 해제됨

413 | 50413 | SmokeNotRunning | 연기감지기 동작 정지
  발생: 정상 수신된 Modbus Status output-state bit 4=OFF
  해제: 정상 수신된 Running=ON (HumanAutomation 정상 측정 응답도 Running=true)
  주의: 아직 응답을 받지 않은 상태를 장치 동작 정지로 추정하지 않음

414 | 50414 | SmokeDetected | 연기 감지 경보
  발생: 정상 Modbus 측정의 Status alarm bit 2=ON
  해제: 정상 Modbus 측정의 해당 bit=OFF

415 | 50415 | SmokeTempWarning | 온도 경고
  발생: 정상 Modbus 측정의 Status alarm bit 0=ON
  해제: 정상 Modbus 측정의 해당 bit=OFF

416 | 50416 | SmokeTempDanger | 온도 위험
  발생: 정상 Modbus 측정의 Status alarm bit 1=ON
  해제: 정상 Modbus 측정의 해당 bit=OFF

신규 주변장치 공통 사항
  내부번호 4xx를 분류 50으로 변환하여 FMS에는 504xx로 보고합니다.
  바코드 오류는 기존 50181/50182가 있으므로 별도 4xx 번호를 중복 배정하지 않습니다.
  통신 단절·장치 비활성화만으로 연기/온도 경보를 정상 처리하지 않습니다.
  HumanAutomation은 개별 알람 비트가 없어 50414~50416 신규 판정을 하지 않으며,
  이전 Modbus 경보도 자동 해제하지 않습니다. 정상 Modbus 비트로 확인해야 합니다.
  온도 경고를 포함해 등록 에러가 있으면 Status=4, ErrorLevel=첫 활성 전체 코드입니다.
  이번 변경은 FMS 보고 추가이며 기존 정지·인터락·재시작 동작은 유지합니다.
'''
entries = re.findall(r'^(\d+) \| (\d+) \| (\w+) \| (.+)$', local, re.M)
enum = re.search(r'enum Local\s*\{(.*?)\};', header, re.S).group(1)
native_local = {name: int(num) for name, num in re.findall(r'(\w+)\s*=\s*(\d+)', enum)}
assert len(entries) == len(native_local) == 35
for raw, code, name, description in entries:
    assert native_local[name] == int(raw)
    assert int(code) == encode(50, int(raw))

rows = []
for group in range(1, 5):
    match = re.search(r'object Memo_En'+str(group)+r':.*?Lines.Strings = \((.*?)\)\r?\n', dfm, re.S)
    assert match
    values, row = [], ''
    for line in match.group(1).splitlines():
        if not line.strip():
            continue
        for token in re.findall(r"'(?:''|[^'])*'|#\d+", line):
            row += chr(int(token[1:])) if token.startswith('#') else token[1:-1].replace("''", "'")
        if not line.rstrip().endswith('+'):
            values.append(row.strip())
            row = ''
    assert len(values) % 2 == 1
    count = len(values)//2
    for code, description in zip(values[:count], values[count:count*2]):
        code = code.replace('\u25a1', '?')
        description = description.replace('|', '/').replace('\u2013', '-')
        payload = code[1:] if group == 4 else code
        if '?' in payload:
            assert code == 'E1??'
            number = f'{encode(40, 0x100)}..{encode(40, 0x1ff)}'
        else:
            number = str(encode(group*10, int(payload, 16)))
        row = (str(group*10), code, number, description)
        assert '| ' + ' | '.join(row) + ' |' in catalog, row
        rows.append(row)
assert len(rows) == 158
report = []
def add(title, text):
    report.append('='*70+'\n'+title+'\n'+'='*70+'\n'+text.strip()+'\n')

report.append('''NGSORTER 에러 및 FMS 보고코드 정리 (단축 코드 적용본)
개정일: 2026-09-17
번호 기준: 현재 NGSorterErrors.h의 Encode / SystemErrorCode / Local
알람 목록: 현재 FormServoAlarmList.dfm의 Memo_En1~4
범위: 로컬 오류 35개 + 서보/보드 알람 카탈로그 158개 항목
변경: 단축 코드 체계 유지 + 미정의 주변장치 오류 7개(401, 411~416) 및 FMS 보고 추가.
발생·해제 설명은 제공된 2026-09-09 정리본을 유지했으며,
신규 4xx 발생·해제 조건은 이번 구현 기준입니다. 실제 FMS 수신은 설비에서 확인합니다.
''')
add('1. 변경된 FMS 보고번호 규칙', '''
FMS 보고값 = 분류번호 2자리 + 원인번호를 최소 3자리로 표현한 10진수.
UInt32 숫자로 전송합니다. 원인번호가 1,000 이상이면 자릿수를 줄이지 않습니다.
따라서 현재 목록은 보통 5자리, 일부 서보/시스템 오류는 6자리입니다.

분류 10 : 서보 보드 시스템 알람 (SSC_ALARM_SYSTEM)
분류 20 : 축별 서보 알람 (SSC_ALARM_SERVO)
분류 30 : 축별 운전 알람 (SSC_ALARM_OPERATION)
분류 40 : 보드 시스템 상태 오류 (E***)
분류 50 : NGSORTER 로컬 설비 / 시퀀스 / FMS 오류

항목             | 원인번호 변환             | 이전 보고값 | 변경 보고값
도어 #1          | 내부번호 121              | 50000121    | 50121
취출 오류        | 내부번호 201              | 50000201    | 50201
FMS 수신 알람    | 내부번호 307              | 50000307    | 50307
시스템 알람 35h  | 35h = 53                  | 10000053    | 10053
서보 알람 1Ah    | 1Ah = 26                  | 20000026    | 20026
서보 알람 888h   | 888h = 2184               | 20002184    | 202184
운전 알람 B2h    | B2h = 178                 | 30000178    | 30178
시스템 오류 E001h| E 제외, 001h = 1          | 40000001    | 40001
시스템 오류 E40Bh| E 제외, 40Bh = 1035       | 40001035    | 401035

계산 예: Encode(50,307) = 50 × 1000 + 307 = 50307
         Encode(40,1035) = 40 × 10000 + 1035 = 401035
원인번호는 16진 표시를 10진수로 변환합니다. 1Ah를 1로 읽지 않습니다.
0은 정상/해제이므로 활성 ErrorNo 배열에 넣지 않습니다.
단순히 기존 번호를 1000으로 나누거나 모든 0을 삭제하면 안 됩니다.
''')
add('2. 보고 태그와 ErrorLevel / Status', '''
[설비 → FMS]
NGS.F1NGS01.EquipmentStatus.Trouble.ErrorNo
  UInt32 배열. 활성화된 중복 없는 에러코드를 발생 순서로 보고합니다.
NGS.F1NGS01.EquipmentStatus.Trouble.ErrorLevel
  첫 번째 활성 에러의 전체 코드. 에러가 없으면 0입니다.
  심각도 1/2/3이나 분류번호 10/20/30/40/50만 보내는 값이 아닙니다.
NGS.F1NGS01.EquipmentStatus.Status
  등록 에러 존재=4, 에러가 없으면 Pause=8 / 작업=2 / Idle=1.
  Pause 중에도 미해제 에러가 남으면 Status=4입니다.

예: ErrorNo=[50121,50304], ErrorLevel=50121, Status=4
도어 오류만 해제: ErrorNo=[50304], ErrorLevel=50304, Status=4
모두 해제: ErrorNo=[], ErrorLevel=0, 현재 작업 상태에 따라 Status 결정.
에러 해제만으로 이동이나 시퀀스가 자동 재개되지는 않습니다.

동일 주 알람이 여러 축에서 발생하면 배열에는 한 번만 표시합니다.
관련 축/읽기 원인이 모두 해제되어야 해당 코드를 제거합니다.
읽기 실패나 통신 단절을 정상으로 판단해 기존 에러를 지우지 않습니다.
16.3 같은 세부코드와 축 번호는 진단 화면/로그에서 확인합니다.
예를 들어 주 알람 16h의 FMS 코드는 20022입니다.

[FMS → 설비]
NGS.F1NGS01.FmsStatus.Trouble.Status
NGS.F1NGS01.FmsStatus.Trouble.ErrorNo
수신 ErrorNo는 FMS가 정한 번호 그대로 사용합니다. 단축 변환 대상이 아닙니다.
예: FMS가 9001을 보내면 수신번호는 9001이며, 설비가 보고하는
    'FMS Trouble 발생' 코드는 별도 50307입니다.

새 FMS Trouble는 Pause 및 알람창을 표시합니다. START/Retry/Restart로
인정한 같은 알람의 재진행 허용과 보고 코드 해제는 별개입니다.
인정만으로 50307을 삭제하거나 수신 Status를 강제 OFF하지 않습니다.
50307은 유효한 연결 스냅샷에서 FMS Trouble.Status=false 확인 시 해제합니다.

[기존 로그를 읽을 때의 대응 예 — 실제 로그 원문은 변경하지 않음]
이전 ErrorNo=[50000307,50000308] → 새 코드 기준 [50307,50308]
수동 Source Load 오류 해제 후 → [50307], ErrorLevel=50307, Status=4
Mode가 1에서 2로 바뀌어도 미해제 에러가 있으면 Status=4입니다.
Mode=2만으로 START 실행 또는 실제 축 이동 완료를 판단하지 않습니다.
TX_DETAIL의 success=true는 FMS 업무 응답/물리 동작 완료를 뜻하지 않습니다.
''')
add('3. 로컬 설비 / 시퀀스 / FMS 코드 전체 목록 (35개)', local)
relations = convert(section(4)).replace('온도/연기 등은 별도 EnvStatus 태그 경로이며 임의의 신규 코드로 넣지 않았습니다.',
    '온도/연기는 EnvStatus 5개 태그와 신규 50413~50416 코드를 함께 사용합니다.')
add('4. 화면 메시지와 FMS 코드의 관계', relations)
add('5. 서보 / 보드 알람 전체 대응표 (158개 항목)', '''
원래 코드: 16진수. FMS ErrorNo: 10진 UInt32.
화면 카탈로그 목록이며 현재 발생 중인 알람 목록이 아닙니다.
보드 API가 읽은 코드에 Encode()를 적용하므로 목록 밖 코드도 보고될 수 있습니다.
영문 설명은 현재 화면 정의를 유지했습니다.
E1??는 E100~E1FF이며 단축 보고 범위는 40256~40511입니다.

분류 | 원래 코드(16진수) | FMS ErrorNo(10진수) | 설명
''' + '\n'.join(' | '.join(row) for row in rows))
legacy = section(6).replace('과거 번호에 무조건 50000000을 더해 현재 코드로 변환하면 안 됩니다.',
    '과거 번호에 무조건 50000을 더해 새 코드로 변환하면 안 됩니다. 현재 Local 번호와 먼저 대조합니다.')
legacy = convert(legacy)
add('6. 과거 MES 번호와 FMS 보고번호 구분', legacy)
add('7. 적용 및 확인사항', '''
1. ErrorNo 배열과 ErrorLevel 모두 단축 코드를 사용합니다.
2. FMS 측 코드표도 단축 번호로 일치시킵니다. 태그 이름과 UInt32 타입은 동일합니다.
3. 외부에서 수신한 FMS Trouble.ErrorNo에는 단축 변환을 적용하지 않습니다.
4. 과거 로그는 원래 숫자를 보존하고 이 문서의 변경 전·후 표로 해석합니다.
5. 번호 단축으로 알람 발생·해제 조건이나 인터락을 변경한 것은 아닙니다.
6. 전체 알람 해제 후 ErrorNo=[] / ErrorLevel=0을 확인합니다.
7. 도어+서보 동시 알람, 여러 축 동일 알람, FMS 응답 후 초기화 대기 상태는
   실제 설비의 승인된 시험 절차에서 수신값과 함께 확인합니다.

확인한 파일
  NGSorterErrors.h : 최소 3자리 Encode, E*** 변환, Local 35개, 활성 알람 관리
  ModMes_OPCUA.cpp : Encode(50,Code), 첫 활성 코드 및 Status 결정
  ModMes_Gateway.cpp : ErrorNo / ErrorLevel / Status 태그 설정
  FormServoAlarmList.dfm : Memo_En1~4 영문 카탈로그 158개
  FMS_ERROR_REPORTING.md / FMS_SERVO_ERROR_CODES.md : 단축 번호 규약 및 코드표

  DetectorFmsAlarmState.h / SmokeDetector_comm.cpp : 주변장치 오류 판정 및 복구
  FormMain.cpp : PLC 전력 수신 감시 및 실제 연기감지기 인스턴스의 오류 보고

검증: 로컬 35개와 서보/보드 158개를 현재 정의 및 변환 규칙과 대조했습니다.
에러번호·알람 유지/해제 단위 테스트와 Win32 Debug 전체 빌드를 통과했습니다.
실장비/FMS 통신시험 및 전체 복구 시퀀스 재시험은 수행하지 않았습니다.
''')
result = '\n'.join(report) + '\n끝.\n'
# Old long codes are intentionally confined to the conversion and historical examples.
for block in report[3:]:
    assert not re.search(r'(?<!\d)(?:10|20|30|40|50)\d{6}(?!\d)', block), block[:80]
assert len({int(code) for _, code, _, _ in entries}) == 35
OUT.write_bytes(b'\xef\xbb\xbf' + result.replace('\n', '\r\n').encode('utf-8'))
evidence = {'local_count': len(entries), 'catalog_count': len(rows), 'group_counts': {
    str(g): sum(row[0] == str(g) for row in rows) for g in (10,20,30,40)},
    'source_sha256': hashlib.sha256(SOURCE.read_bytes()).hexdigest(),
    'output_sha256': hashlib.sha256(OUT.read_bytes()).hexdigest(), 'result': 'PASS'}
(ROOT/'tmp/error_codes_20260917_verification.json').write_text(json.dumps(evidence, indent=2), encoding='utf-8')
print(OUT)
print(json.dumps(evidence, indent=2))
