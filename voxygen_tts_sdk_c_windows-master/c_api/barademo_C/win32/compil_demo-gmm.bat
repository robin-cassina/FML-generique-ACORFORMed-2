@echo off

echo VisualStudio compilation

rem Path to the vcvarsall.bat (in Microsoft Visual Studio)
rem  (!) don't use ""
set VCVAR_PATH=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC
rem set VCVAR_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build
echo.
echo [0] Set VisualStudio 32 bits environment
call "%VCVAR_PATH%\vcvarsall.bat" x86

echo.
echo [1] Compiling ...
CL /MP /c /O2 /Os /Gy /nologo /W3 /MD /EHa /GS- /TP /GR- /fp:precise /wd4996 /Fobarademo_C.obj ../barademo_C.c -I../..

echo.
echo [2] Linking ...
LINK /OUT:barademo_C.exe barademo_C.obj ..\..\..\bin\win32\libbaratinoo.lib /OPT:REF /OPT:ICF /MANIFEST /nologo /machine:X86 /pdb:none

echo.
echo [3] manifest ???
mt.exe -manifest barademo_C.exe.manifest -outputresource:barademo_C.exe

echo.
echo [4] Move barademo_C.exe in bin\win32\ directory to get the right configuration path and DLL path
move barademo_C.exe ..\..\..\bin\win32\
rem copy ..\barademo_C.txt ..\..\..\bin\win32\texte.txt

echo.
echo Run it : it synthesizes the content of the text file texte.txt with first voice of configuration
echo and produces a wav audio file barademo_C.wav.
echo If an error occured, see the log file barademo_C.log.
echo.

