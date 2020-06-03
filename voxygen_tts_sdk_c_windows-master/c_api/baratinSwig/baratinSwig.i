/* vim: set tabstop=4 softtabstop=0 shiftwidth=4 noexpandtab filetype=cpp : */
%module baratinSwig

//-----
// Some define used in the included header
//----

/* List of used #defined:
 *  - baratinoo.h: 
 *   (1) The 'DllExport' modifier
 *    - WIN32 => compilation for Windows target /!\ not defined for Windows 64bit (neither by GCC, better use _WIN32, _WIN64 like in  baratinSwig.h)
 *    -  __GNUC__ => compilation with GCC >=4 (except Windows target)
 *    - MAKE_DLL => define DllExport to export method in the DLL (or import it from a DLL)
 *   (2) Android
 *    - __ANDROID__ => include jni.h and define libraries' init() methods with JNIEnv/context
 *   (3) C++/C API
 *    - BARATINOO_C_API => C API (default is C++ API)
 *    - __cplusplus => in case of C API with C++ code (to add extern "C" {})
 *  - baratinooio.h: 
 *    nota: baratinooio.h include baratinoo.h => (1), (2) and (3)
 *  - baratinSwig.h:
 *   (1) The 'SwigDllExport' modifier (as in baratinoo.h)
 *    - _WIN32 and _WIN64 => compilation for Windows target
 *    -  __GNUC__ => compilation with GCC >=4 (except Windows target)
 *    - SWIG_MAKE_DLL => define SwigDllExport to export method in the DLL (or import it from a DLL)
 */

// [A] Compilation target
// - on windows (32 or 64)
#ifndef NOT_WIN // Add -DNOT_WIN in the Swig CLI to desactivate them
#define WIN32
#define _WIN32
#define _WIN64
#endif  //ndef NOT_WIN

// [b] we make a DLL
//  - baratino(io).h
#define MAKE_DLL
//  - baratinSwing.h
#define SWIG_MAKE_DLL

//-----
// Code insertion, see http://www.swig.org/Doc3.0/SWIGDocumentation.html#SWIG_nn40
//----

//TODO?
//%rename(TimeStampVoxy) TimeStamp;
//%rename(TimeStampEventVoxy) TimeStampEvent;
//%rename(ContinuousEventVoxy) ContinuousEvent;

// - Everything in the %{ ... %} block is simply copied verbatim to the ('header' section of the) resulting wrapper file created by SWIG.
%{
#include "baratinSwig.h"
//#include <iostream>	// std::cerr
%}

//-----
// Usefull includes
//-----

// - Windows interface file
%include <windows.i>

// - Generate proper (Java) enum, see http://www.swig.org/Doc3.0/Java.html#Java_proper_enums 
%include <enums.swg>

// - STL vector & co.
//%include <stl.i>      // This include "all" STL, i.e. std::{string,vector,map,pair}
//TODO? %include <std_vector.i>       // include STL sdt::vector
//%include "std_vector.i" // include local version of std_vector, as the get() method didn't compil with Visual Studio (/!\ Java only ?) // useless as we only use std::vector<byte> to convert methods output into Java byte[]
//TODO? %template(TimeStampEvents) std::vector<TimeStampEvent>;
%include <std_string.i>       // include STL std::string

//-----
// Some typemaps
//-----

// (!) baratinSwig.h define the 'byte' type to distinct between (char *) pointing to a String and to a byte[]

// [A] Mapping the output of the SpeechResult.getRawData/Header/Sound() methods
//   The output is declared as std::vector<byte> and we want to convert it into a (Java) byte[]

// [A.1] Swig --> Java 
#if defined(SWIGJAVA)
// - Transform the C/C++ result of the method (variable $1, here a std::vector<byte>)
//     to the expected Java type (variable $result, here a byte[])
//  nota: macros JCALLx are explain in http://www.swig.org/Doc3.0/Java.html#Java_typemaps_for_c_and_cpp
//        JCALLx(MethodName, jenv, arg0, arg1, ...);    => [c++] jenv->MethodName(arg0, arg1, ...) / [c]  (*jenv)->MethodName(jenv, arg0, arg1, ...);
//        .  JCALL1(NewByteArray, jenv, sz);    => [c++] jenv->NewByteArray(sz) / [c]  (*jenv)->NewByteArray(jenv, sz);
%typemap(out) std::vector<byte> {
	// Get the size of the vector
	const jsize sz = (jsize) $1.size(); //nota: cast to 'jsize' as 'size_t' is sometime define as a longer integer (b.e. 64 bit)
	// Create a new Java byte[sz]
	$result = JCALL1(NewByteArray, jenv, sz);        //c++: jenv->NewByteArray(sz)
	// Set the sz values form vector's data into $result
	JCALL4(SetByteArrayRegion, jenv, $result, 0, sz, (const jbyte*) $1.data());       //c++: jenv->SetByteArrayRegion($result, 0, $1.size(), (const jbyte*) $1.data())
}

// - Transform Java byte[] into a C++ std::vector<byte> for input parameters
// nota: currently unused
%typemap(in) std::vector<byte> (std::vector<byte> bytes) { // bytes is a temporal variable in the wrapper code
	// Check case of null byte[]
	if (!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null array");
		return $null;
	}
	// Get the input (Java byte[]) size
	const jsize sz = JCALL1(GetArrayLength, jenv, $input);
	// Resize the std:vector<byte> bytes (temporal variable)
	bytes.resize(sz);
	// Get a accessor to the Java byte[] array
	jbyte* const jarr = JCALL2(GetByteArrayElements, jenv, $input, 0);
	if (!jarr) return $null;        // failed
	// Copy the input values into the temporal variable
	for ( jsize i = 0; i < sz; i++ )
		bytes[i] = jarr[i];
	// The variable pass (to the C/C++ function) is your temporary variable
	$1 = &bytes;
}

// - Other typemaps required for  std::vector<byte>
//  See http://www.swig.org/Doc3.0/Java.html#Java_typemaps_c_to_java_types
//  . JNI C type: (default) mapping of types from C/C++ to JNI for use in the JNI (C/C++) code
%typemap(jni) std::vector<byte> "jbyteArray"    // std::vector<byte> "become" jbyteArray in the JNI code 
//  . Java intermediary type: (default) mapping of types from C/C++ to Java for use in the native functions in the intermediary JNI class.
//    The type must be the equivalent Java type for the JNI C type specified in the "jni" typemap.
//    See https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
%typemap(jtype) std::vector<byte> "byte[]"      // JNI "jbyteArray" --> Java "byte[]"
//  . Java type: (default) mapping of types from C/C++ to Java for use in the Java module class, proxy classes and type wrapper classes.
%typemap(jstype) std::vector<byte> "byte[]"     // as many time, the same as jtype (we hope it's OK)
//  . Conversion from jstype to jtype (Java code)
//      for "$javainput" explication see http://www.swig.org/Doc3.0/Java.html#Java_special_variables
//   nota: currently unused
%typemap(javain) std::vector<byte> "$javainput" // the more classical (as jtype = jstype)
//  . Conversion from jtype to jstype (Java code)
//   nota: you can add comment to trace it in the generated Java source
%typemap(javaout) std::vector<byte> { return $jnicall; } // the more classical (as jstype = jtype)

// [A.x] Other cases => warning
#else
#warning no typemap(out) "std::vector<byte>" defined for this target language
#endif

// [B] Mapping the 'text' parameter of the textToSpeech() method
//   The 'text' parameter is declared as "const byte *", manytime convert to a (Java) String
//    but we need want to convert it into a (Java) byte[]
//    so the charset encoding conversion should be explicit

// [B.1] Swig --> Java (and more ?)
//%include <various.i>    // Lib/Java/various.i
//%apply char *BYTE {const byte * text};     // "const byte *text" converted into a "byte[]" in the target language (not a "String")

// Adaptation of char *BYTE in various.i to ensure the array is null-terminated
%typemap(jni) char *BYTE0 "jbyteArray"
%typemap(jtype) char *BYTE0 "byte[]"
%typemap(jstype) char *BYTE0 "byte[]"
%typemap(in) char *BYTE0 (char *buf, jint len) {
	buf = NULL; // init buffer variable
	// Get access to the array elements
	$1 = (char *) JCALL2(GetByteArrayElements, jenv, $input, 0); 
	// check if the byte[] is null-terminated
	len = JCALL1(GetArrayLength, jenv, $input);
	if ($1[len-1] != 0) { // non null-terminated byte[] => copy into buf
#ifdef __cplusplus
		buf = new char[len+1]; // just one byte longer
#else
		buf = (char *)malloc((len+1) * sizeof(char)) // just one byte longer;
#endif
		// Copy the input content into the buffer
		memcpy(buf, $1, len * sizeof(char));
		// Append a 0 to have a  null-terminated byte[]
		buf[len] = 0;
		// Release yet the array access (as we have a copy)
		JCALL3(ReleaseByteArrayElements, jenv, $input, (jbyte *) $1, 0);
		$1 = buf; // change $1 by buf
	}// else OK ($1 = GetByteArrayElements result)
}

%typemap(argout) char *BYTE0 {
	//nota: local variable 'buf' in the typemap(in) is renamed buf$argnum
	if (buf$argnum) { // non-NULL buffer => release it
#ifdef __cplusplus
		delete []buf$argnum;
#else
		free(buf$argnum);
#endif
	} else { // release the array access
		JCALL3(ReleaseByteArrayElements, jenv, $input, (jbyte *) $1, 0); 
	}
}

%typemap(javain) char *BYTE0 "$javainput"

// Prevent default freearg typemap from being used (SWIG generate one for String)
%typemap(freearg) char *BYTE0 ""

%apply char *BYTE0 {const byte * text};     // "const byte *text" converted into a null-terminated "byte[]" in the target language (not a "String")

// [C] Mapping the output of the SpeechResult.get*Events methods
//   The output is declared as std::vector<TimeStampEvent*> and we want to convert it into a (Java) TimeStampEvent[]
//                       or as std::vector<ContinuousEvent*> and we want to convert it into a (Java) ContinuousEvent[] (getVisemeEvents)
//   Conversion inspired from:
//     - [SWIG]Lib/java/arrays_java.i : ARRAYSOFCLASSES show how to manage object arrays
//     - https://cppcodetips.wordpress.com/2014/02/25/returning-array-of-user-defined-objects-in-jni/ : build a Object[]
//     - http://www.swig.org/Doc3.0/SWIGDocumentation.html#Java_adding_downcasts : downcast TimeStampEvent --> ContinuousEvent
//   (!) spaces are important "%typemap(...) std::vector< TimeStampEvent * >" works, "%typemap(...) std::vector<TimeStampEvent*>" doesn't

// [C.1] Swig --> Java 
#if defined(SWIGJAVA)

%extend TimeStamp {
	/** Get the finest classname of a TimeStamp. */
	static const char *downcastName(TimeStamp *ts) {
		// Deeper to upper
		// CountinuousEvent
		if (ContinuousEvent *event = dynamic_cast<ContinuousEvent *>(ts)) return "ContinuousEvent";
		// TimeStampEvent
		if (TimeStampEvent *event = dynamic_cast<TimeStampEvent *>(ts)) return "TimeStampEvent";
		// TimeStamp
		if (TimeStamp *event = dynamic_cast<TimeStamp *>(ts)) return "TimeStamp";
		// default
		return NULL;    // "$packagepath/$javaclassname"

	};

	/** JNI: Convert a TimeStamp to the finest corresponding java object. */
	static jobject jdowncast(JNIEnv *jenv, TimeStamp *ts, bool owner=false) {
		const char *downName = TimeStamp_downcastName(ts);
		if (downName==NULL) return NULL;//TODO: null return, exception
		// Build the Java (full) class name
		std::string jclassName("lpl/tts/voxygen/"); // $packagepath
		jclassName += downName;
		// Get the class
		jclass clazz = JCALL1(FindClass, jenv, jclassName.c_str());       // Get the java class
		if (!clazz) {return NULL;}; // (?) return value
		// Get the (jlong, boolean) constructor (generated by SWIG)
		jmethodID mid = jenv->GetMethodID(clazz, "<init>", "(JZ)V"); 
		if (!mid) {return NULL;}; // (?) return value
		// Create the new Java object
		jobject obj = JCALL4(NewObject, jenv, clazz, mid, ts, owner);
		return obj;
	};

	/** TMP: test the jdowncast. */
	static std::vector<TimeStampEvent *> someTimeEvents() {
		std::vector<TimeStampEvent *> res;

		res.push_back(new TimeStampEvent((float) 0.0, 0, 0, BARATINOO_EVENT_TYPE::BARATINOO_MARKER_EVENT, "m1"));
		res.push_back(new ContinuousEvent((float) 1.2, 0, 0, BARATINOO_EVENT_TYPE::BARATINOO_PHONEME_EVENT, "m2", (float) 1.1));
		res.push_back(new TimeStampEvent((float) 3.4, 0, 0, BARATINOO_EVENT_TYPE::BARATINOO_MARKER_EVENT, "m3"));
		res.push_back(new ContinuousEvent((float) 5.6, 0, 0, BARATINOO_EVENT_TYPE::BARATINOO_PHONEME_EVENT, "m4", (float) 2.1));
		res.push_back(new TimeStampEvent((float) 7.8, 0, 0, BARATINOO_EVENT_TYPE::BARATINOO_MARKER_EVENT, "m5"));
		res.push_back(new ContinuousEvent((float) 9.0, 0, 0, BARATINOO_EVENT_TYPE::BARATINOO_PHONEME_EVENT, "m6", (float) 3.1));

		return res;
	}

}

// - Transform the C/C++ result of the method (variable $1, here a std::vector<TimeStampEvent *>)
//     to the expected Java type (variable $result, here a TimeStampEvent[])
//  nota: macros JCALLx are explain in http://www.swig.org/Doc3.0/Java.html#Java_typemaps_for_c_and_cpp
//        JCALLx(MethodName, jenv, arg0, arg1, ...);    => [c++] jenv->MethodName(arg0, arg1, ...) / [c]  (*jenv)->MethodName(jenv, arg0, arg1, ...);
//        .  JCALL1(NewByteArray, jenv, sz);    => [c++] jenv->NewByteArray(sz) / [c]  (*jenv)->NewByteArray(jenv, sz);
%typemap(out) std::vector< TimeStampEvent * > {
	// Get the size of the vector
	const jsize sz = (jsize) $1.size(); //nota: cast to 'jsize' as 'size_t' is sometime define as a longer integer (b.e. 64 bit)
	// Get the TimeStampEvent class (for the NewObjectArray)
	jclass clazz = JCALL1(FindClass, jenv, "lpl/tts/voxygen/TimeStampEvent");       // ? $javaclassname
	if (!clazz) {return $null;}
	// Create a new Java TimeStampEvent[sz]
	$result = JCALL3(NewObjectArray, jenv, sz, clazz, NULL);
	if (!$result) {return $null;}
	// Set the sz values form vector's data into $result
	for (int i=0; i<sz; ++i) {
		jlong cptr = 0;
		*(TimeStampEvent **) &cptr = $1.at(i);
		//jobject obj = JCALL4(NewObject, jenv, clazz, mid, cptr, false); // (?) change to true (or owner)
		jobject obj = TimeStamp_jdowncast__SWIG_0(jenv, (TimeStamp*) cptr, false);  // create the java object
		JCALL3(SetObjectArrayElement, jenv, $result, i, obj);
	}
}
// idem for std::vector< ContinuousEvent * >
%typemap(out) std::vector< ContinuousEvent * > {
	// Get the size of the vector
	const jsize sz = (jsize) $1.size(); //nota: cast to 'jsize' as 'size_t' is sometime define as a longer integer (b.e. 64 bit)
	// Get the ContinuousEvent class (for the NewObjectArray)
	jclass clazz = JCALL1(FindClass, jenv, "lpl/tts/voxygen/ContinuousEvent");       // ? $javaclassname
	if (!clazz) {return $null;}
	// Create a new Java ContinuousEvent[sz]
	$result = JCALL3(NewObjectArray, jenv, sz, clazz, NULL);
	if (!$result) {return $null;}
	// Set the sz values form vector's data into $result
	for (int i=0; i<sz; ++i) {
		jlong cptr = 0;
		*(ContinuousEvent **) &cptr = $1.at(i);
		//jobject obj = JCALL4(NewObject, jenv, clazz, mid, cptr, false); // (?) change to true (or owner)
		jobject obj = TimeStamp_jdowncast__SWIG_0(jenv, (TimeStamp*) cptr, false);  // create the java object
		JCALL3(SetObjectArrayElement, jenv, $result, i, obj);
	}
}

// - Other typemaps required for  std::vector<TimeStampEvent *> / std::vector<ContinuousEvent *> 
//  See http://www.swig.org/Doc3.0/Java.html#Java_typemaps_c_to_java_types
//  . JNI C type: (default) mapping of types from C/C++ to JNI for use in the JNI (C/C++) code
%typemap(jni) std::vector< TimeStampEvent * > "jobjectArray"    // std::vector<TimeStampEvent *> "become" a jobjectArray in the JNI code
%typemap(jni) std::vector< ContinuousEvent * > "jobjectArray"    // std::vector<ContinuousEvent *> "become" a jobjectArray in the JNI code

//  . Java intermediary type: (default) mapping of types from C/C++ to Java for use in the native functions in the intermediary JNI class.
//    The type must be the equivalent Java type for the JNI C type specified in the "jni" typemap.
//    See https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
%typemap(jtype) std::vector< TimeStampEvent * > "TimeStampEvent[]"      // JNI "jobjectArray" --> Java "TimeStampEvent[]"
%typemap(jtype) std::vector< ContinuousEvent * > "ContinuousEvent[]"      // JNI "jobjectArray" --> Java "TimeStampEvent[]"

//  . Java type: (default) mapping of types from C/C++ to Java for use in the Java module class, proxy classes and type wrapper classes.
%typemap(jstype) std::vector< TimeStampEvent * > "TimeStampEvent[]"     //  (like ARRAYSOFCLASSES)
%typemap(jstype) std::vector< ContinuousEvent * > "ContinuousEvent[]"     //  (like ARRAYSOFCLASSES)

//  . Conversion from jtype to jstype (Java code)
//   nota: you can add comment to trace it in the generated Java source
%typemap(javaout) std::vector< TimeStampEvent * > { return $jnicall; } // the more classical (as jstype = jtype)
%typemap(javaout) std::vector< ContinuousEvent * > { return $jnicall; } // the more classical (as jstype = jtype)

// [A.x] Other cases => warning
#else
#warning no typemap(out) "std::vector< TimeStampEvent * >" defined for this target language
#endif
//------------ baratinoo.h/baratinoio.h ----------//

// Import (some) libbaratino.dll header file(s)

// - baratinoo.h for the classes BaratinooTrace, BaratinooKey (inherited by BaratinooSwig)

// Ignore all class/struct and function (=> import enum)
%rename($ignore, %$isclass) "";    // ignore all class
%rename($ignore, %$isfunction) "";    // ignore all function (and class method)
//  + ignore some enums
%rename($ignore) BARATINOO_INIT_RETURN;    // BARATINOO_INIT_RETURN enum
//  + unhide some struct
//TODO?   %rename("%s") BaratinooVoiceInfo;
//  + rename enums and their items
%rename("%(strip:[BARATINOO_])s", %$isenum) "";        // strip BARATINOO_ prefix in enum name
%rename("%(strip:[BARATINOO_])s", %$isenumitem) "";        // strip BARATINOO_ prefix in enem's item
//%rename("PARSING") "BARATINOO_PARSING";        // BARATINOO_PARSING --> PARSING
%rename("%(regex:/.*_(.*)_.*/\\1/)s", regexmatch$name="BARATINOO_.*_PARSING") "";        // ... and its items
//%rename("TEXT_ENCODING") BARATINOO_TEXT_ENCODING;
//%rename("%s") BARATINOO_SIGNAL_CODING;
//%rename("%s") BARATINOO_EVENT_TYPE;

%include "baratinoo.h"

%rename("%s") "";       // reactivate all

// - baratinooio.h for some I/O classes (?)
//  include nothing, but define OutputSignalBuffer::HEADER_FORMAT
%rename("%(rstrip:[_HEADER])s", regextarget=1) "_HEADER$";        // rename HEADER items' names
namespace OutputSignalBuffer {
	enum HEADER_FORMAT {
		WAV_HEADER,
		AU_HEADER
	};
}
%rename("%s") "";       // desactivate all rename


//------------ baratinSwing.h ----------//

// ignore some elements
%ignore BaratinooTraceHelper;   // the BaratinooTraceHelper
%ignore SpeechResult::SpeechResult;    // SpeechResult constructors
%rename($ignore, regextarget=1) "^trace";        // hide trace methods
%rename($ignore, fullname=1) TimeStamp::TimeStamp(const BaratinooEvent &be);        // hide TimeStamp(Event)/ContinuousEvent constructor with BaratinooEvent
%rename($ignore, fullname=1) TimeStampEvent::TimeStampEvent(const BaratinooEvent &be);        // hide TimeStamp(Event)/ContinuousEvent constructor with BaratinooEvent
%rename($ignore, fullname=1) ContinuousEvent::ContinuousEvent(const BaratinooEvent &be);        // hide TimeStamp(Event)/ContinuousEvent constructor with BaratinooEvent

// Method that create new objects/allocate memory
%newobject BaratinooSwig::textToSpeech; // create the SpeechResult
%newobject SpeechResult::getHeader; // create byte[] for the header
%newobject SpeechResult::getSound; // create byte[] with header + raw data

// 
%typemap(javainterfaces) SpeechResult "lpl.tts.SpeechData"
%typemap(javainterfaces) TimeStamp "lpl.tools.TimeStampIfce"
%typemap(javainterfaces) TimeStampEvent "lpl.tools.TimeStampEventIfce"
%typemap(javainterfaces) ContinuousEvent "lpl.tools.ContinuousEventIfce"

// Parse the baratinSwig header file
%include "baratinSwig.h"
