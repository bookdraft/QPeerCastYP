call F:\Qt\4.4.3\bin\qtvars.bat
qmake
make -j2 && release\qpeercastyp.exe || pause
