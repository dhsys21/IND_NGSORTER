from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED
from lxml import etree as E
from copy import deepcopy
from hashlib import sha256
import io,json
root=Path(__file__).resolve().parent
original=next(root.glob('NGSORTER*.docx'))
rendered=root/'QADynamic.docx'
out=root.parents[1]/'outputs'/original.name
out.parent.mkdir(exist_ok=True)
a=ZipFile(original);b=ZipFile(rendered)
ns={'w':'http://schemas.openxmlformats.org/wordprocessingml/2006/main'}
x=E.fromstring(a.read('word/document.xml'));y=E.fromstring(b.read('word/document.xml'))
def fields(xml):
    return {''.join(p.xpath('.//w:instrText/text()',namespaces=ns)).strip():p for p in xml.xpath('//w:p[w:r/w:instrText]',namespaces=ns)}
af,bf=fields(x),fields(y)
assert len(af)==40 and set(af)==set(bf)
for key,p in af.items():
    assert key.startswith('PAGEREF NGSSection')
    p.getparent().replace(p,deepcopy(bf[key]))
buf=io.BytesIO()
with ZipFile(buf,'w',ZIP_DEFLATED) as z:
    for info in a.infolist():
        data=E.tostring(x,xml_declaration=True,encoding='UTF-8',standalone=True) if info.filename=='word/document.xml' else a.read(info.filename)
        z.writestr(deepcopy(info),data)
out.write_bytes(buf.getvalue())
with ZipFile(out) as z:
    assert all(z.read(n)==a.read(n) for n in a.namelist() if n!='word/document.xml')
e=json.loads((root/'evidence.json').read_text(encoding='utf-8'))
assert sha256(Path(e['reference']).read_bytes()).hexdigest()==e['sha256']
print(out)
print('40 TOC page references updated; all other parts preserved; original reference unchanged')
