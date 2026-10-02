@echo off
call "C:\Program Files (x86)\Embarcadero\Studio\18.0\bin\rsvars.bat"
"%BDS%\bin\bcc32.exe" -DNDEBUG -C8 -tWV -tM -tU -I".";"%BDS%\include\windows\vcl";"%BDS%\include\windows\rtl";"%BDS%\include\windows\sdk";"%BDS%\include\windows\crtl";"%BDS%\include\dinkumware";"%BDS%\include";"C:\Users\gsinm\Documents\TMS Smooth Controls";"C:\Users\gsinm\Documents\TMS Smooth Controls\Delphi101Berlin\Win32\Release" -L"%BDS%\lib\win32\release";"%BDS%\lib\win32\release\psdk";"C:\Users\gsinm\Documents\TMS Smooth Controls\Delphi101Berlin\Win32\Release" -ntmp\TeachingReview20261002 -etest.exe tmp\TeachingReview20261002\test.cpp vcl.lib rtl.lib vclimg.lib TMSSmoothControlsPackPkgDXE10.lib
exit /b %errorlevel%
