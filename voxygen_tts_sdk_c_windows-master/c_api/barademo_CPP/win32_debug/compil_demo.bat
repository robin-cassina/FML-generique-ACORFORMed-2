@echo off

echo VisualStudio compilation
echo.
echo Be sure you have set your VisualStudio 32 bits environment before compiling (run vsvars32.bat)
echo.

CL /MP /c /Od /Zi /nologo /W3 /MDd /EHa /GS- /TP /GR  /RTC1 /fp:precise /wd4996 /Fobarademo_CPP.obj ../barademo_CPP.cpp -I../..

LINK /OUT:barademo_CPP.exe barademo_CPP.obj ..\..\..\bin\win32_debug\libbaratinoo.lib /DEBUG /MANIFEST /nologo /machine:X86 /pdb:none Ws2_32.lib Iphlpapi.lib Setupapi.lib

mt.exe -manifest barademo_CPP.exe.manifest -outputresource:barademo_CPP.exe

echo Move barademo_CPP.exe in bin\win32_debug\ directory to get the right configuration path and DLL path
move barademo_CPP.exe ..\..\..\bin\win32_debug\
copy ..\barademo_CPP.txt ..\..\..\bin\win32_debug\texte.txt

echo.
echo Run it : it synthesizes the content of the text file texte.txt with first voice of configuration
echo and produces a wav audio file barademo_CPP.wav.
echo If an error occured, see the log file barademo_CPP.log.
echo.

