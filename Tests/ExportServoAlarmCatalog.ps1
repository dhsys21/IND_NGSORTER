$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$text=[IO.File]::ReadAllText((Join-Path $root 'FormServoAlarmList.dfm'),[Text.Encoding]::GetEncoding(949))
$out=New-Object 'Collections.Generic.List[string]'
$out.Add('# Servo Alarm FMS Codes')
$out.Add('')
$out.Add('Generated from FormServoAlarmList.dfm English memo lists. Native codes are hexadecimal. Alarm subcodes and axis numbers remain in the existing diagnostic screen; identical main codes are reported once until all affected axes recover.')
$out.Add('')
$out.Add('| Group | Native Code (Hex) | FMS ErrorNo (UInt32) | Existing Description |')
$out.Add('| --- | --- | --- | --- |')
foreach($group in 1..4){
 $match=[regex]::Match($text,('(?s)object Memo_En'+$group+':.*?Lines.Strings = \((.*?)\)\r?\n'))
 if(!$match.Success){throw 'Missing English alarm memo'}
 $rows=New-Object 'Collections.Generic.List[string]'
 $row=''
 # Parse Delphi string-list literals, including numeric characters and + continuations.
 foreach($line in ($match.Groups[1].Value -split '\r?\n')){
  if($line.Trim().Length -eq 0){continue}
  foreach($token in [regex]::Matches($line,"'(?:''|[^'])*'|#\d+")){
   if($token.Value.StartsWith('#')){$row += [char][int]$token.Value.Substring(1)}
   else{$row += $token.Value.Substring(1,$token.Value.Length-2).Replace("''", "'")}
  }
  if(!$line.TrimEnd().EndsWith('+')){$rows.Add($row.Trim());$row=''}
 }
 if($rows.Count%2 -ne 1){throw 'Unexpected memo layout'}
 $count=[int][math]::Floor($rows.Count/2)
 foreach($i in 0..($count-1)){
  $code=$rows[$i]
  $payload=$code
  if($group -eq 4){$payload=$payload.Substring(1)}
  $number='40256..40511'
  if($payload -match '^[0-9A-F]+$'){
   $raw=[Convert]::ToUInt32($payload,16)
   # Same compact encoding as NGSorterErrors::Encode: minimum 3 payload digits.
   $scale=1000L
   while($raw -ge $scale){$scale *= 10L}
   $number=([uint32]($group*10*$scale+$raw)).ToString()
  }elseif($group -ne 4 -or $code[1] -ne '1'){throw ('Unrecognized code: '+$code)}
  $code=$code.Replace([string][char]9633,'?')
  $description=$rows[$count+$i].Replace('|','/').Replace([string][char]8211,'-')
  $out.Add('| '+($group*10)+' | '+$code+' | '+$number+' | '+$description+' |')
 }
}
[IO.File]::WriteAllLines((Join-Path $root 'FMS_SERVO_ERROR_CODES.md'),$out,(New-Object Text.UTF8Encoding($false)))
Write-Output ('Exported '+($out.Count-6)+' native alarm definitions.')
