/*
 * A little C++ interface for the Voxigen baratinoo library.
 * Based on the barademo_CPP example.
 *
 *  Created on: 5 avr. 2017
 *      Author: montcheuil
 */

#include "baratinSwig.h"
#include <string.h>

//--------------------   BaratinooTraceHelper  ----------------//

BaratinooTraceHelper::~BaratinooTraceHelper() {
    if (_log != NULL)
        fclose(_log);
}

void BaratinooTraceHelper::open() {
    // all application traces will be written in log file if specified (otherwise they will be discarded)
    if (NULL != _logfile)
        _log = fopen(_logfile, "w+");
}

BaratinooTrace::TraceLevel BaratinooTraceHelper::getTraceLevel() {
	return traceLevel;
}

void BaratinooTraceHelper::setTraceLevel(TraceLevel level) {
	traceLevel = level;
}

void BaratinooTraceHelper::trace(TraceLevel level, const char *source, const char *format, va_list args)
{
	if (level > traceLevel) return;
	write(level, 0, source, format, args);
}

void BaratinooTraceHelper::trace(TraceLevel level, const char *source, const char *format, ...)
{
	if (level > traceLevel) return;
	va_list args;
	va_start(args, format);
	write(level, 0, source, format, args);
	va_end(args);
}

void BaratinooTraceHelper::traceError(const char *source, const char *format, ...)
{
	if (BaratinooTrace::TRACE_ERROR > traceLevel) return;
	va_list args;
	va_start(args, format);
	write(BaratinooTrace::TRACE_ERROR, 0, source, format, args);
	va_end(args);
}

void BaratinooTraceHelper::traceWarning(const char *source, const char *format, ...)
{
	if (BaratinooTrace::TRACE_WARNING > traceLevel) return;
	va_list args;
	va_start(args, format);
	write(BaratinooTrace::TRACE_WARNING, 0, source, format, args);
	va_end(args);

}

void BaratinooTraceHelper::traceInfo(const char *source, const char *format, ...)
{
	if (BaratinooTrace::TRACE_INFO > traceLevel) return;
	va_list args;
	va_start(args, format);
	write(BaratinooTrace::TRACE_INFO, 0, source, format, args);
	va_end(args);
}

void BaratinooTraceHelper::traceDebug(const char *source, const char *format, ...)
{
	if (BaratinooTrace::TRACE_DEBUG > traceLevel) return;
	va_list args;
	va_start(args, format);
	write(BaratinooTrace::TRACE_DEBUG, 0, source, format, args);
	va_end(args);
}
//---------------------------------------------------------------------------------
/* Trace callback implementation from BaratinooTrace
 *   level : trace level [error, init, warning, info, debug]. See baratinoo.h
 *   engineNumber : 0 if trace comes from baratinoo library itself, otherwise the number of the engine which generated the trace (use _engine->getEngineNumber() to get it)
 *   format et args : trace message format and arguments (as used by vprintf function)
 *
 * It's up to your application to device what to do with those traces, no filtering is done by baratinoo library.
 */
void BaratinooTraceHelper::write(TraceLevel level, int engineNumber, const char *source, const char *format, va_list args)
{
    if (_log == NULL)
        return;
    if (level > traceLevel) return;

    (void)engineNumber;        // unused parameter
    const char *prefix;
    switch(level) {
      case BaratinooTrace::TRACE_ERROR:
          prefix = "ERROR";
          break;
      case BaratinooTrace::TRACE_INIT:
          prefix = "INIT";
          break;
      case BaratinooTrace::TRACE_WARNING:
          prefix = "WARNING";
          break;
      case BaratinooTrace::TRACE_INFO:
          prefix = "INFO";
          break;
      case BaratinooTrace::TRACE_DEBUG:
          prefix = "DEBUG";
          break;
      default:
          prefix = "Unknown trace level";
          break;
    }
    fprintf(_log, "%s : %s ", prefix, source);
    vfprintf(_log, format, args);
    fprintf(_log, "\n");
    fflush(_log);
}



//--------------------  SpeechResult ----------------//
// class to store the SpeechResult of a BaratinooSwig::textToSpeech() call.

SpeechResult::~SpeechResult() {
	//if (_pHeader!=NULL) {_outputSignal->deleteSignalHeader(_pHeader);  _pHeader = NULL;}// delete _pHeader
	//if (_sound !=NULL) { delete [] _sound; _sound = NULL;} // delete _sound
	OutputSignalBuffer::deleteInstance(_outputSignal); // delete the OutputSignalBuffer
	// delete all events
    for (auto &event : _allEvents) {
    	delete event; //nota: event is a (TimeStampEvent *) created with new
    }
};

//byte * SpeechResult::getHeader()
std::vector<byte> SpeechResult::getHeader()
{
	//TODO: check _outputSignal is ended
	//if (_pHeader == NULL)
	if (_vHeader.empty())
	{
		byte * _pHeader;
		// write the header in _pHeader
		int _headerLength = _outputSignal->getSignalHeader(_format, &_pHeader);
		if (_headerLength <= 0) {
			if (_logger != NULL) _logger->traceError("SpeechResult", "negative header length:%d (error?%d)", _headerLength, _outputSignal->isError());
			//_pHeader = NULL;
		} else {
			_vHeader.resize(_headerLength);
			_vHeader.assign(_pHeader, _pHeader+_headerLength);
			_outputSignal->deleteSignalHeader(_pHeader);// we have copied _pHeader into _vHeader and can release it
		}
	}
	return _vHeader;
}

//int SpeechResult::getHeaderLength()
size_t SpeechResult::getHeaderLength()
{
	//TODO: check _outputSignal is ended (in getHeader())
	//if (_pHeader == NULL) getHeader(); // build header if necessary
	//return _headerLength;
	if (_vHeader.empty()) getHeader(); // build header if necessary
	return _vHeader.size();
}

std::vector<byte> SpeechResult::getRawData() {
	if (_vRawData.empty()) {
		int length = _outputSignal->getSignalLength();
		char *data = _outputSignal->getSignalBuffer();
		_vRawData.resize(length);
		_vRawData.assign(data, data+length);
	}
	return _vRawData;
}

//int SpeechResult::getRawDataLength()
size_t SpeechResult::getRawDataLength()
{
	if (_vRawData.empty()) {
		return _outputSignal->getSignalLength();
	}
	return _vRawData.size();
}

//byte * SpeechResult::getSound()
std::vector<byte> SpeechResult::getSound()
{
	//TODO: check _outputSignal is ended (in getHeader())
	//if (_sound == NULL)
	if (_vSound.empty())
	{
		//if (_pHeader == NULL) getHeader(); // build header if necessary
		if (_vHeader.empty()) getHeader(); // build header if necessary
		size_t headerLength = _vHeader.size();
		int dataLenght = _outputSignal->getSignalLength();
		//_sound = new byte[_headerLength + dataLenght];
		//if (_sound != NULL)
		{
			_vSound.resize(headerLength + dataLenght);
			// copy the header
			//memcpy(_sound, _pHeader, _headerLength);
			_vSound.assign(_vHeader.begin(), _vHeader.end());
			// copy the raw data
			//memcpy(_sound+_headerLength, _outputSignal->getSignalBuffer(), dataLenght);
			if (_vRawData.empty()) {
				int length = _outputSignal->getSignalLength();
				char *data = _outputSignal->getSignalBuffer();
				_vSound.insert(_vSound.begin()+headerLength, data, data+length);
			} else {
				_vSound.insert(_vSound.begin()+headerLength, _vRawData.begin(), _vRawData.end());
			}
		}// else { }//TODO: error
	}
	return _vSound;
}


std::vector<TimeStampEvent *> SpeechResult::getAllEvents() {
	return _allEvents;
}

std::vector<TimeStampEvent *> SpeechResult::getMarkerEvents() {
	return _markerEvents;
}

std::vector<ContinuousEvent *> SpeechResult::getVisemeEvents() {
	return _visemeEvents;
}

std::vector<TimeStampEvent *> SpeechResult::getTypedEvents(BARATINOO_EVENT_TYPE type) {
	if (type == BARATINOO_MARKER_EVENT) return this->getMarkerEvents();
	//CAST PB:if (type == BARATINOO_VISEME_EVENT) return this->getVisemeEvents();
	std::vector<TimeStampEvent *> res;
	int nb = getTypedEventsSize(type);
	if (nb>0) {
		res.reserve(nb);
    	for (const auto &event : _allEvents) {
			if (event->type == type) res.push_back(event);
		}
	}
	return res;
}

int SpeechResult::getTypedEventsSize(BARATINOO_EVENT_TYPE type) {
	int nb=0;
    for (const auto &event : _allEvents) {
		if (event->type == type) ++nb;
	}
	return nb;
};

//--------------------  BaratinooSwig ----------------//

//---------------------------------------------------------------------------------
/** The default events.*/
const BARATINOO_EVENT_TYPE BaratinooSwig::DEFAULT_EVENTS[BaratinooSwig::DEFAULT_EVENTS_LENGTH] = {
		BARATINOO_EVENT_TYPE::BARATINOO_MARKER_EVENT
		//, BARATINOO_EVENT_TYPE::BARATINOO_PHONEME_EVENT
		, BARATINOO_EVENT_TYPE::BARATINOO_VISEME_EVENT
};

BaratinooSwig::BaratinooSwig(const char *outputPath, const char *logFilename) :
_engine(NULL),
_configFile(NULL),
_outputPath(NULL)
//,_log(NULL)
, _logger(NULL)
, _logfile(NULL)
, _key(NULL)
{
	// init _event with DEFAULT_EVENTS
	for (int i=0; i<DEFAULT_EVENTS_LENGTH; ++i) {
		_events[ DEFAULT_EVENTS[i] ] = true;
	}

	// Output directory (for logfile & co.)
    if (NULL != outputPath && '\0' != *outputPath) {
    	// append a final '/' if necessary
    	size_t opLen = strlen(outputPath);
    	bool addSlash = outputPath[opLen-1] != '/';
    	_outputPath = new char[opLen + 1 + (addSlash?1:0)];
    	if (_outputPath == NULL) return;
    	if (addSlash) {
    		 memcpy(_outputPath, outputPath, opLen);
    		 _outputPath[opLen] = '/';
    		 _outputPath[opLen+1] = '\0';
    	} else memcpy(_outputPath, outputPath, opLen + 1);
    }

    //TODO: move it in a BaratinooTraceHelper method ?
	// Log file
    if ( NULL != _outputPath &&
    	NULL != logFilename && '\0' != *logFilename
        ) {
        _logfile = new  char[strlen(_outputPath) + strlen(logFilename) + 1];
        if (_logfile == NULL)
            return;
        sprintf(_logfile, "%s%s", _outputPath, logFilename);
        _logger = new BaratinooTraceHelper(_logfile);
    }

    // all application traces will be written in log file if specified (otherwise they will be discarded)
    if (_logger!=NULL) _logger->open();

    return;
}

//---------------------------------------------------------------------------------
BaratinooSwig::~BaratinooSwig() {
    if (_logger != NULL)
    	delete _logger;
    if (_logfile != NULL)
    	delete _logfile;
}


//---------------------------------------------------------------------------------
int BaratinooSwig::init(const char *configurationFile) {
	// make a copy of configurationFile
	if (NULL != configurationFile && '\0' != *configurationFile) {
	        _configFile = new  char[strlen(configurationFile) + 1];
	        if (_configFile == NULL)
	            return -1;
	        //sprintf(_configFile, "%s", configurationFile);
	        strcpy(_configFile, configurationFile);
	}
    // initialise TTS library : this must be done before you start creating a TTS engine.
    // it must be balanced with a call to baratinooTerminate when you're done with TTS library.
    // the only parameter is a pointer to a BaratinooTrace object (here the BaratinooSwig object itself)
    // which will receive textual feedback from Baratinoo library.
    //
    if (baratinooInit(_logger) != BARATINOO_INIT_OK) {
        traceError("library", "cannot initialize library");
        return -1;
    }

    //printf("Baratinoo - %s\n\n", getBaratinooVersion());
    traceInfo("BaratinooSwig", "Baratinoo - %s\n\n", getBaratinooVersion());


    // now we can create and initialise a TTS engine using baratinoo configuration file.
    // Once this is done, the engine shall be in INITIALIZED state. If it's not, check the traces to see what went wrong.
    //
    _engine = BaratinooEngine::newInstance();
    if (_engine == NULL) {
        shutdown();
        traceError("library", "cannot create engine");
        return -1;
    }
    if (_engine->init(_configFile) != BaratinooEngine::INITIALIZED) {
        shutdown();
        traceError("library", "cannot initialise engine");
        return -1;
    }
    // We tell the engine we're not interested in receiving any event for the moment.
    _engine->unsetAllWantedEvent();
    // ... except the ones in _events
    for (int ie=BARATINOO_EVENT_TYPE::BARATINOO_MARKER_EVENT;
    		ie<BARATINOO_EVENT_TYPE::BARATINOO_NUMBER_OF_EVENT; ++ie)
    {
    	BARATINOO_EVENT_TYPE e = static_cast<BARATINOO_EVENT_TYPE>(ie);
    	if (_events[e]) _engine->setWantedEvent(e);
    }

    //TMP: display available voices
    for (int i = 0; i<_engine->getNumberOfVoices(); i++) {
    	BaratinooVoiceInfo voiceInfo = _engine->getVoiceInfo(i);
        traceInfo("BaratinooSwig", "Voice #%d: %s %s %s\n", i, voiceInfo.name, voiceInfo.language, voiceInfo.gender);
        //TODO: store it/make a method to access them
    }

    return 0;
}

//---------------------------------------------------------------------------------
void BaratinooSwig::shutdown() {
    if (_engine != NULL) {
        /* first we delete the engine */
        BaratinooEngine::deleteInstance(_engine);
        _engine = NULL;
    }
    /* then we call baratinooTerminate to tell we're done with the TTS library */
    baratinooTerminate();
}

//---------------------------------------------------------------------------------
BaratinooEngine::BARATINOO_STATE BaratinooSwig::reset(BaratinooEngine::BARATINOO_STATE state) {
        if (BaratinooEngine::ENGINE_ERROR == state) {
            // try to recreate engine if an error occured
            shutdown();
            init(_configFile);
            if (_engine != NULL)
                state = _engine->getState();
        } else if (BaratinooEngine::INPUT_ERROR == state) {    // just need to purge the engine if input was not valid
            state = _engine->purge();
        }
	return state;
}

//---------------------------------------------------------------------------------
static const char* EVENT_NAMES[BARATINOO_EVENT_TYPE::BARATINOO_NUMBER_OF_EVENT+1] = {"Marker", "WaitMarker", "Paragraphe", "Sentence", "Word", "Punctuation", "Separator", "Syllabe", "Phoneme", "Viseme", "Silence", "NewVoice", "Raw", "--NUMBER_OF_EVENT--"};//TMP
//int BaratinooSwig::textToSpeech(InputText *pInputText, OutputSignal *pOutputSignal)
SpeechResult * BaratinooSwig::textToSpeech(const byte* text) {
    BaratinooEngine::BARATINOO_STATE state;

    // Create the InputText
	InputTextBuffer *pInputText = InputTextBuffer::newInstance(_parsing, _encoding);
    if (!pInputText->init(text)) {
        traceError("textToSpeech", "InputTextBuffer::init() error.\n");
        return NULL;
    }
    // set engine input : tell the engine where to get its input.
    if (_engine->setInput(pInputText) != BaratinooEngine::READY) {
        shutdown();
        traceError("library", "cannot set engine input");
        return NULL;
    }

    // Create the OutputSignal
	OutputSignalBuffer *pOutputSignal = OutputSignalBuffer::newInstance(_coding, _frequency);
    // set engine output : tell the engine what to do with the sound signal it generates
    _engine->setOutput(pOutputSignal);

    // Create the SpeechResult
    SpeechResult *result = new SpeechResult(pOutputSignal, _format, _logger);
    //TODO: attach marked events

    // main loop : keep calling processLoop until either finished or an error occurs
    do {
    	/* calling processLoop with -1 means it will only return after one of the following happens :
         - a registered event is fired
         - input has been fully processed
         - an error occurs

         we could have passed a positive integer value instead of -1 (say, 100 for example), which would have caused the processLoop method
         to make no more than 100 steps before returning (so you may allow user to cancel the process for example...)
    	 */
    	state = _engine->processLoop(-1);
    	if (BaratinooEngine::EVENT == state) {        // a TTS event occured : here we decided to display only 'word' events
    		const BaratinooEvent &ttsEvent = _engine->getEvent();
    		/*demo--	if (BARATINOO_WORD_EVENT == ttsEvent.type) {
                traceInfo("BaratinooSwig", "processing word : %s", ttsEvent.data.word.input);
            } else */
    		TimeStampEvent *tse = NULL;
    		switch (ttsEvent.type) {
    		// Marker event => push in _markerEvents
    		case BARATINOO_MARKER_EVENT :
    			tse = new TimeStampEvent(ttsEvent);
    			//traceInfo("BaratinooSwig", "marker name : %s; timestamp: %8.2fms", ttsEvent.data.marker.name, ttsEvent.timeStamp);
    			traceDebug("BaratinooSwig", "[%8.2fms] Marker name : %s", tse->millisecond, tse->name);
    			result->_markerEvents.push_back(tse); // push to markerEvents
    			break;
    		// Continuous events :
			case BARATINOO_WAITMARKER_EVENT:
			case BARATINOO_PHONEME_EVENT:
			case BARATINOO_VISEME_EVENT:
				tse = new ContinuousEvent(ttsEvent);
    			traceDebug("BaratinooSwig", "[%8.2fms] Continuous event name : %s; duration: %.2fms", tse->millisecond, tse->name, ((ContinuousEvent *) tse)->duration);
    			if (ttsEvent.type == BARATINOO_VISEME_EVENT) result->_visemeEvents.push_back((ContinuousEvent *) tse); // collect visemes
				break;
			default:
				tse = new TimeStampEvent(ttsEvent);
    			traceDebug("baratinooSwig", "[%8.2fms] Other events name : %s", tse->millisecond, tse->name);
				break;
    		}
    		if (tse!=NULL) {
    			traceDebug("BaratinooSwig", "Push %s event into allEvents (name : %s; timestamp: %.2fms)", EVENT_NAMES[tse->type], tse->name, tse->millisecond);
    			result->_allEvents.push_back(tse); // push to all events
    		}
    	}
    } while (state == BaratinooEngine::RUNNING || state == BaratinooEngine::EVENT);

    // if an error occured, reset engine state
    if (state != BaratinooEngine::INITIALIZED) {
    	state = reset(state);
        if (state != BaratinooEngine::INITIALIZED) {
            traceError("library", "cannot restart BaratinooEngine");
            return NULL; //TODO: error result
        }
        return NULL; //TODO: error result
    }
    //else

    // delete the InputText(Buffer)
    InputTextBuffer::deleteInstance(pInputText);

    // Show the all events
    if (true) {
    	if (int nbEvents = result->getAllEventsSize()) {
    		traceDebug("baratinooSwig", "%d collected events:", nbEvents);
    		/*BUG with MSDN: the test (itEvent != result->getAllEvents().end()) failed
    		for (std::vector<TimeStampEvent *>::const_iterator itEvent = result->getAllEvents().begin(); itEvent != result->getAllEvents().end(); ++itEvent)
    		{
    			traceDebug("baratinooSwig", "[");
    			traceDebug("baratinooSwig", "\t[%8.2fms] name:%s (type:%s)", (*itEvent)->millisecond, (*itEvent)->name.c_str(), EVENT_NAMES[(*itEvent)->type]);
    			//traceDebug("baratinooSwig", "\t[%8.2fms] name:%s (type:%s)\n", (*itEvent)->millisecond, (*itEvent)->name, EVENT_NAMES[(*itEvent)->type]);
    			traceDebug("baratinooSwig", "]");
    		}*/
    		for (const auto &event : result->getAllEvents()) {
    			//traceDebug("baratinooSwig", "[");
    			traceDebug("baratinooSwig", "\t[%8.2fms] name:%s (type:%s)", event->millisecond, event->name.c_str(), EVENT_NAMES[event->type]);
    			//traceDebug("baratinooSwig", "]");
    		}
    		traceDebug("baratinooSwig", "---");
    		if (true) {
    			if (int nbMarkers = result->getMarkerEventsSize()) {
    				traceDebug("baratinooSwig", "%d collected markers:", nbMarkers);
    				for (const auto &event : result->getMarkerEvents()) {
    					//traceDebug("baratinooSwig", "[");
    					traceDebug("baratinooSwig", "\t[%8.2fms] Marker id:%s (type:%s)", event->millisecond, event->name.c_str(), EVENT_NAMES[event->type]);
    					//traceDebug("baratinooSwig", "]");
    				}
    			}
    			traceDebug("baratinooSwig", "---");
    		}
    	} else {
    		traceDebug("baratinooSwig", "Any collected events.\n");
    	}
    }


    return result;
}





//---------------------------------------------------------------------------------


void  BaratinooSwig::setWantedEvent(const BARATINOO_EVENT_TYPE event, const bool value) {
	_events[event] = value;
	if (_engine!=NULL) {
		if (value)	_engine->setWantedEvent(event);
		else	_engine->unsetWantedEvent(event);
	}
}

void  BaratinooSwig::setAllWantedEvent() {
	memset(_events, true, sizeof(bool) * BARATINOO_EVENT_TYPE::BARATINOO_NUMBER_OF_EVENT);
	if (_engine!=NULL) {
		_engine->setAllWantedEvent();
	}
}

void  BaratinooSwig::unsetAllWantedEvent() {
	memset(_events, false, sizeof(bool) * BARATINOO_EVENT_TYPE::BARATINOO_NUMBER_OF_EVENT);
	if (_engine!=NULL) {
		_engine->unsetAllWantedEvent();
	}
}

//------------------------------------------------------------------

void BaratinooSwig::traceError(const char *source, const char *format, ...)
{
	if (_logger!=NULL) {
		va_list args;
		va_start(args, format);
		_logger->trace(BaratinooTrace::TRACE_ERROR, source, format, args);
		va_end(args);
	}
}

void BaratinooSwig::traceWarning(const char *source, const char *format, ...)
{
	if (_logger!=NULL) {
		va_list args;
		va_start(args, format);
		_logger->trace(BaratinooTrace::TRACE_WARNING, source, format, args);
		va_end(args);
	}
}

void BaratinooSwig::traceInfo(const char *source, const char *format, ...)
{
	if (_logger!=NULL) {
		va_list args;
		va_start(args, format);
		_logger->trace(BaratinooTrace::TRACE_INFO, source, format, args);
		va_end(args);
	}
}

void BaratinooSwig::setLogLevel(LogLevel level) {
	if (_logger!=NULL) {
		_logger->setTraceLevel((BaratinooTrace::TraceLevel) level);
	}
}

LogLevel BaratinooSwig::getLogLevel() {
	if (_logger!=NULL) {
		return (LogLevel) _logger->getTraceLevel();
	} else return LogLevel::ERROR;
}

void BaratinooSwig::traceDebug(const char *source, const char *format, ...)
{
	if (_logger!=NULL) {
		va_list args;
		va_start(args, format);
		_logger->trace(BaratinooTrace::TRACE_DEBUG, source, format, args);
		va_end(args);
	}
}
