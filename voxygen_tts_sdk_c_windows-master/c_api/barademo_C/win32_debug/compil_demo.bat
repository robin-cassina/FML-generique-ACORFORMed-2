@echo off

echo VisualStudio compilation
echo.
echo Be sure you have set your VisualStudio 32 bits environment before compiling (run vsvars32.bat)
echo.

CL /MP /c /Od /Zi /nologo /W3 /MDd /EHa /GS- /TP /GR  /RTC1 /fp:precise /wd4996 /Fobarademo_C.obj ../barademo_C.c -I../..

LINK /OUT:barademo_C.exe barademo_C.obj ..\..\..\bin\win32_debug\libbaratinoo.lib /DEBUG /MANIFEST /nologo /machine:X86 /pdb:none

mt.exe -manifest barademo_C.exe.manifest -outputresource:barademo_C.exe

echo Move barademo_C.exe in bin\win32_debug\ directory to get the right configuration path and DLL path
move barademo_C.exe ..\..\..\bin\win32_debug\
copy ..\barademo_C.txt ..\..\..\bin\win32_debug\texte.txt

echo.
echo Run it : it synthesizes the content of the text file texte.txt with first voice of configuration
echo and produces a wav audio file barademo_C.wav.
echo If an error occured, see the log file barademo_C.log.
echo.

