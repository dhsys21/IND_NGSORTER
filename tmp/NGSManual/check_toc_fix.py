from pathlib import Path
import pypdfium2 as pdfium
from pypdf import PdfReader
from PIL import Image, ImageChops
import json

root=Path(__file__).resolve().parent
current=root/'toc-fix'
pdf=pdfium.PdfDocument(str(current/'final.pdf'))
oldreader=PdfReader(root/'rev002/final.pdf')
reader=PdfReader(current/'final.pdf')
assert len(pdf)==len(oldreader.pages)==33
out=current/'pages'
out.mkdir(exist_ok=True)
different=[]
for i,p in enumerate(pdf):
    im=p.render(scale=1.5).to_pil().convert('RGB')
    im.save(out/f'page-{i+1:02}.png')
    old=Image.open(root/f'rev002/pages/page-{i+1:02}.png').convert('RGB')
    if ImageChops.difference(im,old).getbbox(): different.append(i+1)
    assert reader.pages[i].extract_text()==oldreader.pages[i].extract_text(), f'Page text changed: {i+1}'
print('All 33 page texts identical; visually changed page numbers:',different)
records=json.loads((current/'toc-pages.json').read_text(encoding='utf-8'))
for item in records:
    # The cover is unnumbered; printed page 3 is PDF page index 3.
    text=reader.pages[item['page']].extract_text()
    assert ''.join(item['heading'].split()) in ''.join(text.split()),item
print('All 41 TOC numbers match their heading pages')
