param([string]$InputDoc,[string]$Pdf,[switch]$Update)
$ErrorActionPreference='Stop'
$InputDoc=[IO.Path]::GetFullPath($InputDoc).Replace('/','\')
$Pdf=[IO.Path]::GetFullPath($Pdf).Replace('/','\')
$word=$null;$doc=$null;$defaultCheck=$null
function Invoke-Word($Action){
 for($i=0;$i -lt 90;$i++){
  try{return (& $Action)}catch{
   if($_.Exception.Message -notmatch '80010001|rejected|거부|busy|8001010A'){throw}
   Start-Sleep -Milliseconds 500
  }
 }
 throw 'Word remained busy for 45 seconds'
}
try {
 $word=New-Object -ComObject Word.Application
 Write-Output 'Word instance created'
 Invoke-Word {$word.Visible=$true;$word.DisplayAlerts=0}
 Write-Output 'Word visible'
 $defaultCheck=Invoke-Word {$word.Options.AlertIfNotDefault}
 Invoke-Word {$word.Options.AlertIfNotDefault=$false}
 Write-Output 'Default-app prompt disabled'
 Invoke-Word {$script:doc=$word.Documents.Open($InputDoc,$false,(!$Update.IsPresent),$false)}
 Write-Output 'Document opened'
 Write-Output "Open document count: $($word.Documents.Count)"
 if($null -eq $doc){$doc=$word.Documents.Item([IO.Path]::GetFileName($InputDoc))}
 Write-Output "Document null: $($null -eq $doc); Name: $($doc.Name); Fields null: $($null -eq $doc.Fields)"
 Write-Output "Active document: $($word.ActiveDocument.Name)"
 if($Update){
  [void](Invoke-Word {$doc.Fields.Update()})
  Invoke-Word {$doc.Repaginate()}
  Invoke-Word {$doc.Save()}
 }
 Invoke-Word {$doc.ExportAsFixedFormat($Pdf,17)}
 Write-Output "Pages: $(Invoke-Word {$doc.ComputeStatistics(2)}); PDF: $Pdf"
}catch { Write-Output ('RENDER ERROR: '+$_.Exception.Message);throw
}finally{
 if($doc){try{Invoke-Word {$doc.Close(0)}}catch{Write-Warning $_};[void][Runtime.InteropServices.Marshal]::ReleaseComObject($doc)}
 if($word -and $null -ne $defaultCheck){try{Invoke-Word {$word.Options.AlertIfNotDefault=$defaultCheck}}catch{Write-Warning $_}}
 if($word){try{Invoke-Word {$word.Quit()}}catch{Write-Warning $_};[void][Runtime.InteropServices.Marshal]::ReleaseComObject($word)}
}
