from pathlib import Path
from copy import deepcopy
from zipfile import ZipFile, ZIP_DEFLATED
import io, re, json, sys
from docx import Document
from docx.text.paragraph import Paragraph
from docx.table import Table
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.shared import Inches, Pt
from lxml import etree as E
from pypdf import PdfReader

ROOT = Path(__file__).resolve().parent
OUTDIR = ROOT / 'formdoor'
BASE = Path('D:/Project/2026/03.Jeng(India)/01.Document/설비매뉴얼/NGSORTER_96CH_India_Operation_Manual_KR_20260916 001.docx')
OUT = OUTDIR / 'NGSORTER_96CH_India_Operation_Manual_KR_20260916 002.docx'
TITLE = '3.7 도어 및 비상정지 알람 화면'

if len(sys.argv) > 1 and sys.argv[1] == 'toc':
    pdf = PdfReader(OUTDIR / 'draft.pdf')
    pages = [''.join(p.extract_text().split()) for p in pdf.pages]
    ns = {'w': 'http://schemas.openxmlformats.org/wordprocessingml/2006/main'}
    with ZipFile(OUT) as z:
        parts = {i.filename: (deepcopy(i), z.read(i.filename)) for i in z.infolist()}
    xml = E.fromstring(parts['word/document.xml'][1])
    records = []
    for p in xml.xpath('//w:body/w:p[w:pPr/w:pStyle[@w:val="11" or @w:val="21"]]', namespaces=ns):
        texts = p.xpath('.//w:t', namespaces=ns)
        title = ''.join(t.text or '' for t in texts[:-1])
        key = ''.join(title.split())
        hits = [i for i in range(3, len(pages)) if key in pages[i]]
        assert hits, title
        number = hits[0]
        texts[-1].text = str(number)
        records.append([title, number])
    assert len(records) == 42, len(records)
    parts['word/document.xml'] = (parts['word/document.xml'][0], E.tostring(xml, xml_declaration=True, encoding='UTF-8', standalone=True))
    with ZipFile(OUT, 'w', ZIP_DEFLATED) as z:
        for info, payload in parts.values(): z.writestr(info, payload)
    (OUTDIR / 'toc-pages.json').write_text(json.dumps(records, ensure_ascii=False, indent=2), encoding='utf-8')
    print('Updated 42 TOC page numbers')
    sys.exit()

raw = BASE.read_bytes()
doc = Document(io.BytesIO(raw))
def find(text): return next(p for p in doc.paragraphs if p.text == text)
def replace(p, value):
    for r in p.runs: r.text = ''
    p.add_run(value)

body_template = next(p for p in doc.paragraphs if p.text.startswith('불량선별기는'))
heading_template = find('3.5 Configuration')
anchor = find('4. 기동 및 종료')._p
def para(value, heading=False, page=False, caption=False):
    el = deepcopy((heading_template if heading else body_template)._p)
    for c in list(el):
        if c.tag != qn('w:pPr'): el.remove(c)
    p = Paragraph(el, doc._body)
    p.paragraph_format.page_break_before = page
    if caption: p.style = doc.styles['16']
    p.add_run(value)
    anchor.addprevious(el)
    return p

def table(headers, rows):
    template = next(t for t in doc.tables if len(t.columns) == 2)
    el = deepcopy(template._tbl)
    original = Table(el, doc._body)
    prototypes = [deepcopy(original.rows[0]._tr), deepcopy(original.rows[1]._tr)]
    for tr in list(el.findall(qn('w:tr'))): el.remove(tr)
    widths = [2350, 6721]
    for c, width in zip(el.find(qn('w:tblGrid')), widths): c.set(qn('w:w'), str(width))
    for n, values in enumerate([headers] + rows):
        tr = deepcopy(prototypes[0 if n == 0 else 1])
        for h in tr.xpath('./w:trPr/w:trHeight'): h.getparent().remove(h)
        for i, tc in enumerate(tr.findall(qn('w:tc'))):
            tc.find(qn('w:tcPr')).find(qn('w:tcW')).set(qn('w:w'), str(widths[i]))
            p = tc.find(qn('w:p'))
            pp = deepcopy(p.find(qn('w:pPr')))
            for c in list(tc):
                if c.tag != qn('w:tcPr'): tc.remove(c)
            p = OxmlElement('w:p')
            if pp is not None: p.append(pp)
            r = OxmlElement('w:r'); t = OxmlElement('w:t'); t.text = values[i]
            r.append(t); p.append(r); tc.append(p)
        el.append(tr)
    anchor.addprevious(el)

para(TITLE, heading=True, page=True)
para('도어 열림 또는 비상정지가 감지되면 FormDoor 알람 화면이 표시됩니다. 새 알람 발생 시 프로그램은 Pause 및 MANUAL 전환을 요청하고, 알람과 로그를 기록합니다. 화면 전환만으로 실제 축 정지가 완료되었다고 판단하지 않습니다.')
p = para(''); p.paragraph_format.keep_with_next = True
p.add_run().add_picture(str(OUTDIR / 'formdoor.png'), width=Inches(6.25))
para('그림 3-8. FormDoor 실제 화면의 비상정지 알람 표시 예', caption=True)
table(['화면 영역', '확인 내용'], [
    ['Alarm Position', '설비 도면 위에 DOOR #1, DOOR #2, Emergency Stop 및 KEYLOCK 관련 위치와 상태를 표시합니다. 발생 항목은 점멸할 수 있습니다.'],
    ['Alarm Information', '오른쪽 영역에서 현재 알람명과 도어 닫기·비상정지 해제·안전회로 복구 안내를 확인합니다.'],
    ['X002B / X002C', '비상정지 안전회로 / 도어 안전회로의 준비 신호입니다. 각각 ON이면 녹색, OFF이면 회색입니다. 두 신호만으로 Servo ON 또는 HOME 완료를 뜻하지는 않습니다.'],
    ['복구 버튼', '현재 상태에 따라 SERVO OPEN, SAFETY RESET, KEYLOCK 및 BYPASS 버튼을 사용합니다. 확인(OK) 버튼은 복구 조건이 성립해야 표시됩니다.'],
])
para('DOOR #1/#2는 해당 도어 신호가 열림으로 해석될 때, Emergency Stop은 비상정지 상태일 때 표시됩니다. AUTO 중 KEYLOCK 해제, 서보 보드가 열린 상태에서 시스템 준비가 해제된 경우에도 이 화면을 사용합니다.')

para('FormDoor 버튼과 복구 확인', heading=True, page=True)
table(['버튼 또는 표시', '역할 및 주의 사항'], [
    ['Buzzer Stop', '부저만 끕니다. 알람 원인 해제, 안전회로 복구 또는 자동 재시작 기능이 아닙니다.'],
    ['Set KEYLOCK', '도어 닫힘 등 허용 조건을 확인한 뒤 도어 잠금 시퀀스를 요청합니다. 불가 안내가 나오면 원인을 먼저 확인합니다.'],
    ['Release KEYLOCK', '허용 조건에서 도어 잠금 해제를 요청합니다. 내부 접근 전 실제 정지 및 현장 안전 절차를 확인합니다.'],
    ['Set BYPASS', '조건 확인 후 BYPASS 키 잠금 해제 출력(Y003C)을 요청합니다. 작업자가 물리 BYPASS 키를 정상 OFF 위치로 돌리는 조작과는 별개입니다.'],
    ['SAFETY RESET', '안전회로 Reset 펄스(Y0032)를 요청합니다. CC-Link가 연결되고 조작이 허용된 상태에서 사용하며, X002B/X002C가 모두 ON인지 확인합니다.'],
    ['SERVO OPEN', '서보 보드 및 시스템 초기화를 요청합니다. 이 버튼만으로 Servo ON이나 HOME이 완료되지는 않습니다.'],
    ['확인 / OK', '도어·비상정지·서보 시스템 준비 조건이 복구되면 표시됩니다. 클릭 시 KEYLOCK 설정 가능 여부를 다시 확인한 뒤 알람 화면을 닫습니다. 자동운전은 별도로 재개합니다.'],
    ['GRIPPER OPEN / STOP MOVING', 'HOME 복구 등 해당 상태에서 추가로 표시될 수 있습니다. 보유 셀의 낙하 위험을 확인하고 승인된 복구 절차에서만 조작합니다.'],
])
para('도어 열림 복구: 실제 정지를 확인한 후 원인을 제거하고 도어를 닫습니다. KEYLOCK과 BYPASS를 정상 운전 상태로 복구하며, 안전회로가 꺼졌다면 SAFETY RESET을 수행합니다. 상세 조작 순서는 7.3절을 따릅니다.')
para('비상정지 복구: 원인 제거 및 안전 확인 → 비상정지 해제 → 도어·KEYLOCK·BYPASS 정상화 → SAFETY RESET 및 X002B/X002C 확인 → SERVO OPEN → 확인(OK) → 메인 화면에서 Servo ON/HOME 순으로 준비합니다. 셀 보유 상태와 재개 조건은 7.4절을 확인합니다.')
para('확인 버튼이 없거나 화면이 다시 나타나면 남아 있는 도어·비상정지·서보 준비 조건과 통신 상태를 확인합니다. 알람창 숨김이나 시험용 Door/Auto 옵션으로 정상 복구를 대신하지 않습니다. 원인 해제 후에도 AUTO와 START/Restart는 작업자가 별도로 실행합니다.')

# Keep the user's existing manual intact and add only the new screen entry.
toc_anchor = next(p for p in doc.paragraphs if p.text.startswith('4. 기동 및 종료\t'))
toc_template = next(p for p in doc.paragraphs if p.text.startswith('3.6 PLC FMS Interface') and '\t' in p.text)
el = deepcopy(toc_template._p)
tp = Paragraph(el, doc._body)
for c in list(el):
    if c.tag != qn('w:pPr'): el.remove(c)
tp.add_run(TITLE); tp.add_run('\t'); tp.add_run('18')
toc_anchor._p.addprevious(el)
for p in doc.paragraphs:
    if p.style.style_id in ('11', '21'): p.paragraph_format.space_after = Pt(0)

replace(find('5. Servo OPEN, ON, HOME을 실행하고 전 축 정지와 알람 상태를 확인합니다. 보유 셀을 임의로 UNCHUCK하지 않습니다.'),
        '5. FormDoor의 SERVO OPEN으로 시스템을 준비한 뒤 확인(OK) 버튼이 표시되면 누릅니다. 메인 화면에서 Servo ON, HOME을 실행하고 전 축 정지와 알람 상태를 확인합니다. 보유 셀을 임의로 UNCHUCK하지 않습니다. 화면과 버튼 설명은 3.7절을 참고합니다.')

buf = io.BytesIO(); doc.save(buf)
changed = ZipFile(buf)
with ZipFile(io.BytesIO(raw)) as original, ZipFile(OUT, 'w', ZIP_DEFLATED) as result:
    replace_parts = {'word/document.xml', 'word/_rels/document.xml.rels', '[Content_Types].xml'}
    for info in original.infolist():
        payload = changed.read(info.filename) if info.filename in replace_parts else original.read(info.filename)
        result.writestr(deepcopy(info), payload)
    for info in changed.infolist():
        if info.filename not in original.namelist(): result.writestr(deepcopy(info), changed.read(info.filename))
print(OUT)
