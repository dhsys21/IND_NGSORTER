from pathlib import Path
from copy import deepcopy
from zipfile import ZipFile, ZIP_DEFLATED
from hashlib import sha256
import io, re, json
from docx import Document
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.text.paragraph import Paragraph
from docx.table import Table
from docx.shared import Inches, Pt
from lxml import etree as ET

ROOT=Path(__file__).resolve().parents[2]
TMP=Path(__file__).resolve().parent/'rev002'
TMP.mkdir(exist_ok=True)
BASE=ROOT.parent.parent/'01.Document/설비매뉴얼/NGSORTER_96CH_India_Operation_Manual_KR_20260916 001.docx'
OUT=TMP/'NGSORTER_96CH_India_Operation_Manual_KR_20260916 002.docx'
basebytes=BASE.read_bytes(); old=ZipFile(io.BytesIO(basebytes))
doc=Document(io.BytesIO(basebytes)); body=doc._element.body
ns={'w':'http://schemas.openxmlformats.org/wordprocessingml/2006/main'}

def text(el):return ''.join(el.itertext()) if False else ''.join(el.xpath('.//w:t/text()',namespaces=ns))
def find(title):
    return next(p for p in doc.paragraphs if p.text==title and not p._p.xpath('.//w:instrText'))
def write(p,value):
    runs=p.runs
    if runs:
        runs[0].text=value
        for r in runs[1:]:r.text=''
    else:p.add_run(value)
def para_before(anchor,value,style='body',page=False):
    template=find('1.3 통신 구성') if style=='h2' else next(p for p in doc.paragraphs if p.text.startswith('불량선별기는'))
    el=deepcopy(template._p)
    for c in list(el):
        if c.tag!=qn('w:pPr'):el.remove(c)
    p=Paragraph(el,doc._body);p.paragraph_format.page_break_before=page;p.add_run(value)
    anchor.addprevious(el);return p
def image_before(anchor,path,caption,width=6.15):
    p=para_before(anchor,'');p.paragraph_format.keep_with_next=True
    p.paragraph_format.space_after=Pt(4)
    p.add_run().add_picture(str(path),width=Inches(width))
    cap=para_before(anchor,caption);cap.style=doc.styles['16']
    cap.paragraph_format.space_after=Pt(8)
    return p
def fill_table(tbl,headers,rows,widths):
    el=tbl._tbl;header=deepcopy(tbl.rows[0]._tr);row=deepcopy(tbl.rows[1]._tr)
    for tr in list(el.findall(qn('w:tr'))):el.remove(tr)
    grid=el.find(qn('w:tblGrid'))
    for c in list(grid):grid.remove(c)
    for width in widths:
        c=OxmlElement('w:gridCol');c.set(qn('w:w'),str(width));grid.append(c)
    for i,values in enumerate([headers]+rows):
        tr=deepcopy(header if i==0 else row)
        pr=tr.find(qn('w:trPr'))
        if pr is None:pr=OxmlElement('w:trPr');tr.insert(0,pr)
        for h in list(pr):
            if h.tag==qn('w:trHeight'):pr.remove(h)
        for j,cell in enumerate(tr.findall(qn('w:tc'))):
            cell.find(qn('w:tcPr')).find(qn('w:tcW')).set(qn('w:w'),str(widths[j]))
            p=cell.find(qn('w:p'));pp=deepcopy(p.find(qn('w:pPr')))
            r=p.find(qn('w:r'));rp=deepcopy(r.find(qn('w:rPr'))) if r is not None else None
            for c in list(cell):
                if c.tag!=qn('w:tcPr'):cell.remove(c)
            p=OxmlElement('w:p')
            if pp is not None:p.append(pp)
            r=OxmlElement('w:r')
            if rp is not None:r.append(rp)
            for k,line in enumerate(str(values[j]).split('\n')):
                if k:r.append(OxmlElement('w:br'))
                t=OxmlElement('w:t');t.text=line;r.append(t)
            p.append(r);cell.append(p)
        el.append(tr)

# Names come directly from the 16-step process indicator, not PLC timer cases.
cpp=(ROOT/'FormMain.cpp').read_text(encoding='utf-8-sig')
block=re.search(r'static const char \*names\[16\] = \{(.*?)\};',cpp,re.S).group(1)
names=re.findall(r'"([^"]+)"',block);assert len(names)==16
details=[
 'Source Tray In(D10103)을 확인하고 Source 바코드를 읽습니다.',
 'Location1 TrayLoad 요청, 수신 데이터 검증 및 응답 초기화를 완료합니다.',
 'Source 센터링을 요청하고 D10104 ON을 확인합니다.',
 'Target In(D10105)과 Centering(D10106)을 확인하고 Target 바코드를 읽습니다.',
 'Location2 TrayLoad 및 FMS/LOCAL 비교, 응답 초기화를 완료합니다.',
 'ProcessStart 성공 응답 및 Response=0 복귀를 확인합니다.',
 '미완료 Source NG 셀을 선택하고 Target 빈 채널을 예약합니다.',
 'Z 상승 후 Source 취출 채널의 X/Y 위치로 이동합니다.',
 'Z 하강, CHUCK, 셀 감지 확인 후 Z축을 상승시킵니다.',
 'Target 삽입 채널의 X/Y 위치로 이동합니다.',
 'Z 하강, UNCHUCK, 셀 해제 확인 후 Z축을 상승시킵니다.',
 '이재 결과를 저장하고 CellTrackOut 성공 응답 및 초기화를 확인합니다.',
 '대기위치/다음 NG를 확인합니다. 남은 NG는 STEP 07부터 반복합니다.',
 '남은 NG가 없으면 누적 결과를 반영하고 ProcessEnd를 완료합니다.',
 'Source 센터링 요청 해제 후 PLC에 Source Tray Out을 명령합니다.',
 'Target 배출 조건 성립 시 TrayUnload 완료 후 Target을 배출합니다.'
]
oldseq=next(t for t in doc.tables if t.cell(0,0).text=='순서' and t.cell(0,1).text=='공정 흐름')
# The third column is cloned from the reference three-column table component.
template=next(t for t in doc.tables if len(t.columns)==3)
seq=Table(deepcopy(template._tbl),doc._body)
fill_table(seq,['STEP','프로그램 표시 명칭','동작'],[[f'{i+1:02}',names[i],details[i]] for i in range(16)],[650,3000,5421])
oldseq._tbl.getparent().replace(oldseq._tbl,seq._tbl)
write(find('그림 5-1. 일반 자동 선별 흐름'),'표 5-1. 프로그램 공정 표시와 일치하는 16단계 자동 시퀀스')
write(find('Source와 Target의 정보가 정상이어야 실제 취출·삽입이 시작됩니다. Target 교환 중에는 해당 Source의 남은 작업을 보존하며, 새 Target의 바코드와 FMS/LOCAL 확인이 끝나야 다음 삽입이 가능합니다.'),
      'STEP 01~05는 Source 입고 → Source 정보 수신 → Source 센터링 → Target 준비 → Target 정보 수신 순서입니다. 두 트레이의 정보와 센터링이 확인되어야 STEP 06 이후로 진행합니다.')

check=next(t for t in doc.tables if t.cell(0,0).text=='Step' and t.cell(0,1).text=='작업')
checks=[
 ['01','SOURCE TRAY IN','PC/PLC AUTO, Source Tray In 및 바코드','입고 신호·바코드 리더 확인'],
 ['02','SOURCE TRAY LOAD','Location1 데이터, Response=1 후 0','FMS 알람 단계 확인'],
 ['03','SOURCE CENTERING','D10104 ON, 센터링 완료','PLC AUTO·센터링 장치 확인'],
 ['04','TARGET TRAY READY','D10105/10106 ON, Target 바코드','Target 입고·센터링 확인'],
 ['05','TARGET TRAY LOAD','FMS/LOCAL 일치 및 빈 슬롯','실물과 셀·트레이 정보 대조'],
 ['06','PROCESS START','성공 응답과 초기화 완료','FMS 오류 복구 절차 사용'],
 ['07','NG CHANNEL SELECT','미완료 Source NG, Target 예약','남은 NG·만셀·셀 ID 확인'],
 ['08','MOVE EJECT CHANNEL','Z 상승과 Source X/Y 위치','축 위치·Motion 알람 확인'],
 ['09','CELL EJECT','CHUCK·셀 감지·Z 상승 완료','취출 오류창에서 원인 확인'],
 ['10','MOVE INSERT CHANNEL','Target 채널 X/Y 위치','축 위치·Motion 알람 확인'],
 ['11','CELL INSERT','UNCHUCK·셀 해제·Z 상승 완료','삽입 오류창에서 원인 확인'],
 ['12','CELL TRACK OUT','결과 저장·성공 응답·Response=0','셀을 다시 옮기지 말고 보고 복구'],
 ['13','WAIT / NEXT CHECK','대기/다음 NG·Target 빈 슬롯','다음 작업·교환 조건 확인'],
 ['14','PROCESS END','누적 결과·성공 응답·초기화','미완료 보고 확인'],
 ['15','SOURCE TRAY OUT','D10154 OFF 후 D10155 ON','배출 및 Source In 해제 확인'],
 ['16','TARGET TRAY UNLOAD','배출 조건·FMS 완료·D10156','교환 후 새 Tray ID/정보 확인']]
fill_table(check,['STEP','프로그램 표시 명칭','확인 사항','이상 시 조치'],checks,[600,2550,3221,2700])
anchor=find('실제 진행 중인 단계는 로그의 MAIN/GRIPPER/ROBOT 및 FMS 메시지로 확인합니다. 단계가 바뀌지 않는다고 해서 이전 동작을 임의로 반복하거나 RESET WORK를 누르지 않습니다.')._p
para_before(anchor,'반복 및 분기: 남은 NG가 있으면 STEP 13 → 07로 반복합니다. Target 만셀은 Source 작업 중에도 STEP 16의 교환 흐름으로 처리하고, 새 Target 준비·정보 검증 후 남은 Source 작업을 계속합니다. 설정 수량 기준 교환은 Source 선별 종료 후에 안내합니다.')

# Add the same orientation reference used by the corrected IR/OCV manual.
anchor=find('1.3 통신 구성')._p
ph=para_before(anchor,'1.3 트레이 방향 및 바코드 위치','h2',True)
image_before(anchor,ROOT.parent/'01.IROCV/outputs/operation_manual_build/manual_assets/tray_orientation_corrected.png','그림 1-2. IR/OCV와 동일한 트레이 기준 방향',6.25)
para_before(anchor,'트레이 상면을 위 그림 방향으로 보았을 때 모따기는 좌측 상단, 바코드는 우측 상단입니다. Source와 Target 모두 같은 기준 방향을 사용합니다. 트레이를 180° 돌려 투입하지 않습니다.')
para_before(anchor,'1번 채널은 우측 하단입니다. 한 열의 번호가 아래에서 위로 증가하며, 오른쪽부터 1~24, 25~48, 49~72, 73~96번으로 구성됩니다. 모따기·바코드·1번 채널 위치를 함께 확인합니다.')
para_before(anchor,'그림의 화살표는 IR/OCV 이송 방향 표시입니다. 불량선별기의 Source/Target 이송은 현장의 위치 표기와 PLC 이송 방향을 따릅니다.')
write(find('1.3 통신 구성'),'1.4 통신 구성')
write(find('1.4 용어'),'1.5 용어')

# Actual screen captures are inserted only from the confirmed capture manifest.
manifest=TMP/'captures.json'
if manifest.exists():
    captures=json.loads(manifest.read_text(encoding='utf-8'))
    for caption in ('그림 3-1. 메인 화면 기능 구성 안내','그림 3-2. Teaching 화면 기능 구성 안내'):
        p=find(caption);p._p.getparent().remove(p._p)
    for item in captures:
        head=find(item['heading']);anchor=head._p.getnext()
        image_before(anchor,Path(item['path']),item['caption'],item.get('width',6.25))
        if item.get('table_new_page'):
            # Keep the screenshot and explanation on separate, readable pages.
            nextp=para_before(anchor,item['heading']+' 조작 설명')
            nextp.paragraph_format.page_break_before=True
    anchor=find('3.6 PLC FMS Interface 및 I/O Monitoring 조작 설명')._p
    p=para_before(anchor,'I/O Monitoring 화면')
    p.paragraph_format.page_break_before=True
    image_before(anchor,TMP/'captures/io.png','그림 3-7. I/O 주소와 실제 신호 상태 확인',6.25)
    notice=find('화면 구성 안내도는 기능 위치와 명칭을 설명합니다. 설치 PC의 실제 화면 배치와 버튼 표시 여부는 버전·권한·설정에 따라 달라질 수 있습니다.')
    write(notice,'프로그램 캡처는 화면 구성과 조작 위치를 설명합니다. 캡처의 통신·알람·셀 데이터 값은 촬영 당시 상태이며 정상 운전 판정 기준이 아닙니다. 설치 버전·권한·설정에 따라 배치가 달라질 수 있습니다.')

# Rebuild TOC and bookmarks using the retained source TOC formatting.
tocpars=[p for p in doc.paragraphs if p._p.xpath('.//w:instrText')]
toc1=deepcopy(tocpars[0]._p);toc2=deepcopy(tocpars[1]._p)
tocanchor=tocpars[0]._p.getprevious()
for p in tocpars:body.remove(p._p)
heads=[]
for p in doc.paragraphs:
    if p.style.style_id not in ('10','20') or not re.match(r'^\d+\.',p.text):continue
    for x in p._p.xpath('./w:bookmarkStart|./w:bookmarkEnd'):p._p.remove(x)
    name=f'NGSSection{len(heads)+1:03}'
    start=OxmlElement('w:bookmarkStart');start.set(qn('w:id'),str(200+len(heads)));start.set(qn('w:name'),name)
    end=OxmlElement('w:bookmarkEnd');end.set(qn('w:id'),str(200+len(heads)))
    p._p.insert(1,start);p._p.append(end);heads.append((p.text,p.style.style_id,name))
for title,style,name in heads:
    el=deepcopy(toc1 if style=='10' else toc2)
    Paragraph(el,doc._body).paragraph_format.space_after=Pt(1)
    for c in list(el):
        if c.tag!=qn('w:pPr'):el.remove(c)
    r=OxmlElement('w:r');t=OxmlElement('w:t');t.text=title;r.append(t);el.append(r)
    r=OxmlElement('w:r');r.append(OxmlElement('w:tab'));el.append(r)
    for fieldtype in ('begin','instruction','separate','result','end'):
        r=OxmlElement('w:r')
        if fieldtype=='instruction':c=OxmlElement('w:instrText');c.text=f' PAGEREF {name} \\h '
        elif fieldtype=='result':c=OxmlElement('w:t');c.text=''
        else:c=OxmlElement('w:fldChar');c.set(qn('w:fldCharType'),fieldtype)
        r.append(c);el.append(r)
    tocanchor.addnext(el);tocanchor=el
for p in doc.paragraphs:
    if 'Rev. 0.1' in p.text:write(p,p.text.replace('Rev. 0.1','Rev. 0.2'))
rev=next(t for t in doc.tables if t.cell(0,0).text=='Rev.')
fill_table(rev,['Rev.','발행일','개정 내용','작성'],[
 ['0.1','2026.09.16','불량선별기 운영 매뉴얼 초판 작성','대흥시스텍'],
 ['0.2','2026.09.16',('16단계 자동 시퀀스, 프로그램 캡처 및 트레이 방향 설명 보완' if manifest.exists() else '16단계 자동 시퀀스 및 트레이 방향 설명 보완'),'대흥시스텍']],[700,1400,5500,1471])
buffer=io.BytesIO();doc.save(buffer);new=ZipFile(buffer)
changed={'word/document.xml','word/_rels/document.xml.rels','[Content_Types].xml'}
with ZipFile(OUT,'w',ZIP_DEFLATED) as z:
    for info in old.infolist():z.writestr(deepcopy(info),new.read(info.filename) if info.filename in changed else old.read(info.filename))
    for name in new.namelist():
        if name not in old.namelist():z.writestr(name,new.read(name))
assert BASE.read_bytes()==basebytes
with ZipFile(OUT) as z:
    assert all(z.read(n)==old.read(n) for n in old.namelist() if n not in changed)
print(OUT)
print('Headings',len(heads),'Captures',len(captures) if manifest.exists() else 0,'Source unchanged',sha256(basebytes).hexdigest())
