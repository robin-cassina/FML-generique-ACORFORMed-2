@echo on

REM Set the SWIG_EXE
set SWIG_EXE=D:\Installations\swigwin-4.0.1[PORTABLE]\swig.exe

if exist java-swig\generated-src\main\java\lpl\tts\voxygen\NUL (
        rem Delete old generated files
        echo Remove java files in java-swig\generated-src\main\java\lpl\tts\voxygen
        del java-swig\generated-src\main\java\lpl\tts\voxygen\*.java 
) else (
        rem Create java-swig\generated-src\main\java\lpl\tts\voxygen
        echo Create generated java files directory: java-swig\generated-src\main\java\lpl\tts\voxygen
        mkdir java-swig\generated-src\main\java\lpl\tts\voxygen
)
rem Run Swig
echo Run SWIG for Java on baratinSwig.i =^> baratinSwig_wrap.cxx
rem (add -debug-tmsearch to debug typemap)
%SWIG_EXE% -c++ -I.. -java -package lpl.tts.voxygen -outdir java-swig\generated-src\main\java\lpl\tts\voxygen baratinSwig.i 

pause