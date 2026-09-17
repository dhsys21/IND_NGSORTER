@echo off
call "C:\Program Files (x86)\Embarcadero\Studio\18.0\bin\rsvars.bat"
"C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe" NGSORTER.cbproj /t:Rebuild /p:Config=Debug /p:Platform=Win32 /p:__ShouldBuildVersion=true /p:IntermediateOutputDir=tmp\PeripheralFms20260917\build\obj /p:FinalOutputDir=tmp\PeripheralFms20260917\build\bin\ /v:minimal /nologo /fl /flp:logfile=tmp\PeripheralFms20260917\debug-build.log
exit /b %errorlevel%
