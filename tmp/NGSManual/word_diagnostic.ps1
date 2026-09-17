Add-Type @'
using System;
using System.Text;
using System.Runtime.InteropServices;
public class WordWindowDiagnostic {
 public delegate bool Callback(IntPtr h,IntPtr p);
 [DllImport("user32.dll")] public static extern bool EnumWindows(Callback c,IntPtr p);
 [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr h,Callback c,IntPtr p);
 [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h,out uint p);
 [DllImport("user32.dll",CharSet=CharSet.Unicode)] public static extern int GetWindowText(IntPtr h,StringBuilder s,int n);
}
'@
$ids=@(27904,21468,16900,31532,12080)
$cb=[WordWindowDiagnostic+Callback]{param($h,$p)
 [uint32]$id=0;[void][WordWindowDiagnostic]::GetWindowThreadProcessId($h,[ref]$id)
 if($id -in $ids){
  $s=[Text.StringBuilder]::new(1024);[void][WordWindowDiagnostic]::GetWindowText($h,$s,1024)
  if($s.Length){Write-Host "$id : $s"}
  $child=[WordWindowDiagnostic+Callback]{param($c,$p)
   $s=[Text.StringBuilder]::new(1024);[void][WordWindowDiagnostic]::GetWindowText($c,$s,1024)
   if($s.Length){Write-Host "  $s"};return $true
  }
  [void][WordWindowDiagnostic]::EnumChildWindows($h,$child,[IntPtr]::Zero)
 }
 return $true
}
[void][WordWindowDiagnostic]::EnumWindows($cb,[IntPtr]::Zero)
