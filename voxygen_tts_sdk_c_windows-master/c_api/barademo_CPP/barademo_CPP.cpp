/* $Id: barademo_CPP.cpp 26916 2016-05-16 07:34:42Z pierre-yves.jolivet $ -*- coding: utf-8-unix -*- */

/* (c) Voxygen 2016
 *
 * barademo_CPP : simple example of Baratinoo C++ Api usage.
 *
 *     This example synthesizes the content of the text file texte.txt with first voice of the configuration
 *     and produce an audio file barademo_CPP.wav.
 *     Baratinoo traces are written in the log file barademo_CPP.log.
 */

#include "barademo_CPP.h"
#include <string.h>


/*==================================================================================================*/
/*====================================================================================================*/
int main(int argc, char** argv) {
    BaratinooDemo *demo = new BaratinooDemo(OUTPUT_PATH, OUTPUT_LOG_FILE, OUTPUT_WAV_FILE);

    fprintf(stderr, "Logs BARATINOO are stored in file '%s'\n\n", OUTPUT_LOG_FILE);

    if (0 == demo->init(CFG_FILE)) {
        demo->run();
        demo->shutdown();
    } else {
        fprintf(stderr, "Initialisation failed\n");
    }

    delete demo;

    return 0;
}


//---------------------------------------------------------------------------------
BaratinooDemo::BaratinooDemo(const char *outputPath, const char *logFile, const char *soundfile) :
_engine(NULL),
_logfile(NULL),
_soundfile(NULL),
_log(NULL) {
    if (NULL != logFile && '\0' != *logFile) {
        _logfile = new  char[strlen(outputPath) + strlen(logFile) + 1];
        if (_logfile == NULL)
            return;
        sprintf(_logfile, "%s%s", outputPath, logFile);
    }

    if (NULL != soundfile && '\0' != *soundfile) {
        _soundfile = new  char[strlen(outputPath) + strlen(soundfile) + 1];
        if (_soundfile == NULL)
            return;
        sprintf(_soundfile, "%s%s", outputPath, soundfile);
    }

    // all application traces will be written in log file if specified (otherwise they will be discarded)
    if (NULL != _logfile)
        _log = fopen(_logfile, "w+");

    return;
}

//---------------------------------------------------------------------------------
BaratinooDemo::~BaratinooDemo() {
    if (_log != NULL)
        fclose(_log);

    if (_logfile != NULL) {
        delete[] _logfile;
    }

    if (_soundfile != NULL) {
        delete[] _soundfile;
    }
}


//---------------------------------------------------------------------------------
int BaratinooDemo::init(const char *configurationFile) {
    int i;
    /* initialise TTS library : this must be done before you start creating a TTS engine.
     * it must be balanced with a call to baratinooTerminate when you're done with TTS library.
     * the only parameter is a pointer to a BaratinooTrace object (here the BaratinooDemo object itself) 
     * which will receive textual feedback from Baratinoo library.
     */
    if (baratinooInit(this) != BARATINOO_INIT_OK) {
        traceError("library", "cannot initialize library");
        return -1;
    }

    printf("Baratinoo - %s\n\n", getBaratinooVersion());


    /* now we can create and initialise a TTS engine using baratinoo configuration file.
     * Once this is done, the engine shall be in INITIALIZED state. If it's not, check the traces to see what went wrong.
     */
    _engine = BaratinooEngine::newInstance();
    if (_engine == NULL) {
        shutdown();
        traceError("library", "cannot create engine");
        return -1;
    }
    if (_engine->init(configurationFile) != BaratinooEngine::INITIALIZED) {
        shutdown();
        traceError("library", "cannot initialise engine");
        return -1;
    }
    /* we tell the engine we're not interested in receiving any event for the moment. */
    _engine->unsetAllWantedEvent();

    /* display available voices */
    for (i = 0; i<_engine->getNumberOfVoices(); i++) {
        traceInfo("BaratinooDemo", "Voice #%d: %s %s %s\n", i, _engine->getVoiceInfo(i).name, _engine->getVoiceInfo(i).language, _engine->getVoiceInfo(i).gender);
    }

    return 0;
}

//---------------------------------------------------------------------------------
void BaratinooDemo::shutdown() {
    if (_engine != NULL) {
        /* first we delete the engine */
        BaratinooEngine::deleteInstance(_engine);
        _engine = NULL;
    }
    /* then we call baratinooTerminate to tell we're done with the TTS library */
    baratinooTerminate();
}

//---------------------------------------------------------------------------------
int BaratinooDemo::textToSpeech(InputText *pInputText, OutputSignal *pOutputSignal) {
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
            if (BARATINOO_WORD_EVENT == ttsEvent.type) {
                traceInfo("BaratinooDemo", "processing word : %s", ttsEvent.data.word.input);
            }
        }
    } while (state == BaratinooEngine::RUNNING || state == BaratinooEngine::EVENT);

    // if an error occured, reset engine state
    if (state != BaratinooEngine::INITIALIZED) {
        if (BaratinooEngine::ENGINE_ERROR == state) {
            // try to recreate engine if an error occured
            shutdown();
            init(CFG_FILE);
            if (_engine != NULL)
                state = _engine->getState();
        } else if (BaratinooEngine::INPUT_ERROR == state) {    // just need to purge the engine if input was not valid
            state = _engine->purge();
        }
        if (state != BaratinooEngine::INITIALIZED) {
            traceError("library", "cannot restart BaratinooEngine");
        }
        return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------------
int BaratinooDemo::run() {
    InputTextBuffer inputTextBuffer(BARATINOO_ALL_PARSING, BARATINOO_UTF8);
    OutputSignalBuffer outputSignalBuffer(BARATINOO_PCM, OUTPUT_FREQUENCY);
    SampleOutputSignal sampleOutputSignal(BARATINOO_PCM, OUTPUT_FREQUENCY, this);
    char Buf[4000];
    memset(Buf, 0, 4000);
    FILE *f;

    // First synthesis using helper classes InputTextBuffer and OutputSignalBuffer
    // initialise input object
    f = fopen(TXT_FILE, "rb");
    if (f != NULL) {
        fread(Buf, 4000, sizeof(char), f);
        fclose(f);
    } else {
        strcpy(Buf, "No text file found. Using default text.");
    }
    traceInfo("BaratinooDemo", "Text to be synthesized : %s", Buf);
    if (!inputTextBuffer.init(Buf)) {
        traceError("library", "InputTextBuffer::init() error.\n");
        return -1;
    }

    // synthesize
    if (0 == textToSpeech(&inputTextBuffer, &outputSignalBuffer)) {
        // synthesis ok : retrieve sound data from OutputSignalBuffer.
        if (NULL != _soundfile) {
            f = fopen(_soundfile, "wb");
            if (f != NULL) {
                char *pHeader = NULL;
                int headerLength;
                headerLength = outputSignalBuffer.getSignalHeader(OutputSignalBuffer::WAV_HEADER, &pHeader);
                if (headerLength <= 0) {
                    traceError("BaratinooDemo", "couldn't retrieve generated signal wav header\n");
                } else {
                    fwrite(pHeader, headerLength, sizeof(char), f);
                    outputSignalBuffer.deleteSignalHeader(pHeader);
                    fwrite(outputSignalBuffer.getSignalBuffer(), outputSignalBuffer.getSignalLength(), sizeof(char), f);
                }
                fclose(f);
            }
        }
    }
    traceInfo("BaratinooDemo", "First synthesis done.");
    
    fprintf(stderr, "Signal was stored in WAV file '%s'\n", OUTPUT_WAV_FILE);

    // We can also implement our own OutputSignal object to play the audio as soon as it is being synthesised instead of waiting for whole text to be processed.
    // the SampleOutputSignal::writeSignal will be called several times during synthesis process.
    traceInfo("BaratinooDemo", "Running synthesis using SampleOutputSignal (audio is not stored).");
    _engine->setWantedEvent(BARATINOO_WORD_EVENT);
    inputTextBuffer.init("Here is another sample text.");
    textToSpeech(&inputTextBuffer, &sampleOutputSignal);

    return 0;
}  // end run




//---------------------------------------------------------------------------------
void BaratinooDemo::traceError(const char *source, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    write(BaratinooTrace::TRACE_ERROR, 0, source, format, args);
    va_end(args);
}
//---------------------------------------------------------------------------------
void BaratinooDemo::traceInfo(const char *source, const char *format, ...)
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
void BaratinooDemo::write(TraceLevel level, int engineNumber, const char *source, const char *format, va_list args)
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



//---------------------------------------------------------------------------------
/* Signal callback implementation from OutputSignal 
 * Here is the place to use if you want to process audio signal on-the-fly
 */
int BaratinooDemo::SampleOutputSignal::writeSignal(const void *address, int length) {
    
    _baratinooDemo->traceInfo("SampleOutputSignal", "Received signal part : %d bytes", length);
    return 0;        // return 1 to stop synthesis
}

