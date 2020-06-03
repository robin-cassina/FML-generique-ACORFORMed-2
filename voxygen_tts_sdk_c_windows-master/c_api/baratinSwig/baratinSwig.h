/*
 * A little C++ interface for the Voxygen baratinoo library.
 * Based on the barademo_CPP example.
 *
 *  Created on: 5 avr. 2017
 *      Author: montcheuil
 */

#ifndef BARATINSWIG_H_
#define BARATINSWIG_H_

#include <vector>
#include <string>

#include "baratinoo.h"
#include "baratinooio.h"

// Define SwigDllExport based on SWIG_MAKE_DLL (similar to baratinoo.h)
#if defined(_WIN32) || defined(_WIN64)
  #if defined (SWIG_MAKE_DLL)
    #pragma message ( "[WIN] SWIG_MAKE_DLL is defined")
    #define SwigDllExport __declspec( dllexport )
  #else
    #pragma message ( "[WIN] SWIG_MAKE_DLL isn't defined")
    //#define SwigDllExport
    #define SwigDllExport __declspec( dllimport )
  #endif
#else
  #if defined (SWIG_MAKE_DLL) && __GNUC__ >= 4
    #pragma message ( "[!WIN] SWIG_MAKE_DLL is defined")
    #define SwigDllExport __attribute__ ((visibility("default")))
  #else
    #pragma message ( "[!WIN] SWIG_MAKE_DLL isn't defined (or GNUC<4)")
    #define SwigDllExport
  #endif
#endif

typedef OutputSignalBuffer::HEADER_FORMAT SOUND_HEADER_FORMAT;
typedef char byte;

/**
 * The logging class.
 */
class BaratinooTraceHelper :
		public BaratinooTrace	// Helper for BaratinooTrace
{
	//--- Constructor parameters ---//
    //--- Working variables ---//
    FILE                    *_log = NULL;	// (optional) the log file handler

    protected:
		/** (optional) A log file path. */
        char                    *_logfile;

        //--- BaratinooTrace ---//
        using BaratinooTrace::write; // unhide base class virtual functions
        void write(TraceLevel level, int engineNumber, const char *source, const char *format, va_list args);
        TraceLevel traceLevel = TRACE_DEBUG;

    public:
        SwigDllExport BaratinooTraceHelper(char *logfile=NULL): _logfile(logfile) {};
        SwigDllExport ~BaratinooTraceHelper();

        /** Open the log file.*/
        void open();

        // trace level
        TraceLevel getTraceLevel();
        void setTraceLevel(TraceLevel level);

        // - help methods
        SwigDllExport void traceError(const char *source, const char *format, ...);
        SwigDllExport void traceWarning(const char *source, const char *format, ...);
        SwigDllExport void traceInfo (const char *source, const char *format, ...);
        SwigDllExport void traceDebug (const char *source, const char *format, ...);
        SwigDllExport void trace(TraceLevel level, const char *source, const char *format, ...);
        SwigDllExport void trace(TraceLevel level, const char *source, const char *format, va_list args);
};

/**
 * A class for all timestamp.
 */
class TimeStamp {
	public:
		/** The timestamp in milliseconds (since the start of the signal)  */
		float millisecond;
		/** The timestamp in bytes (since the start of the signal)  */
		unsigned long bytes;
		/** The timestamp in samples (since the start of the signal)  */
		unsigned long samples;

		/** Constructor with values */
		SwigDllExport TimeStamp(float m=0., unsigned long b=0, unsigned long s=0): millisecond(m), bytes(b), samples(s) {};
		/** Constructor copy */
		SwigDllExport TimeStamp(const TimeStamp &ts): millisecond(ts.millisecond), bytes(ts.bytes), samples(ts.samples) {};
		/** Constructor with BaratinooEvent */
		SwigDllExport TimeStamp(const BaratinooEvent &be): millisecond(be.timeStamp), bytes(be.byteStamp), samples(be.sampleStamp) {};

		/** Destuctor.*/
		SwigDllExport virtual ~TimeStamp() {};  // a virtual method is required to be polymorphic (and dynamic_cast<>)
};

// Number of visemes for a mapping int --> viseme name/symbol
static const int BARATINOO_INDEX_LAST_VISEME = 21;

static const char* VISEME_NAMES[BARATINOO_INDEX_LAST_VISEME+1] = {
		 "#"  // 0 FR: # ^
		,"A"  // 1 FR: A AN E
		,"aa" // 2 FR-; EN: aa (dart --> d aa t)
		,"O"  // 3 FR: O AU EU OE ON UN (mode --> M O D)
		,"AI" // 4 FR: AI EI IN (procès --> P R AU S AI)
		,"@"  // 5 FR-; EN: @ (about --> @ b au t)
		,"Y"  // 6 FR: Y I
		,"W"  // 7 FR: W U OU UI
		,"ou" // 8 FR-; EN: ou (go --> g ou)
		,"au" // 9 FR-; EN: au (how --> h au)
		,"oi" //10 FR-; EN: oi (boy --> b oi)
		,"ai" //11 FR-; EN: ai (buy --> b ai)
		,"%"  //12 FR: % (???); EN: H % (hat --> h a t)
		,"r"  //13 FR-; EN: r
		,"L"  //14 FR: L
		,"Z"  //15 FR: Z S
		,"J"  //16 FR: J CH
		,"dh"  //17 FR-; EN: dh th (then --> dh e n; thin --> th i n)
		,"V"  //18 FR: V F
		,"T"  //19 FR: T D N
		,"K"  //20 FR: K G R
		,"P"  //21 FR: P B M
};

/**
 * A class for all timestamped events.
 */
class TimeStampEvent : public TimeStamp
{
	public:
		BARATINOO_EVENT_TYPE type;

		/** The name of the event :
		 *  - the <i>id</i> in case of marker events.
		 *  - the <i>input</i> in case of word/punctuation/separator events.
		 *  - the <i>symbol</i> in case of phoneme events.
		 *  - the <i>number</i>(0-21) in case of viseme events.
		 */
		//const char *name;
		std::string name;

		/** Constructor with values */
		SwigDllExport TimeStampEvent(float m=0., unsigned long b=0, unsigned long s=0, BARATINOO_EVENT_TYPE t=BARATINOO_EVENT_TYPE::BARATINOO_NUMBER_OF_EVENT, const char *n=""): TimeStamp(m, b, s), type(t), name() { name.append(n);};
		/** Constructor with TimeStamp */
		SwigDllExport TimeStampEvent(const TimeStamp &ts, BARATINOO_EVENT_TYPE t, const char *n=""): TimeStamp(ts), type(t), name() { name.append(n);};
		/** Constructor copy */
		SwigDllExport TimeStampEvent(const TimeStampEvent &tse): TimeStamp(tse), type(tse.type), name() { name.append(tse.name);};
		/** Constructor with BaratinooEvent */
		SwigDllExport TimeStampEvent(const BaratinooEvent &be): TimeStamp(be), type(be.type), name() {
			const char *n=NULL;
			std::string tostr; // useful to preserve viseme.type --> string conversion
			switch (be.type) {
			case BARATINOO_MARKER_EVENT:
				n = be.data.marker.name; break;
			case BARATINOO_WAITMARKER_EVENT:
				n = be.data.waitMarker.name; break;
			case BARATINOO_WORD_EVENT:
				n = be.data.word.input; break;	//TODO: add the tts field
			case BARATINOO_PUNCTUATION_EVENT:
				n = be.data.punctuation.input; break;	//TODO: add the tts field
			case BARATINOO_SEPARATOR_EVENT:
				n = be.data.separator.input; break;
			case BARATINOO_PHONEME_EVENT:
				n = be.data.phoneme.symbol; break;
			case BARATINOO_VISEME_EVENT:
				// convert viseme.type (int) --> viseme name (string)
				// (a) VISEME_NAMES
				if (be.data.viseme.type>=0 && be.data.viseme.type<=BARATINOO_INDEX_LAST_VISEME) { // use the name
					n = VISEME_NAMES[be.data.viseme.type];
				}
				if (n==NULL) { //default: convert the value into string
					tostr = std::to_string(be.data.viseme.type);
					n = tostr.c_str(); // get the char *
				}
				break;
			case BARATINOO_NEW_VOICE_EVENT:
				n = be.data.newVoice.name; break;

			case BARATINOO_PARAGRAPHE_EVENT:
			case BARATINOO_SENTENCE_EVENT:
			case BARATINOO_SYLLABLE_EVENT:
			case BARATINOO_SILENCE_EVENT:
			case BARATINOO_RAW_EVENT:
			default:
				n = "--NoNameEvent--";
			}
			if (n!=NULL) name.append(n);
		};

		/** Destuctor.*/
		SwigDllExport virtual ~TimeStampEvent() {};  // a virtual method is required to be polymorphic (and dynamic_cast<>)
};

/**
 * A class for timestamped events with a duration, like phonemes or wait marker.
 */
class ContinuousEvent : public TimeStampEvent
{
	public:
		/** The event duration (in milliseconds) */
		float duration=0.;
		/** The event duration (in samples) */
		int nb_samples=0;

		/** Constructor with values */
		SwigDllExport ContinuousEvent(float m, unsigned long b, unsigned long s, BARATINOO_EVENT_TYPE t, const char *n, float d=0., int ns=0): TimeStampEvent(m, b, s, t, n), duration(d), nb_samples(ns) {};
		/** Constructor with TimeStampEvent */
		SwigDllExport ContinuousEvent(const TimeStampEvent &tse,  float d=0., int ns=0): TimeStampEvent(tse),  duration(d), nb_samples(ns) {};
		/** Constructor copy */
		SwigDllExport ContinuousEvent(const ContinuousEvent &ce): TimeStampEvent(ce), duration(ce.duration), nb_samples(ce.nb_samples) {};
		/** Constructor with BaratinooEvent */
		SwigDllExport ContinuousEvent(const BaratinooEvent &be): TimeStampEvent(be) {
			switch (be.type) {
			case BARATINOO_WAITMARKER_EVENT:
				duration = be.data.waitMarker.duration;
				nb_samples = be.data.waitMarker.samples;
				break;
			case BARATINOO_PHONEME_EVENT:
				duration = be.data.phoneme.duration;
				nb_samples = be.data.phoneme.samples;
				break;
			case BARATINOO_VISEME_EVENT:
				duration = be.data.viseme.duration;
				nb_samples = be.data.viseme.samples;
				break;
			/*
			case BARATINOO_MARKER_EVENT:
			case BARATINOO_PARAGRAPHE_EVENT:
			case BARATINOO_SENTENCE_EVENT:
			case BARATINOO_WORD_EVENT:
			case BARATINOO_PUNCTUATION_EVENT:
			case BARATINOO_SEPARATOR_EVENT:
			case BARATINOO_SYLLABLE_EVENT:
			case BARATINOO_SILENCE_EVENT:
			case BARATINOO_NEW_VOICE_EVENT:
			case BARATINOO_RAW_EVENT:
			default:
			*/
			};
		};

		/** Destuctor.*/
		SwigDllExport virtual ~ContinuousEvent() {};  // a virtual method is required to be polymorphic (and dynamic_cast<>)
};

/**
 * A class to store the SpeechResult of a BaratinooSwig::textToSpeech() call.
 */
class SpeechResult {
	friend class BaratinooSwig; // BaratinooSwig can access SpeechResult elements

	private:
        //--- Working variables ---//
		//byte *_pHeader = NULL;
        std::vector<byte> _vHeader;
	    //int _headerLength = -1;	// header memory
        std::vector<byte> _vRawData;//
		//byte *_sound = NULL; // the sound signal
		std::vector<byte> _vSound;// = NULL; // the sound signal

	protected:
		//BARATINOO_SIGNAL_CODING _coding;
		//int _frequency;
		OutputSignalBuffer *_outputSignal;
		SOUND_HEADER_FORMAT _format;	// the "default" header format

		BaratinooTraceHelper *_logger;

		//TODO: mark mapping, phonemes list, etc.
		std::vector<TimeStampEvent *> _allEvents;
		std::vector<TimeStampEvent *> _markerEvents;
		std::vector<ContinuousEvent *> _visemeEvents;

	public:
		static const int BYTES_PER_SAMPLE = 2;

		//DllExport SpeechResult(BARATINOO_SIGNAL_CODING coding=BARATINOO_PCM, int frequency=BARATINOO_OUTPUT_FREQUENCY):_coding(coding),_frequency(frequency) {}
		SwigDllExport SpeechResult(OutputSignalBuffer *outputSignal
				, SOUND_HEADER_FORMAT format = SOUND_HEADER_FORMAT::WAV_HEADER
				, BaratinooTraceHelper *logger = NULL
				): _outputSignal(outputSignal), _format(format), _logger(logger) { };
		SwigDllExport ~SpeechResult();

		SwigDllExport static void deleteInstance(SpeechResult *instance) {delete instance;};

		// Getter for the OutputSignal information
		SwigDllExport BARATINOO_SIGNAL_CODING getCoding() const {return _outputSignal->getCoding();}
		SwigDllExport int getFrequency() const {return  _outputSignal->getFrequency();}

		//TODO: check/get _outputSignal->isError();

		// Getter for the OutputSignalBuffer
		/** Get the output error status.*/
		SwigDllExport int isError() {return _outputSignal->isError();}
		/** Get the raw data length (without header, in byte) */
		//SwigDllExport int   getRawDataLength()                         { return _outputSignal->getSignalLength(); }
		SwigDllExport size_t   getRawDataLength();//                         { return _outputSignal->getSignalLength(); }
		/** Get the raw data (without header, in byte) */
		//SwigDllExport byte *getRawData()                         { return _outputSignal->getSignalBuffer();}
		SwigDllExport std::vector<byte> getRawData();//                         { return _outputSignal->getSignalBuffer();}
		/** Get the header length (in byte) */
		//SwigDllExport int   getHeaderLength();
		SwigDllExport size_t   getHeaderLength();
		/** Get the header */
		//SwigDllExport byte *getHeader();
		SwigDllExport std::vector<byte> getHeader();
		/** Get the speech = (header + raw data) length (in byte) */
		//SwigDllExport int   getSoundLength()                         { return _outputSignal->getSignalLength() + getHeaderLength(); }//TMP
		SwigDllExport size_t   getSoundLength()                         { return _outputSignal->getSignalLength() + getHeaderLength(); }//TMP
		/** Get the speech = (header + raw data) */
		//SwigDllExport byte *getSound();
		SwigDllExport std::vector<byte> getSound();

		/** Get the speech number of samples.*/
		SwigDllExport int getNbSamples() { return (int) getRawDataLength() / BYTES_PER_SAMPLE;};
		/** Get the speech duration in milisecond.*/
		SwigDllExport float getDuration() { return (1000.0/BYTES_PER_SAMPLE) * getRawDataLength() / getFrequency();};

		// Collected events
		/** Get the list of all (collected) events.*/
		SwigDllExport std::vector<TimeStampEvent *> getAllEvents();
		/** Number of all (collected) events.*/
		SwigDllExport int getAllEventsSize() {return (int) _allEvents.size();};

		/** Get the list of (collected) marker events.*/
		SwigDllExport std::vector<TimeStampEvent *> getMarkerEvents();
		/** Number of (collected) marker events.*/
		SwigDllExport int getMarkerEventsSize() {return (int) _markerEvents.size();};

		/** Get the list of (collected) viseme events.*/
		SwigDllExport std::vector<ContinuousEvent *> getVisemeEvents();
		/** Number of (collected) viseme events.*/
		SwigDllExport int getVisemeEventsSize() {return (int) _visemeEvents.size();};

		/** Get the list of events of a type.*/
		SwigDllExport std::vector<TimeStampEvent *> getTypedEvents(BARATINOO_EVENT_TYPE type);
		/** Number of (collected) events of a type.*/
		SwigDllExport int getTypedEventsSize(BARATINOO_EVENT_TYPE type);
};

enum LogLevel {
        ERROR = BaratinooTrace::TraceLevel::TRACE_ERROR,
        INIT = BaratinooTrace::TraceLevel::TRACE_INIT,
        WARNING = BaratinooTrace::TraceLevel::TRACE_WARNING,
        INFO = BaratinooTrace::TraceLevel::TRACE_INFO,
        DEBUG = BaratinooTrace::TraceLevel::TRACE_DEBUG
};

/**
 * A class to create a BaratinooEngine
 *  and convert (binary) text into speech data
 *  and complementary information (as marks and phonemes timestamps).
 *
 * Lifecycle :
 *  (a) Creation of a new BaratinooSwig
 *    => create an UNITIALIZED BaratinooEngine
 *  (b) Set the initialization parameters
 *    . config file, etc.
 *  (c) init() the BaratinooEngine for this config file
 *  (d) Set the working parameters :
 *    . input encoding and parsing mode
 *    . output coding and frequency
 *    . events to collect
 *  (e) textToSpeech() : convert binary text into speech data
 *  Repeat (d) and (e)...
 *  (f) shutdown() => destroy the BaratinooEngine
 */
class BaratinooSwig //:
		//public BaratinooTraceHelper //public BaratinooTrace	//TMP: implement BaratinooTrace for logging
		// private BaratinooKey	//TMP: implement BaratinooKey just in case
{

    private:
		//--- Constructor parameters ---//
		/** (optional) A output directory. */
        char                    *_outputPath;

		/** (optional) The (BaratinooTraceHelper) log file path. */
        char                    *_logfile;

		//--- Initialization parameters ---//
		/** The Voxygen config file path. */
        char                    *_configFile;

        //--- Engine, input/output parameters ---//
        // - Events
        bool _events[BARATINOO_EVENT_TYPE::BARATINOO_NUMBER_OF_EVENT] = {};
        /** The default events.*/
        static const int DEFAULT_EVENTS_LENGTH = 2;
        static const BARATINOO_EVENT_TYPE DEFAULT_EVENTS[DEFAULT_EVENTS_LENGTH];//Error[c2864] = {BARATINOO_EVENT_TYPE::BARATINOO_MARKER_EVENT, BARATINOO_EVENT_TYPE::BARATINOO_PHONEME_EVENT};

        // - InputText
        BARATINOO_PARSING _parsing = BARATINOO_PARSING::BARATINOO_XML_PARSING; // only SSML by default
        BARATINOO_TEXT_ENCODING _encoding = BARATINOO_TEXT_ENCODING::BARATINOO_DEFAULT_ENCODING; // default/XML aware encoding by default

        // - OutputSignal
        BARATINOO_SIGNAL_CODING _coding = BARATINOO_SIGNAL_CODING::BARATINOO_PCM; //TODO
        int _frequency = BARATINOO_OUTPUT_FREQUENCY;	// 24000Hz
		SOUND_HEADER_FORMAT _format = SOUND_HEADER_FORMAT::WAV_HEADER;	// the "default" header format

        //--- Working variables ---//
        BaratinooEngine         *_engine;	// the BaratinooEngine

        BaratinooSwig(const BaratinooSwig& lite); // private, not supported or implemented
        BaratinooSwig& operator= (const BaratinooSwig& lite); // private, not supported or implemented

        /*
        //--- BaratinooKey ---//
        // begin share secret key : specific use case
        using BaratinooKey::get; // unhide base class virtual functions
        const char * get(int index) const { return (0 == index ? "shared_secret_key_provided_by_Voxygen" : NULL); }
        // end share secret key
         */

    protected:
        //TMP ?
        // reset the engine
        BaratinooEngine::BARATINOO_STATE reset(BaratinooEngine::BARATINOO_STATE state);

		BaratinooTraceHelper *_logger;
		BaratinooKey *_key;//TODO

    public:
        SwigDllExport BaratinooSwig(const char *outputPath, const char *logFilename);
        SwigDllExport ~BaratinooSwig();

        // init and shutdown methods
        SwigDllExport int init(const char *configurationFile);
        SwigDllExport void shutdown();

        // log/trace level
        SwigDllExport void setLogLevel(LogLevel level);
        SwigDllExport LogLevel getLogLevel();

        //SwigDllExport int textToSpeech(InputText *pInputText, OutputSignal *pOutputSignal);
        /** Convert a (binary) text into speech data. */
        SwigDllExport SpeechResult *textToSpeech(const byte *text);

        // Getter/Setter
        //- InputText
        /** Get the input encoding.*/
        SwigDllExport BARATINOO_TEXT_ENCODING getEncoding() const { return _encoding; }
        /** Set the input encoding.*/
        SwigDllExport void setEncoding(BARATINOO_TEXT_ENCODING encoding) { _encoding = encoding; }

        /** Get the parsing type.*/
        SwigDllExport BARATINOO_PARSING getParsing() const { return _parsing; }
        /** Set the parsing type.*/
        SwigDllExport void setParsing(BARATINOO_PARSING parsing) { _parsing = parsing; }

        //- OutputSignal
        /** Get the signal coding.*/
        SwigDllExport BARATINOO_SIGNAL_CODING getCoding() const { return _coding; }
        /** Set the signal coding.*/
        SwigDllExport void setCoding(BARATINOO_SIGNAL_CODING coding) { _coding = coding; }

        /** Get the output signal frequency.*/
        SwigDllExport int getFrequency() const { return _frequency; }
        /** Set the output signal frequency.*/
        SwigDllExport void setFrequency(int frequency) { _frequency = frequency; }

        /** Get the sound format (wav or au).*/
        SwigDllExport SOUND_HEADER_FORMAT getFormat() const { return _format; }
        /** Set the sound format (wav or au).*/
        SwigDllExport void setFormat(SOUND_HEADER_FORMAT format) { _format = format; }

        //-Engine
        //TODO: Getter to Engine information : version, voice(s) information, etc.
        /** Set an events */
        SwigDllExport void setWantedEvent(const BARATINOO_EVENT_TYPE event, const bool value=true);//TODO { _events[event] = value; }
        /** Unset an events */
        SwigDllExport void unsetWantedEvent(const BARATINOO_EVENT_TYPE event) { setWantedEvent(event,false); }
        /** Get an events */
        SwigDllExport bool isWantedEvent(const BARATINOO_EVENT_TYPE event) { return _events[event]; }
        /** */
        SwigDllExport void setAllWantedEvent();
        SwigDllExport void unsetAllWantedEvent();
        //TODO: list of wanted events

        // - BaratinooTraceHelper help methods
        SwigDllExport void traceError(const char *source, const char *format, ...);
        SwigDllExport void traceWarning(const char *source, const char *format, ...);
        SwigDllExport void traceInfo (const char *source, const char *format, ...);
        SwigDllExport void traceDebug (const char *source, const char *format, ...);
        //SwigDllExport void trace(TraceLevel level, const char *source, const char *format, ...);
     };


// just in case, undefine SwigDllExport
#undef SwigDllExport

#endif /* BARATINSWIG_H_ */
