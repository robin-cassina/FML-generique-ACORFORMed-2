# Voxygen library integration in Java

Steps:
 1. SWIG wrapping code for the Voxygen library
 2. Compile Java code

## SWIG wrapping code

The SWIG wrapping code is in the current directory (baratinSwig/) :
 - baratinSwig.{h,cpp}: some complementary C++ code to facilitate baratinoo library integration
   nota: baratinoo library header and I/O code is define in parent directory (c_api/)
 - baratinSwig.i: the SWIG interface file for baratinSwig
 - callswig.bat: batch to (re)call SWIG to generate C++ and Java wrapping code
 + runBaratinSwig.cpp: a little C++ test application

1a) Generate SWIG wrapper code

```bat
callswig.bat
```

=> Generate :
   - baratinSwig_wrap.cxx: the C++ wrapping code
   - java-swig/generated-src/main/java/lpl/tts/voxygen/*.java: the JAVA wrapping code

Nota: require SWIG, here installed in C:\swigwin, see SWIG_EXE variable

1b) Compile the SWIG DLL

The compilation is specific to the architecture, for Windows 64-bit go to the 'win64/' directory:

```bat
cd win64
compil_demo-gmm.bat
```
=> Generate :
   - baratinSwig.obj and baratinSwig_wrap.obj: compilation of C++ code
   - baratinSwig.{dll,lib}: the SWIG DLL/library (linked to libbaratinoo.lib)
     nota: The DLL is copied into java-swig/Common\Lib/External/Voxygen/Win64/
           and the two files (DLL and lib) are moved into corresponding bin directory ('..\..\bin\win64')
   - baratinSwig.exp: extra generated file
   + runBaratinSwig.{obj,exe}: compilation of runBaratinSwig.cpp
     nota: The EXE is moved into the corresponding bin directory ('..\..\bin\win64')

Nota: require the VisualStudio compiling environement
      and a JDK (currently 1.8.0_92 in %ProgramFiles%\Java\jdk%JDK_VERSION%)

## Java code

2a) Compile the java code

The Java code is into 'java-swig/' directory.
A maven project is defined.

```bat
cd ..\java-swig
mvn package
```

=> Generate all in the target/ directory :
  - baratin-swig-1.0-SNAPSHOT.jar: the project JAR
  - VoxFMLToSpeech-1.0-SNAPSHOT.jar: a JAR with all dependencies to run a FML --> speech convertor
 
2b) Copy the Voxygen config files

The config files are in the 'config/' directory, up in the directory structure :

```bat
xcopy ..\..\..\config Common\Lib\External\Voxygen\config\ /s
``` 

You need also copy the activation file (V8.1_1748.lic) in Common\Lib\External\Voxygen\config\.

You can now test the FML --> speech conversion

```bat
java -jar target\VoxFMLToSpeech-1.0-SNAPSHOT.jar fmlFile*.xml
```

Output files are in the ```test-fml2speech/<YYMMDD-hhmm>/``` sub-directory:
  - *.wav/.raw* : Audio file (wav and *raw*, i.e. without header)
   nota: the wav header change every day, but the raw data are the same (allow MD5 sum comparison)
  - *.markers.json* : index of markers (in the speech section)
  - *.visemes.json* : index of visemes (in the speech section)
  - *.ssml* : the intermediary SSML file
In case of a FML contains various ```<speech>``` section, this files are generated for each section, with an index ```-<idx>``` to denote the section.
In all case, the files *.allmarkers.json* and *.allvisemes.json* list markers/visemes for the whole FML file. 
