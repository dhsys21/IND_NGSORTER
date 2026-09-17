from pathlib import Path
import pypdfium2 as pdfium
from PIL import Image, ImageDraw
from pypdf import PdfReader
root=Path(__file__).resolve().parent
pdf=pdfium.PdfDocument(str(root/'final.pdf'))
out=root/'pages';out.mkdir(exist_ok=True)
for i,page in enumerate(pdf):
    im=page.render(scale=1.5).to_pil().convert('RGB')
    im.save(out/f'page-{i+1:02}.png')
for start in range(0,len(pdf),6):
    sheet=Image.new('RGB',(1800,1740),'#bbbbbb')
    draw=ImageDraw.Draw(sheet)
    for j in range(start,min(start+6,len(pdf))):
        im=Image.open(out/f'page-{j+1:02}.png');im.thumbnail((584,825))
        x=(j-start)%3*600+8;y=(j-start)//3*870+25
        sheet.paste(im,(x,y));draw.text((x,y-18),f'PAGE {j+1}',fill='black')
    sheet.save(out/f'sheet-{start//6+1}.png')
reader=PdfReader(root/'final.pdf')
for i,page in enumerate(reader.pages):
    text=page.extract_text()
    print(i+1,len(text),text[:100].replace('\n',' | '), '...',text[-80:].replace('\n',' | '))
(root/'rendered_text.txt').write_text('\n\n'.join(p.extract_text() for p in reader.pages),encoding='utf-8')
