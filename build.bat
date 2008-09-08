set QTDIR=C:\Qt\4.4.0
set PATH=C:\Qt\4.4.0\bin
set PATH=%PATH%;C:\MinGW\bin
set PATH=%PATH%;%SystemRoot%\System32
set QMAKESPEC=win32-g++

qmake
make && release\qpeercastyp.exe || pause
