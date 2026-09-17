from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED
from lxml import etree as E
from copy import deepcopy

root=Path(__file__).resolve().parent/'rev002'
original=next(root.glob('NGSORTER*.docx'))
rendered=root/'QA.docx'
out=root/'final'/original.name
out.parent.mkdir(exist_ok=True)
ns={'w':'http://schemas.openxmlformats.org/wordprocessingml/2006/main'}
with ZipFile(original) as a, ZipFile(rendered) as b:
    x=E.fromstring(a.read('word/document.xml'));y=E.fromstring(b.read('word/document.xml'))
    def fields(xml):
        return {''.join(p.xpath('.//w:instrText/text()',namespaces=ns)).strip():p for p in xml.xpath('//w:p[w:r/w:instrText]',namespaces=ns)}
    af,bf=fields(x),fields(y)
    assert len(af)==41 and set(af)==set(bf)
    for key,p in af.items():
        assert key.startswith('PAGEREF NGSSection')
        p.getparent().replace(p,deepcopy(bf[key]))
    with ZipFile(out,'w',ZIP_DEFLATED) as z:
        for info in a.infolist():
            data=E.tostring(x,xml_declaration=True,encoding='UTF-8',standalone=True) if info.filename=='word/document.xml' else a.read(info.filename)
            z.writestr(deepcopy(info),data)
    with ZipFile(out) as z:
        assert all(z.read(n)==a.read(n) for n in a.namelist() if n!='word/document.xml')
print(out)
