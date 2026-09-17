from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED
from copy import deepcopy
from lxml import etree as E
import json, re
from pypdf import PdfReader

base = Path('D:/Project/2026/03.Jeng(India)/01.Document/설비매뉴얼/NGSORTER_96CH_India_Operation_Manual_KR_20260916 002.docx')
outdir = Path(__file__).resolve().parent / 'toc-fix'
outdir.mkdir(exist_ok=True)
out = outdir / 'NGSORTER_96CH_India_Operation_Manual_KR_20260916 003.docx'
ns = {'w': 'http://schemas.openxmlformats.org/wordprocessingml/2006/main'}
tag = lambda name: '{' + ns['w'] + '}' + name
records = []
verified_pdf = PdfReader(Path(__file__).resolve().parent / 'rev002/final.pdf')
page_map = {}
normalize = lambda value: ''.join(value.split())
for line in verified_pdf.pages[2].extract_text().splitlines():
    match = re.match(r'^(.*?)\.{3,}\s*(\d+)\s*$', line.strip())
    if match:
        page_map[normalize(match.group(1))] = int(match.group(2))
assert len(page_map) == 41
with ZipFile(base) as src:
    xml = E.fromstring(src.read('word/document.xml'))
    toc = xml.xpath('//w:p[w:r/w:instrText[contains(., "PAGEREF NGSSection")]]', namespaces=ns)
    assert len(toc) == 41
    for p in toc:
        runs = list(p.findall(tag('r')))
        title = ''.join(runs[0].xpath('.//w:t/text()', namespaces=ns))
        in_result = False
        result = []
        for r in runs:
            f = r.find(tag('fldChar'))
            if f is not None:
                kind = f.get(tag('fldCharType'))
                if kind == 'separate':
                    in_result = True
                elif kind == 'end':
                    in_result = False
                p.remove(r)
            elif r.find(tag('instrText')) is not None:
                p.remove(r)
            elif in_result:
                texts = r.findall(tag('t'))
                number = page_map[normalize(title)]
                assert normalize(title) in normalize(verified_pdf.pages[number].extract_text())
                assert len(texts) == 1
                texts[0].text = str(number)
                result.append(str(number))
                # Retain the resolved field value as ordinary visible text.
                rp = r.find(tag('rPr'))
                if rp is not None:
                    for el in list(rp):
                        if el.tag in (tag('vanish'), tag('webHidden')):
                            rp.remove(el)
        number = ''.join(result)
        assert number.isdigit(), (title, number)
        records.append({'heading': title, 'page': int(number)})
    assert not xml.xpath('//w:instrText[contains(., "PAGEREF NGSSection")]', namespaces=ns)
    with ZipFile(out, 'w', ZIP_DEFLATED) as dst:
        for info in src.infolist():
            payload = E.tostring(xml, xml_declaration=True, encoding='UTF-8', standalone=True) if info.filename == 'word/document.xml' else src.read(info.filename)
            dst.writestr(deepcopy(info), payload)
    with ZipFile(out) as dst:
        assert all(dst.read(n) == src.read(n) for n in src.namelist() if n != 'word/document.xml')
        before = E.fromstring(src.read('word/document.xml'))
        before_toc = before.xpath('//w:p[w:r/w:instrText[contains(., "PAGEREF NGSSection")]]', namespaces=ns)
        for el in before_toc:
            el.getparent().remove(el)
        after = deepcopy(xml)
        for el in after.xpath('//w:body/w:p[w:pPr/w:pStyle[@w:val="11" or @w:val="21"]]', namespaces=ns):
            el.getparent().remove(el)
        assert E.tostring(before) == E.tostring(after), 'Non-TOC body content changed'
(outdir / 'toc-pages.json').write_text(json.dumps(records, ensure_ascii=False, indent=2), encoding='utf-8')
print(out)
print('41 TOC page numbers converted to plain text; all non-TOC content preserved')
