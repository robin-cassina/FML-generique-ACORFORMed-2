@echo off

rem Be sure you have set your VisualStudio environment (run vsvars32.bat) before compiling

rem Path to the vcvarsall.bat (in Microsoft Visual Studio)
rem  (!) don't use ""
set VCVAR_PATH=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC
rem set VCVAR_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build
echo.
echo [0] Set VisualStudio 32 bits environment
call "%VCVAR_PATH%\vcvarsall.bat" x86

echo.
echo [1] Compiling ...
CL /MP /c /O2 /Os /Gy /nologo /W3 /MD /EHa /GS- /TP /GR- /fp:precise /wd4996 /Fobarademo_CPP.obj ../barademo_CPP.cpp -I../..

echo.
echo [2] Linking ...
LINK /OUT:barademo_CPP.exe barademo_CPP.obj ..\..\..\bin\win32\libbaratinoo.lib /LTCG /OPT:REF /OPT:ICF /MANIFEST /nologo /machine:X86 /pdb:none Ws2_32.lib Iphlpapi.lib Setupapi.lib

echo.
echo [3] manifest ???
mt.exe -manifest barademo_CPP.exe.manifest -outputresource:barademo_CPP.exe

rem copy in bin\win32\ directory to get the right configuration path
echo.
echo [4] Move barademo_C.exe in bin\win32\ directory to get the right configuration path and DLL path
move barademo_CPP.exe ..\..\..\bin\win32\
rem copy ..\barademo_CPP.txt ..\..\..\bin\win32\texte.txt

echo.
echo Run it : it synthesizes the content of the text file texte.txt with first voice of configuration
echo and produces a wav audio file barademo_CPP.wav.
echo If an error occured, see the log file barademo_CPP.log.
echo.

