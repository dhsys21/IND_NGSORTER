# NGSORTER operation manual template contract

Reference: D:/Project/2026/03.Jeng(India)/01.Document/설비매뉴얼/IROCV_96CH_India_Operation_Manual_KR_20260916 001.docx
Retain the reference unchanged. SHA-256 and complete ZIP member inventory are generated in evidence.json by the builder before authoring.

## Page and typography
One A4 portrait section, 11906 x 16838 twips. Margins top1701/bottom567/left1440/right1440; header567/footer992. First page differs; numbering starts0, so cover is unnumbered. Preserve sectPr and original header logo/footer PAGE field.
Normal: Malgun Gothic (맑은 고딕), 9pt, 4pt after. H1 style10: blue2F75B5 15pt bold, keepNext, direct before7pt/after6pt. H2 style20: black12pt bold. Caption style16: gray595959 8pt. Reuse source paragraph properties and table components, not a generic preset.
Cover: source body paragraph indexes0-7, centered year blue band, 28pt blue equipment title, gray13pt English subtitle, identity/date, blue16pt manufacturer near bottom. Replace identity only; preserve source direct spacing.
Tables: source tables0(two-column),5(three-column),18(four-column); fixed grids, pale-blue D9EAF7 header, gray A6A6A6 half-point borders, 70twip cell margins, 8pt text, centered header. Clone rows/columns; allow body row height to grow, repeat header.

## Components and editable slots
Body document.xml is editable. Rewrite original equipment-specific prose/tables, remove IR/OCV-specific screenshot paragraphs and replace with NGS equipment drawing or clearly labeled editable screen-component guides. Preserve cover, heading/caption rhythm, 9-chapter structure. Add NGS procedures using repeated source paragraph/table patterns; additional pages allowed as content requires.
Styles, theme, numbering, customXml, original media, header relationships, footnotes/endnotes are preserve-only. Header/footer text can replace IR/OCV identity if present. Main document relationships may add the verified Assets/NGSORTER.png drawing. Settings updateFields can change. Core document title/subject can change. Preserve all other ZIP members byte-for-byte.
TOC: rebuild from NGS headings with matching toc1/toc2 styles and PAGEREF fields; bookmarks generated uniquely. Word refresh required for final page references. Do not reuse source TOC numbers.

## Content map
Cover; document information/revision; TOC; 1 system and communication; 2 safety/checklist; 3 screens(mode/tray,96-channel maps,Teaching,Config,Interface); 4 startup/shutdown; 5 automatic sequence,FMS,target exchange; 6 manual/JOG/teaching/manual completion; 7 alarms,FMS,doors,EMG recovery; 8 saved records/logs/checks; 9 I/O/PLC/FMS status.
Operational claims are checked against current FormMain, ModRobostar, ModGripper, FormDoor, FormManualComplete, ModMes_OPCUA, ModPLC_Bin and Define. RESUME WORK has no click handler; use START/Restart. Do not copy IR/OCV probe, measurement, remeasurement or controller command procedures. Door polarity and Y003C/Y003D semantics come from current code, not old chat.
NGS screen captures unavailable: use labeled functional guides, not fabricated screenshots. Existing machine drawing is actual project asset. No production program launch or machine commands.

## Fidelity and QA
Reference rendering with render_docx.py failed: bundled LibreOffice unavailable on Windows. Native Word read-only PDF export is being attempted; Word automation currently awaits a dialog/response. Do not terminate user Word processes. Final QA must use Word export and page PNGs when available; record any remaining verification limitation honestly.
Compare ZIP preserve-only hashes, unchanged sectPr/styles, all intended headings/tables, absence of leftover IROCV procedures, correct references, and all final rendered pages. Final output is DOCX only.

## Final verification
Microsoft Word native COM export succeeded using C# after PowerShell COM document-property failures. Final DOCX is 27 pages, including the cover. All 27 pages were rasterized at 1.5x and visually inspected, followed by the 10 pages whose glyph rasterization differed between the Word-saved QA copy and the preserve-only final package. No clipping, overlapping text, blank pages or broken tables were found. Page text is identical in the two PDFs. All 40 TOC page references are populated. Original reference SHA-256 is unchanged; non-body parts of the generated package are preserved when importing the rendered TOC fields. Production git status shows only new outputs/ and tmp/NGSManual/ artifacts.
