from pathlib import Path
from PIL import Image
import json

root = Path(__file__).resolve().parent / 'rev002'
cap = root / 'captures'
main = Image.open(cap / 'main.png')
main.crop((15, 90, 751, 481)).save(cap / 'tray-info.png')
main.crop((454, 575, 1913, 1032)).save(cap / 'sorting.png')
Image.open(cap / 'io-full.png').crop((585, 182, 1455, 902)).save(cap / 'io.png')
Image.open(cap / 'teaching.png').crop((9, 65, 656, 354)).save(cap / 'jog.png')

items = [
    ('3.1 메인 화면 전체', 'main.png', '그림 3-1. NG SORTER 메인 화면 실제 캡처', False),
    ('3.2 운전 모드 및 Tray 정보', 'tray-info.png', '그림 3-2. 운전 조작부와 Target 및 Source Tray 정보', True),
    ('3.3 96채널 선별 화면', 'sorting.png', '그림 3-3. 좌측 Target와 우측 Source의 96채널 표시', False),
    ('3.4 Teaching 및 수동 조작 화면', 'teaching.png', '그림 3-4. Teaching 및 수동 조작 화면 실제 캡처', True),
    ('3.5 Configuration', 'config.png', '그림 3-5. Configuration 화면 실제 캡처', True),
    ('3.6 PLC FMS Interface 및 I/O Monitoring', 'interface.png', '그림 3-6. PLC 및 MES 통신 데이터 확인 화면', True),
    ('6.2 JOG 및 Z축 속도 설정', 'jog.png', '그림 6-1. JOG 및 Z축 조작부 확대', False),
]
manifest = [dict(heading=h, path=str(cap/f), caption=c, table_new_page=p) for h,f,c,p in items]
manifest[-1]['width'] = 5.5
(root/'captures.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2), encoding='utf-8')
print('Prepared',len(manifest),'capture entries')
