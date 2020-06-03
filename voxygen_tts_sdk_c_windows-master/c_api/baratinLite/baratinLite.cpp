/*
 * A little C++ interface for the Voxigen baratinoo library.
 * Based on the barademo_CPP example.
 *
 *  Created on: 5 avr. 2017
 *      Author: montcheuil
 */

#include "baratinLite.h"
#include <string.h>


//---------------------------------------------------------------------------------
BaratinooLite::BaratinooLite(const char *outputPath, const char *logFile) :
_engine(NULL),
_configFile(NULL),
_logfile(NULL),
_log(NULL) {
    if (NULL != logFile && '\0' != *logFile) {
        _logfile = new  char[strlen(outputPath) + strlen(logFile) + 1];
        if (_logfile == NULL)
            return;
        sprintf(_logfile, "%s%s", outputPath, logFile);
    }

    // all application traces will be written in log file if specified (otherwise they will be discarded)
    if (NULL != _logfile)
        _log = fopen(_logfile, "w+");

    return;
}

//---------------------------------------------------------------------------------
BaratinooLite::~BaratinooLite() {
    if (_log != NULL)
        fclose(_log);

    if (_logfile != NULL) {
        delete[] _logfile;
    }
}


//---------------------------------------------------------------------------------
int BaratinooLite::init(const char *configurationFile) {
	if (NULL != configurationFile && '\0' != *configurationFile) {
	        _configFile = new  char[strlen(configurationFile) + 1];
	        if (_configFile == NULL)
	            return -1;
	        //sprintf(_configFile, "%s", configurationFile);
	        strcpy(_configFile, configurationFile);
	}
    int i;
    /* initialise TTS library : this must be done before you start creating a TTS engine.
     * it must be balanced with a call to baratinooTerminate when you're done with TTS library.
     * the only parameter is a pointer to a BaratinooTrace object (here the BaratinooLite object itself)
     * which will receive textual feedback from Baratinoo library.
     */
    if (baratinooInit(this) != BARATINOO_INIT_OK) {
        traceError("library", "cannot initialize library");
        return -1;
    }

    //printf("Baratinoo - %s\n\n", getBaratinooVersion());
    traceInfo("BaratinooLite", "Baratinoo - %s\n\n", getBaratinooVersion());


    /* now we can create and initialise a TTS engine using baratinoo configuration file.
     * Once this is done, the engine shall be in INITIALIZED state. If it's not, check the traces to see what went wrong.
     */
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
    /* we tell the engine we're not interested in receiving any event for the moment. */
    _engine->unsetAllWantedEvent();
    _engine->setWantedEvent(BARATINOO_MARKER_EVENT);// Except the <mark /> events

    /* display available voices */
    for (i = 0; i<_engine->getNumberOfVoices(); i++) {
        traceInfo("BaratinooLite", "Voice #%d: %s %s %s\n", i, _engine->getVoiceInfo(i).name, _engine->getVoiceInfo(i).language, _engine->getVoiceInfo(i).gender);
	//TODO: store it/make a method to access them
    }

    return 0;
}

//---------------------------------------------------------------------------------
void BaratinooLite::shutdown() {
    if (_engine != NULL) {
        /* first we delete the engine */
        BaratinooEngine::deleteInstance(_engine);
        _engine = NULL;
    }
    /* then we call baratinooTerminate to tell we're done with the TTS library */
    baratinooTerminate();
}

//---------------------------------------------------------------------------------
BaratinooEngine::BARATINOO_STATE BaratinooLite::reset(BaratinooEngine::BARATINOO_STATE state) {
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
int BaratinooLite::textToSpeech(InputText *pInputText, OutputSignal *pOutputSignal) {
    BaratinooEngine::BARATINOO_STATE state;

    // set engine input : tell the engine where to get its input.
    if (_engine->setInput(pInputText) != BaratinooEngine::READY) {
        shutdown();
        traceError("library", "cannot set engine input");
        return -1;
    }

    // set engine output : tell the engine what to do with the sound signal it generates
    _engine->setOutput(pOutputSignal);

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
                traceInfo("BaratinooLite", "processing word : %s", ttsEvent.data.word.input);
            } else */
	    if (BARATINOO_MARKER_EVENT == ttsEvent.type) {
                traceInfo("BaratinooLite", "marker name : %s; timestamp: %8.2fms", ttsEvent.data.marker.name, ttsEvent.timeStamp);
		//TODO: store info into a map
	    }
        }
    } while (state == BaratinooEngine::RUNNING || state == BaratinooEngine::EVENT);

    // if an error occured, reset engine state
    if (state != BaratinooEngine::INITIALIZED) {
	state = reset(state);
        if (state != BaratinooEngine::INITIALIZED) {
            traceError("library", "cannot restart BaratinooEngine");
            return -1;
        }
        return -1;
    }

    return 0;
}





//---------------------------------------------------------------------------------
void BaratinooLite::traceError(const char *source, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    write(BaratinooTrace::TRACE_ERROR, 0, source, format, args);
    va_end(args);
}
//---------------------------------------------------------------------------------
void BaratinooLite::traceInfo(const char *source, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    write(BaratinooTrace::TRACE_INFO, 0, source, format, args);
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
void BaratinooLite::write(TraceLevel level, int engineNumber, const char *source, const char *format, va_list args)
{
    if (_log == NULL)
        return;

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
