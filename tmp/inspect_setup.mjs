import { FileBlob, SpreadsheetFile } from "@oai/artifact-tool";

const source = process.argv[2];
const workbook = await SpreadsheetFile.importXlsx(await FileBlob.load(source));
const sheets = await workbook.inspect({ kind: "sheet", include: "id,name", maxChars: 4000 });
process.stdout.write(sheets.ndjson + "\n");
for (const line of sheets.ndjson.split("\n")) {
  if (!line.trim()) continue;
  const item = JSON.parse(line);
  const sheetId = item.id ?? item.sheetId;
  if (!sheetId) continue;
  const matches = await workbook.inspect({
    kind: "match",
    sheetId,
    searchTerm: "position|board|driver|motion|MC2|드라이버|보드",
    options: { useRegex: true, maxResults: 100 },
    maxChars: 12000,
  });
  if (matches.ndjson.trim()) process.stdout.write(matches.ndjson + "\n");
}
