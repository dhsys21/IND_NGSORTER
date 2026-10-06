@echo off
call "C:\Program Files (x86)\Embarcadero\Studio\18.0\bin\rsvars.bat"
if not exist tmp\recovery-preview mkdir tmp\recovery-preview
"%BDS%\bin\bcc32.exe" -DNDEBUG -tWV -tM -tU -I".";"%BDS%\include\windows\vcl";"%BDS%\include\windows\rtl";"%BDS%\include\windows\sdk";"%BDS%\include\windows\crtl";"%BDS%\include\dinkumware";"%BDS%\include" -L"%BDS%\lib\win32\release";"%BDS%\lib\win32\release\psdk" -ntmp\recovery-preview -eRecoveryPreview_test.exe Tests\RecoveryPreview_test.cpp vcl.lib rtl.lib vclimg.lib
exit /b %errorlevel%
