@echo off
call "C:\Program Files (x86)\Embarcadero\Studio\18.0\bin\rsvars.bat"
"%BDS%\bin\bcc32.exe" -DNDEBUG -tWV -tM -tU -I"%BDS%\include\windows\vcl";"%BDS%\include\windows\rtl";"%BDS%\include\windows\sdk";"%BDS%\include\windows\crtl";"%BDS%\include\dinkumware";"%BDS%\include" -L"%BDS%\lib\win32\release";"%BDS%\lib\win32\release\psdk" -ntmp\manual-complete-open -etest.exe tmp\manual-complete-open\test.cpp vcl.lib rtl.lib
exit /b %errorlevel%
