from pathlib import Path
import json,re
from pypdf import PdfReader
root=Path(__file__).resolve().parent
heads=json.loads((root/'headings.json').read_text(encoding='utf-8'))
pdf=PdfReader(root/'final.pdf')
texts=[page.extract_text() for page in pdf.pages]
page_map={}
for h in heads:
    matches=[]
    for i,text in enumerate(texts):
        if i<3:continue
        if re.sub(r'\s','',h) in re.sub(r'\s','',text):matches.append(i)
    assert len(matches)==1,(h,matches)
    page_map[h]=matches[0]
(root/'toc_pages.json').write_text(json.dumps(page_map,ensure_ascii=False,indent=2),encoding='utf-8')
assert len(pdf.pages)==40,len(pdf.pages)
assert list(page_map.values())==list(range(3,40)),page_map
print('PASS 40 pages, all 37 section headings on expected pages')
script=root.parent/'NGSManual/qa_pages.py'
exec(compile(script.read_text(encoding='utf-8'),str(root/'qa_pages.py'),'exec'),{'__file__':str(root/'qa_pages.py')})
