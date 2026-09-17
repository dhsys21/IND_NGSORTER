from pathlib import Path
import json, re
from pypdf import PdfReader
from docx import Document
from docx.oxml.ns import qn

root = Path(__file__).resolve().parent
pdf = PdfReader(root/'final.pdf')
assert len(pdf.pages) == 44, len(pdf.pages)
texts = [page.extract_text() for page in pdf.pages]
heads = json.loads((root/'headings.json').read_text(encoding='utf-8'))
page_map = json.loads((root/'toc_pages.json').read_text(encoding='utf-8'))
norm = lambda s: re.sub(r'\s', '', s)
for h in heads:
    hits = [i for i, text in enumerate(texts) if i >= 3 and norm(h) in norm(text)]
    assert hits == [page_map[h]], (h, hits)
    assert norm(h + str(page_map[h])) in norm(re.sub(r'\.{2,}', '', texts[2])), h
assert list(page_map.values()) == list(range(3, 43))
assert 'B.2' in texts[43]
assert 'Rev. 0.3' in texts[0]

path = next(root.glob('*.docx'))
doc = Document(path)
body_text = []
for el in doc._element.body:
    text = ''.join(el.xpath('.//w:t/text()'))
    if text == '부록 B 확인사항':
        break
    body_text.append(text)
body_text = '\n'.join(body_text)
for bad in ['노트북', '참고 자료', '제공 캡처', '현재 코드', '현행 코드', '이력 기준', '미확정', '과거 캡처', '운영 매뉴얼과', '같은 양식', '그림 설명용', '계획안']:
    assert bad not in body_text, bad
assert len(doc.inline_shapes) == 19, len(doc.inline_shapes)
assert all(h in page_map for h in heads)

script = root.parent.parent/'NGSManual/qa_pages.py'
exec(compile(script.read_text(encoding='utf-8'), str(root/'qa_pages.py'), 'exec'), {'__file__': str(root/'qa_pages.py')})
print('PASS 44 pages; 40 TOC entries and page numbers; body editorial checks; 19 original images')
