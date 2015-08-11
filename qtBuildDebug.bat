# Do not use right now. Does not work that well
C:\Qt\Qt5.5\5.5\msvc2013_64\bin\qtenv2.bat

"C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" x86_amd64

qmake.exe E:\Dropbox\CDI_PlayGo-Final\CDI_PlayGo.pro -r -spec win32-msvc2013 "CONFIG+=debug"

cd E:\Dropbox\CDI_PlayGo-Final\CDI_PlayGo.pro

jom.exe