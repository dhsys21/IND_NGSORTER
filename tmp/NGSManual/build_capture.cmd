@echo off
call "C:\Program Files (x86)\Embarcadero\Studio\18.0\bin\rsvars.bat"
"C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe" NGSORTER.cbproj /t:Build /p:Config=Debug /p:Platform=Win32 /p:IntermediateOutputDir=tmp\NGSManual\capture-build\obj /p:FinalOutputDir=tmp\NGSManual\capture-build\bin /v:minimal /nologo /fl /flp:logfile=tmp\NGSManual\capture-build.log
exit /b %errorlevel%
