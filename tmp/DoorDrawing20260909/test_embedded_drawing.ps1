param([string]$ExePath=(Join-Path $PSScriptRoot 'debug-bin/NGSORTER.exe'))
$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$exe=(Resolve-Path -LiteralPath $ExePath).Path
Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class DoorDrawingResource {
 [DllImport("kernel32.dll",CharSet=CharSet.Unicode,SetLastError=true)] public static extern IntPtr LoadLibraryEx(string path,IntPtr file,uint flags);
 [DllImport("kernel32.dll",CharSet=CharSet.Unicode,SetLastError=true)] public static extern IntPtr FindResource(IntPtr module,string name,IntPtr type);
 [DllImport("kernel32.dll")] public static extern uint SizeofResource(IntPtr module,IntPtr info);
 [DllImport("kernel32.dll")] public static extern IntPtr LoadResource(IntPtr module,IntPtr info);
 [DllImport("kernel32.dll")] public static extern IntPtr LockResource(IntPtr resource);
 [DllImport("kernel32.dll")] public static extern bool FreeLibrary(IntPtr module);
}
'@
# LOAD_LIBRARY_AS_DATAFILE: inspect PE resources only; never run the equipment EXE.
$module=[DoorDrawingResource]::LoadLibraryEx($exe,[IntPtr]::Zero,2)
if($module -eq [IntPtr]::Zero){throw 'Could not map EXE as data'}
try{
 $info=[DoorDrawingResource]::FindResource($module,'NGSORTER_DRAWING',[IntPtr]10)
 if($info -eq [IntPtr]::Zero){throw 'NGSORTER_DRAWING RCDATA not linked'}
 $size=[DoorDrawingResource]::SizeofResource($module,$info)
 $resource=[DoorDrawingResource]::LoadResource($module,$info)
 $pointer=[DoorDrawingResource]::LockResource($resource)
 $bytes=New-Object byte[] $size
 [Runtime.InteropServices.Marshal]::Copy($pointer,$bytes,0,$bytes.Length)
 $original=[IO.File]::ReadAllBytes((Join-Path $root 'Assets/NGSORTER.png'))
 if([Convert]::ToBase64String($bytes) -cne [Convert]::ToBase64String($original)){throw 'Embedded PNG differs from original asset'}
 Add-Type -AssemblyName System.Drawing
 $stream=New-Object IO.MemoryStream(,$bytes)
 try{
  $drawing=[Drawing.Image]::FromStream($stream)
  try{
   if($drawing.Width -ne 1394 -or $drawing.Height -ne 614){throw 'Unexpected drawing dimensions'}
   Write-Output "PASS: embedded original PNG decodes as 1394x614 ($size bytes); EXE not executed"
  }finally{$drawing.Dispose()}
 }finally{$stream.Dispose()}
}finally{[void][DoorDrawingResource]::FreeLibrary($module)}
if(Test-Path -LiteralPath (Join-Path (Split-Path $exe) 'NGSORTER.png')){throw 'Test build unexpectedly has a loose PNG'}
$dfm=[IO.File]::ReadAllText((Join-Path $root 'FormDoor.dfm'),[Text.Encoding]::GetEncoding(949))
if($dfm -notmatch '(?s)object imgMachineDrawing: TImage\s+Left = 136\s+Top = 221\s+Width = 1088\s+Height = 560\s+Stretch = True\s+end'){throw 'Designer drawing/overlay layout unexpectedly changed'}
Write-Output 'PASS: no PNG beside test EXE; existing 80%-width panel layout preserved without DFM Picture.Data'
