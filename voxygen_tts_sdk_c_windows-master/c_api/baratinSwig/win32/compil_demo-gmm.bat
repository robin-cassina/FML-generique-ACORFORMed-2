@echo off

rem Set your JDK path
set JDK_VERSION=1.8.0_131
set JDK_PATH=%ProgramFiles(x86)%\Java\jdk%JDK_VERSION%
echo.
echo [0a] Check JDK Path: %JDK_PATH%
if not exist "%JDK_PATH%" (
	echo ERROR: JDK directory "%JDK_PATH%" didn't exist
	exit /B 0
) else (
	echo JDK directory is "%JDK_PATH%"
)

rem Be sure you have set your VisualStudio environment (run vsvars32.bat) before compiling
rem Path to the vcvarsall.bat (in Microsoft Visual Studio)
rem  (!) don't use ""
set VCVAR_PATH=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC
rem set VCVAR_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build
echo.
echo [0b] Set VisualStudio 32 bits environment
call "%VCVAR_PATH%\vcvarsall.bat" x86

rem TODO: call swig on baratinSwig.i

echo.
echo [1a] Compiling the DLL
rem Options:
rem  CL (https://msdn.microsoft.com/en-us/library/fwkeyyhe.aspx)	-->	g++ (https://gcc.gnu.org/onlinedocs/gcc-3.0/gcc_3.html)
rem    /D<name>[=<value>] = Preprocessor Definitions
rem      HERE we define SWIG_MAKE_DLL that change baratinSwig.h to export some methods as DLL
rem    /MP = Compiles multiple source files by using multiple processes	-->	? (not so usefull ?)
rem    /c = Compiles without linking	-->	-c 
rem    /O2 = Creates fast code	-->	-O2 (or -O3, see https://gcc.gnu.org/onlinedocs/gcc-3.0/gcc_3.html#SEC13)
rem    /Os = Favors small code.	-->	-Os ( /!\ Os => -O2, but disable -O3)
rem    /Gy = Enables function-level linking (http://stackoverflow.com/questions/629894/function-level-linking-gy-switch-in-vc-what-is-it-good-for)	-->	?
rem    /nologo = Suppresses display of sign-on banner	-->	(nothing)
rem    /W3 = warning level	-->	-W... (https://gcc.gnu.org/onlinedocs/gcc-3.0/gcc_3.html#SEC11)
rem    /MD = Creates a multithreaded DLL using MSVCRT.lib	-->	??? -mthreads, 
rem    /EHa = Exception Handling Model (a => catches both asynchronous (structured) and synchronous (C++) exceptions, "Structured Exception Handling (SEH)")	-->	??? (https://gcc.gnu.org/wiki/WindowsGCCImprovements)
rem    /GS- => disable Buffer Security Check (https://msdn.microsoft.com/en-us/library/8dbf701c.aspx)	-->	?
rem    /TP =  treat all files named on the command line as C++ source	-->	useless as files have cpp extension (-x c++)
rem    /fp:precise => Floating-Point Behavior "precise" (https://msdn.microsoft.com/en-us/library/e7s85ffb.aspx)	-->	? default or -frounding-math -fsignaling-nans	(https://gcc.gnu.org/wiki/FloatingPointMath)
rem    /wd4996	=> disables compiler warning C4996 ("The compiler encountered a deprecated declaration", https://msdn.microsoft.com/en-us/library/ttcz0bys.aspx)
rem    -I../..	=> include path	-->	-I../..
rem Removed options:
rem    /Fo<pathname> => object file name (or directory) => create baratinSwig.obj (useless)	--> -o barademo_CPP.o (useless)
rem    /GR- => disable Run-Time Type Information (code to check object types at run time) : required for dynamic_cast<>
if exist baratinSwig.obj del baratinSwig.obj
if exist baratinSwig_wrap.obj del baratinSwig_wrap.obj
CL /DSWIG_MAKE_DLL /MP /c /O2 /Os /Gy /nologo /W3 /MD /EHa /GS- /TP /fp:precise /wd4996 ../baratinSwig.cpp ../baratinSwig_wrap.cxx -I../.. -I"%JDK_PATH%\include" -I"%JDK_PATH%\include\win32"

echo.
echo [1b] Build the DLL (and lib)
rem  Options:
rem  LINK (https://msdn.microsoft.com/en-us/library/y0zzbyt4.aspx)	-->	g++ 
rem   /DLL = build a DLL	--> (~ -share)
rem   /LIBPATH:<path> = Path to search before the environmental library path	-->	...
rem     HERE the libbaratinoo.lib
rem   /OPT:REF => eliminates functions and data that are never referenced
rem   /OPT:ICF => perform identical COMDAT folding
rem   /nologo = Suppress Startup Banner
rem   /machine:x86 => Specify Target Platform
rem   /pbd:none	(? don't create Use Program Database, see https://msdn.microsoft.com/en-us/library/kwx19e36.aspx)
LINK /DLL baratinSwig.obj baratinSwig_wrap.obj /LIBPATH:..\..\..\bin\win32 libbaratinoo.lib /OPT:REF /OPT:ICF /nologo /machine:X86 /pdb:none

echo.
echo [1c] Copy baratinSwig.dll in ..\java-swig\Common\Lib\External\Voxygen\Win32\ directory like libbaratinoo.{dll,lib}
if not exist ..\java-swig\Common\Lib\External\Voxygen\Win32\NUL (
        echo "Create ..\java-swig\Common\Lib\External\Voxygen\Win32\, and copying libbaratinoo.dll"
        mkdir ..\java-swig\Common\Lib\External\Voxygen\Win32\
        COPY /Y ..\..\..\bin\win32\libbaratinoo.dll ..\java-swig\Common\Lib\External\Voxygen\Win32\
)
COPY /Y baratinSwig.dll ..\java-swig\Common\Lib\External\Voxygen\Win32\

echo.
echo [1d] Move baratinSwig.{dll,lib} in bin\win32\ directory like libbaratinoo.{dll,lib}
MOVE /Y baratinSwig.dll ..\..\..\bin\win32\
MOVE /Y baratinSwig.lib ..\..\..\bin\win32\

echo.
echo [2a] Compiling the executable
rem Options are the same as baratinSwig.cpp compilation
rem  EXCEPT /DSWIG_MAKE_DLL as this time baratinSwig.h is used to import methods from the DLL
CL /MP /c /O2 /Os /Gy /nologo /W3 /MD /EHa /GS- /TP /fp:precise /wd4996 /ForunBaratinSwig.obj ../runBaratinSwig.cpp -I../..

echo.
echo [2b] Linking the executable
rem  Options:
rem  LINK (https://msdn.microsoft.com/en-us/library/y0zzbyt4.aspx)	-->	g++ 
rem   /OUT:<filename> = output filename	--> -o 
rem   /LIBPATH:<path> = Path to search before the environmental library path	-->	...
rem     HERE baratinSwig.lib and libbaratinoo.lib
rem   /LTGC = Link-time Code Generation (https://msdn.microsoft.com/en-us/library/xbf3tbeh.aspx)
rem     NOTA: Throws a warning "/LTCG specified but no code generation required; remove /LTCG from the link command line to improve linker performance"
rem           We remove it.
rem   /OPT:REF => eliminates functions and data that are never referenced
rem   /OPT:ICF => perform identical COMDAT folding
rem   /MANIFEST = Create Side-by-Side Assembly Manifest (https://msdn.microsoft.com/en-us/library/f2c0w594.aspx)
rem   /nologo = Suppress Startup Banner
rem   /machine:x86 => Specify Target Platform
rem   /pbd:none	(? don't create Use Program Database, see https://msdn.microsoft.com/en-us/library/kwx19e36.aspx)
LINK /OUT:runBaratinSwig.exe runBaratinSwig.obj /LIBPATH:..\..\..\bin\win32 baratinSwig.lib libbaratinoo.lib /OPT:REF /OPT:ICF /MANIFEST /nologo /machine:X86 /pdb:none Ws2_32.lib Iphlpapi.lib Setupapi.lib

echo.
echo [2c] manifest ???
mt.exe /nologo -manifest runBaratinSwig.exe.manifest -outputresource:runBaratinSwig.exe

rem copy in bin\win32\ directory to get the right configuration path
echo.
echo [2d] Move runBaratinSwig.exe in bin\win32\ directory to get the right configuration path and DLL path
MOVE /Y runBaratinSwig.exe ..\..\..\bin\win32\

echo.
echo Run it : it synthesizes the content of the text file texte.txt with first voice of configuration
echo and produces a wav audio file runBaratinSwig.wav.
echo If an error occured, see the log file runBaratinSwig.log.
echo.

rem echo.
rem echo Exit
rem exit /B 0


