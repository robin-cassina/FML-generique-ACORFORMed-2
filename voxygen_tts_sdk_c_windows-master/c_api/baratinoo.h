/* $Id: baratinoo.h 28126 2017-02-10 14:12:13Z paul $ -*- coding: utf-8-unix -*- */

/* (C) Copyright Voxygen S.A.S 2012
 *
 * BARATINOO TTS system.
 * Public interface for BARATINOO TTS engine.
 */

#ifndef _BARATINOO_H
#include <stdarg.h>
#include <stddef.h>

#ifdef __ANDROID__
#include <jni.h>
#endif

#if defined(WIN32)
  #if defined (MAKE_A_DLL)
    #define DllExport __declspec( dllexport )
  #else
    #define DllExport
  #endif
#else
  #if defined (MAKE_A_DLL) && __GNUC__ >= 4
    #define DllExport __attribute__ ((visibility("default")))
  #else
    #define DllExport
  #endif
#endif
#endif /* _BARATINOO_H */

/* ============================================================================== */
/*  Library initialization                                                        */
/*   BARATINOO library must be initialized once before creating any               */
/*   BaratinooEngine objects                                                      */
/*    -through a call to BaratinooInit()                                          */
/*    -argument is a pointer on a BaratinooTrace object (or NULL)                 */
/*    -or through a call to BaratinooInitWithKey()                                */
/*    -additional argument is a pointer on a BaratinooKey object (or NULL)        */
/*   When all BaratinooEngine are deleted                                         */
/*    -call to BaratinooTerminate() to clear memory                               */
/*                                                                                */
/*  BaratinooTrace is a pure virtual class. An implementation must have a write() */
/*  member function which is called by the BARATINOO library and its              */
/*  BaratinooEngine objects                                                       */
/*    -level: 5 categories of trace                                               */
/*    -engineNumber: 0 when trace is issued from library                          */
/*    -source: part of engine which generated the trace                           */
/*    -privatedata: void pointer if passed to engine constructor (otherwise NULL) */
/*    -format + args: formated print, same syntax as printf function              */
/*                                                                                */
/*  BaratinooKey is a pure virtual class that may be implemented. It must have    */
/*  get() member function which is called by the BARATINOO library and its        */
/*  BaratinooEngine objects                                                       */
/*    -index: request the 'index+1'th key (>=0)                                   */
/* ============================================================================== */
#ifndef BARATINOO_C_API
#ifndef _BARATINOO_CPP_H
class BaratinooTrace
{
  public:
    enum TraceLevel {
        TRACE_ERROR,
        TRACE_INIT,
        TRACE_WARNING,
        TRACE_INFO,
        TRACE_DEBUG
    };
    DllExport virtual void write(TraceLevel level, int engineNumber, const char *source, const char *format, va_list args) = 0;
    DllExport virtual void write(TraceLevel level, int engineNumber, const char *source, const void *privatedata, const char *format, va_list args) {
      write(level, engineNumber, source, format, args);
      (void)privatedata; /* unused variable */
    }
    DllExport virtual ~BaratinooTrace() {};
};
class BaratinooKey
{
public:
    DllExport virtual const char *get(int index) const = 0;
    DllExport virtual ~BaratinooKey() {};
};
#endif
#else /* BARATINOO_C_API */
#ifndef _BARATINOO_C_H
typedef enum {
    BARATINOO_TRACE_ERROR,
    BARATINOO_TRACE_INIT,
    BARATINOO_TRACE_WARNING,
    BARATINOO_TRACE_INFO,
    BARATINOO_TRACE_DEBUG
} BaratinooTraceLevel;
typedef void (*BaratinooTraceCB)(BaratinooTraceLevel level, int engineNumber, const char *source, const void *privatedata, const char *format, va_list args);
typedef const char * (*BaratinooKeyCB)(int index);
#endif
#endif

#ifndef _BARATINOO_H
typedef enum
{
    BARATINOO_INIT_OK,
    BARATINOO_INIT_ERROR
} BARATINOO_INIT_RETURN;
typedef struct
{
    int major;
    int minor;
    const char *patch;
    const char *level; /* "final" or "candidate" or "extended" */
    const char *svnversion;
} BaratinooVersionStruct;
#define BARATINOO_VERSION_SIZE 128
#endif

#ifndef BARATINOO_C_API
#ifndef _BARATINOO_CPP_H
#ifdef __ANDROID__
DllExport BARATINOO_INIT_RETURN baratinooInit(BaratinooTrace *trace, JNIEnv* env, jobject context);
DllExport BARATINOO_INIT_RETURN baratinooInitWithKey(BaratinooTrace *trace, BaratinooKey *key, JNIEnv* env, jobject context);
#else
DllExport BARATINOO_INIT_RETURN baratinooInit(BaratinooTrace *trace);
DllExport BARATINOO_INIT_RETURN baratinooInitWithKey(BaratinooTrace *trace, BaratinooKey *key);
#endif
DllExport void baratinooTerminate();
DllExport const char *getBaratinooVersion(); /* <major>.<minor><patch>-final or <major>.<minor><patch>-<level>:<svnversion> */
DllExport const BaratinooVersionStruct *getBaratinooVersionStruct();
DllExport size_t baratinooGetUIDString(size_t uidBufferSize, char *uidBuffer);
DllExport size_t baratinooGetTodayString(size_t timeBufferSize, char *timeBuffer);
DllExport size_t baratinooPercentEncodeString(const char *inBuffer, size_t outBufferSize, char *outBuffer);
#endif
#else /* BARATINOO_C_API */
#ifndef _BARATINOO_C_H
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __ANDROID__
    DllExport BARATINOO_INIT_RETURN BCinitlib(BaratinooTraceCB traceCB, JNIEnv* env, jobject context);
    DllExport BARATINOO_INIT_RETURN BCinitlibWithKey(BaratinooTraceCB traceCB, BaratinooKeyCB keyCB, JNIEnv* env, jobject context);
#else
    DllExport BARATINOO_INIT_RETURN BCinitlib(BaratinooTraceCB traceCB);
    DllExport BARATINOO_INIT_RETURN BCinitlibWithKey(BaratinooTraceCB traceCB, BaratinooKeyCB keyCB);
#endif
    DllExport void BCterminatelib(void);
    DllExport const char *BCgetBaratinooVersion(void); /* <major>.<minor><patch>-final or <major>.<minor><patch>-<level>:<svnversion> */
    DllExport const BaratinooVersionStruct *BCgetBaratinooVersionStruct(void);
    DllExport size_t BCgetUIDString(size_t uidBufferSize, char *uidBuffer);
    DllExport size_t BCgetTodayString(size_t timeBufferSize, char *timeBuffer);
    DllExport size_t BCpercentEncodeString(const char *inBuffer, size_t outBufferSize, char *outBuffer);
#ifdef __cplusplus
}
#endif
#endif
#endif


/* ============================================================================== */
/*  BaratinooEngine objects take their input from an input object. InputText is   */
/*  a pure virtual class. An implementation of this class must have a readText()  */
/*  member function which is called when the BaratinooEngine needs data. This     */
/*  function should copy the next 'length' bytes of text at 'address' and return  */
/*  the number of bytes actually copied.                                          */
/*    -'_parsing' member tells BaratinooEngine what kind of parsing is expected   */
/*    -'_encoding' member tells BaratinooEngine how text is encoded               */
/*   BaratinooEngine object is told which input to use through a call to          */
/*   BaratinooEngine::setInput().                                                 */
/*                                                                                */
/*  InputBinary class is reserved for internal use                                */
/* ============================================================================== */
#ifndef _BARATINOO_H
typedef enum {
    BARATINOO_ALL_PARSING,         /* Proprietary parsing and XML parsing both activated */
    BARATINOO_NO_PARSING,          /* No parsing at all */
    BARATINOO_PROPRIETARY_PARSING, /* Proprietary parsing only */
    BARATINOO_XML_PARSING          /* XML parsing only */
} BARATINOO_PARSING;

typedef enum {
    BARATINOO_DEFAULT_ENCODING,  /*  language dependant or XML defined */
    BARATINOO_CP437,             /*  MS code page 437 / DOS */
    BARATINOO_CP850,             /*  MS code page 850 */
    BARATINOO_CP1250,            /*  MS code page 1250 */
    BARATINOO_CP1252,            /*  MS code page 1252 */
    BARATINOO_CP1256,            /*  MS code page 1256 */
    BARATINOO_ISO_8859_1,        /*  ISO-8859-1 / latin-1 */
    BARATINOO_ISO_8859_2,        /*  ISO-8859-2 / latin-2 */
    BARATINOO_ISO_8859_6,        /*  ISO-8859-6 / arabic */
    BARATINOO_ISO_8859_15,       /*  ISO-8859-15 / latin-9 */
    BARATINOO_ISO_8859_16,       /*  ISO-8859-16 / latin-10 */
    BARATINOO_UTF8               /*  UTF8 (last in list) */
} BARATINOO_TEXT_ENCODING;
#endif

#ifndef BARATINOO_C_API
#ifndef _BARATINOO_CPP_H
class InputText
{
  protected:
    BARATINOO_PARSING _parsing;
    BARATINOO_TEXT_ENCODING _encoding;
    int _voiceIndex;
    const char *_voiceModules;
    const char *_uri;

  private:
    InputText(const InputText& inputText); /* private, not supported or implemented */
    InputText& operator= (const InputText& inputText); /* private, not supported or implemented */

  public:
    DllExport InputText(BARATINOO_PARSING parsing=BARATINOO_ALL_PARSING, BARATINOO_TEXT_ENCODING encoding=BARATINOO_DEFAULT_ENCODING, int voiceIndex=0, const char *voiceModules=0, const char *uri=0):_parsing(parsing),_encoding(encoding),_voiceIndex(voiceIndex),_voiceModules(voiceModules),_uri(uri) {}
    DllExport virtual ~InputText() {};
    DllExport virtual int readText(void *address, int length) = 0;
    DllExport BARATINOO_PARSING getParsing() const {return _parsing;}
    DllExport BARATINOO_TEXT_ENCODING getTextEncoding() const {return _encoding;}
    DllExport int getVoiceIndex() const {return _voiceIndex;}
    DllExport const char *getVoiceModules() const {return _voiceModules;}
    DllExport const char *getUri() const {return _uri;}
};


class InputBinary
{
  public:
    DllExport virtual ~InputBinary(){};
    DllExport virtual int readBinary(void *address, int length) = 0;
};
#endif
#else /* BARATINOO_C_API */
#ifndef _BARATINOO_C_H
typedef int (*BaratinooInputTextCB)(void *privateData, void *address, int length);
typedef int (*BaratinooInputBinaryCB)(void *privateData, void *address, int length);
#endif
#endif

/* ============================================================================== */
/*  BaratinooEngine objects produce their output signal in an output object.      */
/*  OutputSignal is a pure virtual class. An implementation of this class must    */
/*  have a writeSignal() member function which is called when the BaratinooEngine */
/*  has data to give. This function should deal with the next 'length' bytes of   */
/*  signal at 'address'. If the return value of writeSignal() is not 0, the       */
/*  processing loop stops and BaratinooEngine enters RUNNING state (see below)    */
/*    -'_coding' member tells BaratinooEngine what kind of signal coding is       */
/*     expected                                                                   */
/*    -'_frequency' member tells BaratinooEngine which signal frequency in Hertz  */
/*     is expected (in [6000 ; 48000])                                            */
/*   BaratinooEngine object is told which output to use through a call to         */
/*   BaratinooEngine::setOutput().                                                */
/*                                                                                */
/*  OutputBinary class is reserved for internal use                               */
/* ============================================================================== */
#ifndef _BARATINOO_H
typedef enum
{
    BARATINOO_PCM,
    BARATINOO_A_LAW,
    BARATINOO_U_LAW,
    BARATINOO_PCM_SWAP,
    BARATINOO_PCM_LE,
    BARATINOO_PCM_BE,
    BARATINOO_VOX
} BARATINOO_SIGNAL_CODING;

#define BARATINOO_OUTPUT_FREQUENCY 24000
#endif

#ifndef BARATINOO_C_API
#ifndef _BARATINOO_CPP_H
class OutputSignal
{
  protected:
    BARATINOO_SIGNAL_CODING _coding;
    int _frequency;
  public:
    DllExport OutputSignal(BARATINOO_SIGNAL_CODING coding=BARATINOO_PCM, int frequency=BARATINOO_OUTPUT_FREQUENCY):_coding(coding),_frequency(frequency) {}
    DllExport virtual ~OutputSignal() {};
    DllExport virtual int writeSignal(const void *address, int length) = 0;
    DllExport BARATINOO_SIGNAL_CODING getCoding() const {return _coding;}
    DllExport int getFrequency() const {return  _frequency;}
    DllExport void setCoding(BARATINOO_SIGNAL_CODING coding) {_coding = coding;}
    DllExport void setFrequency(int frequency) {_frequency = frequency;}
};


class OutputBinary
{
  public:
    enum EOutputBinaryMode {
      BARATINOO_BINARY,
      BARATINOO_BINCVOX
    };
  protected:
    BARATINOO_SIGNAL_CODING _coding;
    int _frequency;
    int _SIGfilter;
    int _stopOnLink;
    EOutputBinaryMode _mode;
  public:
    DllExport OutputBinary():_coding(BARATINOO_PCM),_frequency(BARATINOO_OUTPUT_FREQUENCY),_SIGfilter(0),_stopOnLink(0),_mode(BARATINOO_BINARY) {}
    DllExport OutputBinary(BARATINOO_SIGNAL_CODING coding, int frequency, EOutputBinaryMode mode = BARATINOO_BINARY):_coding(coding),_frequency(frequency),_SIGfilter(0),_stopOnLink(0),_mode(mode) {}
    DllExport virtual ~OutputBinary() {};
    DllExport virtual void writeBinary(const void *address, int length) = 0;
    DllExport BARATINOO_SIGNAL_CODING getCoding() const {return _coding;}
    DllExport int getFrequency() const {return  _frequency;}
    DllExport int getSIGfilter() const {return _SIGfilter;}
    DllExport int getStopFlag() const {return _stopOnLink;}
    DllExport EOutputBinaryMode getBinaryMode() const {return _mode;}
    DllExport void setCoding( BARATINOO_SIGNAL_CODING coding) {_coding = coding;}
    DllExport void setFrequency( int frequency) {_frequency = frequency;}
    DllExport void setSIGfilter(int filter) {_SIGfilter = filter;}
    DllExport void setStopFlag(int stop) {_stopOnLink = stop;}
};
#endif
#else /* BARATINOO_C_API */
#ifndef _BARATINOO_C_H
typedef int (*BaratinooOutputSignalCB)(void *privateData, const void *address, int length);
typedef void (*BaratinooOutputBinaryCB)(void *privateData, const void *address, int length);
#endif
#endif

/* ============================================================================== */
/*  BaratinooEngine object is the public interface to Baratinoo TTS system        */
/*                                                                                */
/* Basic usage                                                                    */
/* ~~~~~~~~~~~                                                                    */
/*   -When library is initialized it is possible to create one or more            */
/*    BaratinooEngine objects. The library handle must be given to constructor.   */
/*   -When created, a BaratinooEngine must be initialized with a call to the      */
/*    init() member function. This function takes one parameter which is the      */
/*    file name of a Baratinoo configuration file. Before initialization, the     */
/*    BaratinooEngine is in UNINITIALIZED state. After it becomes INITIALIZED.    */
/*   -One input object (a specific implementation of the InputText class) must be */
/*    given to BaratinooEngine through a call to setInput() afterwhich the        */
/*    BaratinooEngine object is in READY state.                                   */
/*   -One output object (a specific implementation of the OutputSignal class) can */
/*    be given to BaratinooEngine through a call to setOutput(). This call is not */
/*    mandatory for the BaratinooEngine to run, but it is the only way to get the */
/*    signal.                                                                     */
/*   -It is then possible to call the processLoop() member which is the function  */
/*    where BaratinooEngine actually works and transforms input text into output  */
/*    signal. Optional parameter 'count' of processLoop() is the number of        */
/*    internal steps after which processLoop() should stop. If omited, the whole  */
/*    text is processed. It is not possible to know a priori how many steps are   */
/*    needed to process a given text. All steps do not last the same time. A good */
/*    value for 'count' depends on the application and how responsive it should   */
/*    be. Maybe between 10 and 100.                                               */
/*    When the whole text is processed, BaratinooEngine goes into INITIALIZED     */
/*    state again, where another call to setInput() is expected if another text   */
/*    is to be processed.                                                         */
/*   -A call to purge() set the BaratinooEngine to INITIALIZED state without      */
/*    finishing current input text.                                               */
/*   -Destruction of BaratinooEngine can occurs in all states. It implicitely     */
/*    calls purge() before deleting.                                              */
/*  Folowing functions returns a enum of type BARATINOO_STATE which reflects the  */
/*  current state of BaratinooEngine: init(), setInput(), processLoop(), purge()  */
/*                                                                                */
/*                                                                                */
/* Events                                                                         */
/* ~~~~~~                                                                         */
/*   -In parallel with output signal, a BaratinooEngine produces some events,     */
/*    synchronized with signal. Events are listed in enum BARATINOO_EVENT_TYPE.   */
/*   -An event is represented by a class BaratinooEvent which contains the        */
/*    associated values of event (in an union) and the timestamp of the event in  */
/*    the output signal. Timestamp unit is the sample. timestamp is reseted to 0  */
/*    for each input text.                                                        */
/*   -Possible events and their associated values can be fetch with a call to     */
/*    getEvent() (only in EVENT state).                                           */
/*   -Default behaviour of BaratinooEngine is to be silent (no event activated).  */
/*    Events are individually activated with a call to setWantedEvent(),          */
/*    individually deactivated with a call to unsetWantedEvent() and globally     */
/*    deactivated with a call to unsetAllWantedEvents(). Those three functions    */
/*    can be called from any state but UNINITIALIZED.                             */
/*                                                                                */
/*                                                                                */
/* State diagram                                                                  */
/* ~~~~~~~~~~~~~                                                                  */
/*   Here are the 7 possible states of a BaratinooEngine:                         */
/*     o UNINITIALIZED. This state is the state of a BaratinooEngine after        */
/*       creation. The only allowed function call in this state is init().        */
/*     o ERROR. This state is reach when a fatal error occurs. Probably because   */
/*       of lack of memory or a function call  not allowed in the current state  */
/*       or a function's argument not valid. More detail is given in an ERROR trace. In that case, */
/*       engine cannot run anymore and destruction is the only allowed action.    */
/*     o INITIALIZED. This state is reach after a call to init() and after        */
/*       processLoop() processed the whole input text. In this state a call       */
/*       to setInput() is expected.                                               */
/*     o READY. This state is reached from INITIALIZED or INPUT_ERROR states      */
/*       after a call to setInput(). A call to processLoop() is expected at that  */
/*       time.                                                                    */
/*     o RUNNING. This state is reached after a call to processLoop() when input  */
/*       text is partially processed. This happens if the optional value 'count'  */
/*       of processLoop() is not negative or if the callback                      */
/*       OutputSignal::writeSignal() returns a non zero value. In this state      */
/*       another call to processLoop() is expected.                               */
/*     o EVENT. This state is reached after a call to processLoop() when an       */
/*       activated event occurs. Associated value of current event can be fetch   */
/*       with a call to getEvent(). Another call to processLoop() is expected.    */
/*     o INPUT_ERROR. processLoop() returns this value when an error occurs in    */
/*       input, probably because of a parsing error (not valid XML document for   */
/*       example). Like in INITIALIZED state, another call to setInput() is       */
/*       necessary to change state to READY.                                      */
/*                                                                                */
/*                                                                                */
/* Exceptions                                                                     */
/* ~~~~~~~~~~                                                                     */
/*   No exception are thrown outside the library.  */
/*                                                                                */
/*                                                                                */
/* Multithread                                                                    */
/* ~~~~~~~~~~~                                                                    */
/*   The BARATINOO TTS system is multithread safe in a sense that many            */
/*   BaratinooEngine objects can exist in a same process, working in different    */
/*   threads (limitation coming from ressources: memory or license).              */
/*   But BaratinooEngine objects are not multithread safe. If two member          */
/*   functions are called on a same object from two different threads, result is  */
/*   undefined.                                                                   */
/* ============================================================================== */
#ifndef _BARATINOO_H
typedef enum {
    BARATINOO_MARKER_EVENT,
    BARATINOO_WAITMARKER_EVENT,
    BARATINOO_PARAGRAPHE_EVENT,
    BARATINOO_SENTENCE_EVENT,
    BARATINOO_WORD_EVENT,
    BARATINOO_PUNCTUATION_EVENT,
    BARATINOO_SEPARATOR_EVENT,
    BARATINOO_SYLLABLE_EVENT,
    BARATINOO_PHONEME_EVENT,
    BARATINOO_VISEME_EVENT,
    BARATINOO_SILENCE_EVENT,
    BARATINOO_NEW_VOICE_EVENT,
    BARATINOO_RAW_EVENT,

    BARATINOO_NUMBER_OF_EVENT
} BARATINOO_EVENT_TYPE;

typedef struct {
    BARATINOO_EVENT_TYPE type;
    float         timeStamp;            /* millisecond unit */
    unsigned long byteStamp;            /* byte unit */
    unsigned long sampleStamp;          /* sample unit */
    union   {
        struct    {
            const char *name;
        } marker;
        struct    {
            const char *name;
            float duration;             /* millisecond unit */
            int samples;                /* sample unit */
        } waitMarker;
        struct    {			/* UTF-8 encoding */
            const char *tts;
            const char *input;
        } word;
        struct    {			/* UTF-8 encoding */
            const char *tts;
            const char *input;
        } punctuation;
	struct {			/* UTF-8 encoding */
	    const char *input;
	} separator;
        struct    {
            const char *symbol;
            float duration;             /* millisecond unit */
            int samples;                /* sample unit */
        } phoneme;
        struct    {
            float duration;             /* millisecond unit */
            int samples;                /* sample unit */
            int type;			/* SAPI viseme type */
        } viseme;
        struct    {
            const char *name;
            const char *language;
        } newVoice;
        struct    {
            int type;
            int size;
            const char *datas;
        } raw;
    } data;
} BaratinooEvent;

typedef struct {
    const char *name;
    const char *language;
    const char *iso639;                 /* language: ISO-639 language */
    const char *iso3166;                /* language: ISO-3166 region */
    const char *variant;                /* language: IETF BCP47 variant */
    const char *accent;
    const char *gender;
    const char *version;
    const char *modules;
    int         age;                    /* equals 0 if unspecified */
    int         expire_days;            /* days until expiration of the activation key being used */
} BaratinooVoiceInfo;
#endif

#ifndef BARATINOO_C_API
#ifndef _BARATINOO_CPP_H
class Engine;
class BaratinooEngine
{
  public:
    enum BARATINOO_STATE {
        UNINITIALIZED,
        INITIALIZED,
        READY,
        RUNNING,
        EVENT,
        INPUT_ERROR,
        ENGINE_ERROR
    };
    enum BARATINOO_ERROR {
        NONE_ERROR,
        FORBIDDEN_CALL,
        BAD_PARAMETER,
        INVALID_CONFIGURATION,
        NOT_ENOUGH_MEMORY,
        INTERNAL_ERROR,
        LAST_ERROR
    };
    enum BARATINOO_CANSPEAK {
        CANSPEAK_FAILURE = 0,
        CANSPEAK_LANGUAGE,
        CANSPEAK_LANGUAGE_REGION,
        CANSPEAK_LANGUAGE_REGION_VARIANT
    };

  private:
    BARATINOO_STATE     _state;
    int                 _engineNumber;
    const void         *_privatedata;		/* private data returned in trace callback */
    Engine             *_engine;
    BaratinooVoiceInfo  _voiceInfo;

    void exceptionHandler(BARATINOO_ERROR error);

    BaratinooEngine(const BaratinooEngine& baratinooEngine); /* private, not supported or implemented */
    BaratinooEngine& operator= (const BaratinooEngine& baratinooEngine); /* private, not supported or implemented */

  public:
    /* use newInstance/deleteInstance instead of new/delete operators to create and destroy BaratinooEngine objects on the heap.
       However if your own class inherits BaratinooEngine, you must use new/delete operators to create or delete objects of that class. */
    DllExport static BaratinooEngine *newInstance(const void *privatedata = NULL);
    DllExport static void deleteInstance(BaratinooEngine *instance);

    DllExport BaratinooEngine(const void *privatedata = NULL);
    DllExport virtual ~BaratinooEngine();
    DllExport BARATINOO_STATE init(const char *config, const char *voicesFilter = NULL, const char *languagesFilter = NULL, const char *gendersFilter = NULL);
    DllExport int getEngineNumber() {return _engineNumber; }
    DllExport BARATINOO_STATE getState() {return _state;}

    DllExport BARATINOO_STATE setInput(InputText *input);
    DllExport BARATINOO_STATE setInput(InputBinary *input);
    DllExport void setOutput(OutputSignal *output);
    DllExport void setOutput(OutputBinary *output);

    DllExport BARATINOO_STATE processLoop(int count = -1);

    DllExport BARATINOO_STATE purge();

    DllExport void setWantedEvent(BARATINOO_EVENT_TYPE type);
    DllExport void unsetWantedEvent(BARATINOO_EVENT_TYPE type);
    DllExport void setAllWantedEvent();
    DllExport void unsetAllWantedEvent();
    DllExport BaratinooEvent &getEvent();

    DllExport int getNumberOfVoices();
    DllExport BaratinooVoiceInfo &getVoiceInfo(int voiceNumber);
    DllExport BARATINOO_CANSPEAK canSpeak (int voiceNumber, const char *language_iso639, const char *region_iso3166, const char *variant);
};
#endif
#else /* BARATINOO_C_API */
#ifndef _BARATINOO_C_H
typedef enum {
    BARATINOO_UNINITIALIZED,
    BARATINOO_INITIALIZED,
    BARATINOO_READY,
    BARATINOO_RUNNING,
    BARATINOO_EVENT,
    BARATINOO_INPUT_ERROR,
    BARATINOO_ENGINE_ERROR
} BARATINOOC_STATE;
typedef enum {
    BARATINOO_CANSPEAK_FAILURE = 0,
    BARATINOO_CANSPEAK_LANGUAGE,
    BARATINOO_CANSPEAK_LANGUAGE_REGION,
    BARATINOO_CANSPEAK_LANGUAGE_REGION_VARIANT
} BARATINOO_CANSPEAK;

typedef void* BCengine;
#ifdef __cplusplus
extern "C" {
#endif
    DllExport BCengine BCnew(const void *privatedata);
    DllExport void BCinit(BCengine engine, const char *config);
    DllExport void BCinitWithVoices(BCengine engine, const char *config, const char *voicesFilter);
    DllExport void BCinitWithFilters(BCengine engine, const char *config, const char *voicesFilter, const char *languagesFilter, const char *gendersFilter);
    DllExport void BCdelete(BCengine engine);
    DllExport int BCgetEngineNumber(BCengine engine);
    DllExport BARATINOOC_STATE BCgetState(BCengine engine);
    DllExport BARATINOOC_STATE BCsetInputText(BCengine engine, BaratinooInputTextCB cb, void *privateData, BARATINOO_PARSING parsing, BARATINOO_TEXT_ENCODING encoding, int voiceIndex, const char *voiceModules, const char *uri);
    DllExport BARATINOOC_STATE BCsetInputBinary(BCengine engine, BaratinooInputBinaryCB cb, void *privateData);
    DllExport void BCsetOutputSignal(BCengine engine, BaratinooOutputSignalCB cb, void *privateData, BARATINOO_SIGNAL_CODING coding, int frequency);
    DllExport void BCsetOutputBinary(BCengine engine, BaratinooOutputBinaryCB cb, void *privateData, BARATINOO_SIGNAL_CODING coding, int frequency, int SIGfilter, int stopFlag);
    DllExport BARATINOOC_STATE BCprocessLoop(BCengine engine, int count);
    DllExport BARATINOOC_STATE BCpurge(BCengine engine);
    DllExport void BCsetWantedEvent(BCengine engine, BARATINOO_EVENT_TYPE type);
    DllExport void BCunsetWantedEvent(BCengine engine, BARATINOO_EVENT_TYPE type);
    DllExport void BCsetAllWantedEvent(BCengine engine);
    DllExport void BCunsetAllWantedEvent(BCengine engine);
    DllExport BaratinooEvent BCgetEvent(BCengine engine);
    DllExport int BCgetNumberOfVoices(BCengine engine);
    DllExport BaratinooVoiceInfo BCgetVoiceInfo(BCengine engine,int voiceNumber);
    DllExport BARATINOO_CANSPEAK BCcanSpeak (BCengine engine, int voiceNumber, const char *language_iso639, const char *region_iso3166, const char *variant);
#ifdef __cplusplus
}
#endif
#endif
#endif


#ifndef _BARATINOO_H
#define _BARATINOO_H
#endif
#ifndef BARATINOO_C_API
#ifndef _BARATINOO_CPP_H
#define _BARATINOO_CPP_H
#endif
#else
#ifndef _BARATINOO_C_H
#define _BARATINOO_C_H
#endif
#endif
