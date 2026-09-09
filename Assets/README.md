# FormDoor drawing

`NGSORTER.png` is the original 1394 x 614 machine drawing recovered byte-for-byte
from commit `641805bbca6b21791e6c77b1d8af269c9a9155e4` (`Win32/Debug/NGSORTER.png`).
Git blob: `fb7c97267645d33df4200630eb837ae7c6d29c03`.

Keep this source asset under version control. `myres.rc` embeds it as
`NGSORTER_DRAWING` RCDATA so Debug/Release EXEs work without a loose PNG file.
FormDoor loads it after DFM streaming; do not reinsert a large `Picture.Data`
property (previous form construction reported EReadError).

A valid `NGSORTER.png` beside the EXE, or the legacy `D:\Program\NGSORTER.png`,
still overrides the embedded image. Missing/invalid external files fall back
to the resource. Designer image dimensions and alarm overlay positions remain
unchanged. Rebuild after replacing the PNG.
