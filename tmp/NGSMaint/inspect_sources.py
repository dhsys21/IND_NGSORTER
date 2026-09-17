from pathlib import Path
from pypdf import PdfReader
from PIL import Image, ImageDraw, ImageFont
from zipfile import ZipFile
from lxml import etree as E
import json

ROOT=Path(__file__).resolve().parent
M=Path('D:/Project/2026/03.Jeng(India)/03.Manual')
ROOT.mkdir(exist_ok=True)
for folder in ['Position Board','시리얼카드','연기감지기','Keyence SR-X100']:
    images=[p for p in (M/folder).iterdir() if p.suffix.lower() in ('.png','.jpg')]
    images.sort(key=lambda p:p.name)
    font=ImageFont.truetype('C:/Windows/Fonts/malgun.ttf',18)
    for start in range(0,len(images),6):
        sheet=Image.new('RGB',(1800,1470),'#cccccc');d=ImageDraw.Draw(sheet)
        for j,p in enumerate(images[start:start+6]):
            im=Image.open(p).convert('RGB');im.thumbnail((885,445))
            x=(j%2)*900;y=(j//2)*490
            sheet.paste(im,(x,y+35));d.text((x+5,y+5),p.name,font=font,fill='black')
        sheet.save(ROOT/f'{folder}-{start//6+1}.png')
pdfs=list((M/'연기감지기').glob('*.pdf'))+list((M/'Power Meter').glob('*.pdf'))+list((M/'MR-MC2XX 매뉴얼').glob('[123]*.pdf'))+[M/'[CCLINK] CCBD2/Manual/English/sh080527engw.pdf']
for p in pdfs:
    reader=PdfReader(p)
    texts=[page.extract_text() or '' for page in reader.pages]
    (ROOT/(p.stem+'.txt')).write_text('\n'.join(f'\n=== PAGE {i+1} ===\n{t}' for i,t in enumerate(texts)),encoding='utf-8')
    print(p.name,len(texts),'pages')
ppt=M/'Keyence SR-X100/Keyence SR-X100W 셋팅방법.pptx'
with ZipFile(ppt) as z:
    slides=[]
    for name in sorted(z.namelist()):
        if name.startswith('ppt/slides/slide') and name.endswith('.xml'):
            x=E.fromstring(z.read(name));slides.append(name+'\n'+'\n'.join(x.xpath('//*[local-name()="t"]/text()')))
    (ROOT/'keyence_ppt.txt').write_text('\n\n'.join(slides),encoding='utf-8')
