/* $Id: barademo_C.c 28171 2017-03-02 14:23:06Z paul $ -*- coding: utf-8-unix -*- */

/* (c) Voxygen 2016
 *
 * barademo_C : simple example of Baratinoo C Api usage.
 *
 *     This example synthesizes the content of the text file texte.txt with first voice of the configuration
 *     and produce an audio file barademo_C.wav.
 *     Baratinoo traces are written in the log file barademo_C.log.
 */
 
#include <stdio.h>
#include <string.h>

#define CFG_FILE      "../../config/baratinoo.cfg"
#define LOG_FILE      "barademo_C.log"
#define WAV_FILE      "barademo_C.wav"
#define TXT_FILE      "texte.txt"

#define DEMO_TEXT     "Le centre du système solaire est indéniablement excentré."

/* baratinoo.h contains both C and C++ API. 
 * to use C API you have to define BARATINOO_C_API before including this header.
 */
#define BARATINOO_C_API
#include "baratinoo.h"
#include "baratinooio.h"


static void traceCB(BaratinooTraceLevel level, int engineNumber, const char *source, const void *privatedata, const char *format, va_list args);
static const char *keyCB(int index);

int runDemo();


/* Function main() */
int main(int argc, char*argv[])
{
    /* truncate log file */
    FILE *ptf;
    ptf=fopen(LOG_FILE, "w+");
    if (ptf)
      fclose(ptf);

    runDemo();
    return 0;
}


int runDemo()
{
    int ret;
    int i;
    int stopFlag = 0;
    BCengine engine = 0;
    BCinputTextBuffer input = NULL;
    BCoutputSignalBuffer output = NULL;
    BARATINOOC_STATE state;
    BaratinooEvent ttsEvent;
    FILE *f = NULL;
    char text[10001];

    /* BCinitlib(): Library initialisation. The argument passed to the
     *   function is a callback which is called for each message the library sends out.
     *   If parameter is NULL, no message will be avaible.
     * See traceCB comment for more details
     */
    ret = BCinitlib(traceCB);
    if(ret != BARATINOO_INIT_OK) {
        fprintf(stderr, "BCinitlib() returned %d\n", ret);
        return(1);
    }
    printf("Baratinoo - %s\n\n", BCgetBaratinooVersion());

    fprintf(stderr, "Logs BARATINOO are stored in file '%s'\n", LOG_FILE);
    
    /* BCnew() and BCinit() : Engine creation and configuration via a 
     *  .cfg file. The path to this file maybe be relative to current directory.
     * BCgetState() returns the actual state of the engine (see BARATINOOC_STATE definition in baratinoo.h)
     * After initialisation the BARATINOO_INITIALIZED state is expected.
     * Otherwise information about the error is made available via trace callback.
     */
    engine = BCnew(NULL);
    if(!engine) {
        fprintf(stderr, "Cannot allocate BCengine\n");
        goto error;
    }
    
    BCinit(engine, CFG_FILE);
    state = BCgetState(engine); 
    if(state != BARATINOO_INITIALIZED) {
        fprintf(stderr, "Cannot init Baratinoo Engine\n");
        goto error;
    }

    /* BCgetNumberOfVoices() and BCgetVoiceInfo(): Informations about loaded voices */
    for(i=0; i<BCgetNumberOfVoices(engine); i++) {
        printf("Voice #%d: %s %s %s\n", i, BCgetVoiceInfo(engine, i).name, BCgetVoiceInfo(engine, i).language, BCgetVoiceInfo(engine, i).gender);
    }
    printf("\n");


    /* BCinputTextBufferNew() and BCinputTextBufferInit() : Creation and initialisation of 
     *  an inputTextBuffer object with text to read.
     */
    input = BCinputTextBufferNew(BARATINOO_ALL_PARSING, BARATINOO_UTF8, 0, 0);
    if (!input) {
        fprintf(stderr, "cannot allocate BCinputTextBuffer\n");
        goto error;
    }

    /* Read text to synthesize into a static buffer for simplicity */
    f = fopen(TXT_FILE, "rb");
    if (f) {
        size_t read = fread((void*)text, sizeof(char), 10000, f);
        if (read > 0) {
            text[read] = 0; /* end string with a NUL */
        } else {
            fprintf(stderr, "Error while reading input text '%s'\n" , TXT_FILE);
        }
        fclose(f);
    } else {
        fprintf(stderr, "Cannot read input text file '%s'\nUses internal demo text\n" , TXT_FILE);
        strncpy(text, DEMO_TEXT, strlen(DEMO_TEXT));
        text[strlen(DEMO_TEXT)] = 0;
    }

    ret = BCinputTextBufferInit(input, text);
    if (!ret) {
        fprintf(stderr, "cannot init input\n");
        goto error;
    }

    /* BCinputTextBufferSetInEngine() : Set the inputTextBuffer as the engine input.
     * Alternatively you could also use :
     * BCsetInputText() : Set the input callback which will be called by the engine
     * to get input text. Arguments of BCsetInputText() :
     *   #1 : engine instance (= value returned by BCnew())
     *   #2 : input text callback
     *   #3 : private parameter which will be passed to the callback
     *   #4 : kind of parsing the engine will apply on the input text.
     *        see BARATINOO_PARSING definition in baratinoo.h and CVOX31/CVOX32 documents
     *   #5 : input text encoding. See BARATINOO_TEXT_ENCODING definition in baratinoo.h
     *   #6 : index of the voice to use as default voice; range is [0;BCgetNumberOfVoices(engine)-1]
     *   #7 : 0
     *   #8 : 0
     * The new engine state is returned by this function and shall be BARATINOO_READY.
     */
    state = BCinputTextBufferSetInEngine(input, engine);
    if (state != BARATINOO_READY) {
        fprintf(stderr, "Cannot set input\n");
        goto error;
    }

    /* BCoutputSignalBufferNew() : Creation of an outputSignalBuffer object which will receive generated sound signal. */
    output = BCoutputSignalBufferNew(BARATINOO_PCM, 24000);
    if (!output) {
        fprintf(stderr, "Cannot allocate BCoutputSignalBufferNew\n");
        goto error;
    }

    /* BCoutputTextBufferSetInEngine() : Set the outputSignalBuffer as the engine output.
     * Alternatively you could also use :
     * BCsetOutputSignal() : Set the output signal callback which will be called by the engine
     * to receive sound signal data. Arguments of BCsetOutputSignal() :
     *   #1 : engine instance (= value returned by BCnew())
     *   #2 : output signal callback
     *   #3 : private parameter which will be passed to the callback
     *   #4 : desired signal encoding. See BARATINOO_SIGNAL_CODING definition in baratinoo.h
     *   #5 : desired signal sampling rate in range [6000;48000] Hz
     */
    BCoutputTextBufferSetInEngine(output, engine);

    /* BCprocessLoop() : Main processing loop.
     *  The first parameter is the engine instance, 
     *  The second one (100) means we want to interrupt the TTS process every 100 steps.
     *   In this case BCprocessLoop will return after making 100 processing steps, which allows you to make
     *   other tasks while processing is being run.
     *
     *   Set -1 if you don't want BCprocessLoop() to return until the input text has
     *   been fully processed and the whole signal has been generated, or an event occurs.
     *
     * Engine will call the output callback each time a buffer of signal will be available.
     * After all input text has been processed, engine state shall be : BARATINOO_INITIALIZED
     *
     * An external thread can set the variable stopFlag to exit from the processing loop
     */
    stopFlag = 0;
     
    do {
        state = BCprocessLoop(engine, 100);
        if (state == BARATINOO_EVENT) {
            /* Call BCgetEvent() to retrieve the event occurred. See BARATINOO_EVENT_TYPE in baratinoo.h for all the event types. */
            ttsEvent = BCgetEvent(engine);
            fprintf(stderr, "Receive an event\n");
        }
            
    } while (((state == BARATINOO_RUNNING) && (stopFlag == 0))  ||  (state == BARATINOO_EVENT)) ;

    if (state == BARATINOO_INPUT_ERROR) {
        fprintf(stderr, "Bad input text generates a parsing error\n");
        goto error;
    }
    if (state == BARATINOO_RUNNING) {
        /* Processing was stopped. You must call BCpurge() in order to reuse this engine. */
        BCpurge(engine);
        fprintf(stderr, "Processing was stopped by user\n");
    }
    if (state != BARATINOO_INITIALIZED) {
        /* Unexpected state: you must destroy this engine. */
        fprintf(stderr, "Error while processing text\n");
        goto error;
    }
    

    /* BCdelete() : Engine destruction.
     *  you could also use this engine again after making another call to BCinputTextBufferSetInEngine 
     *  or BCsetInputText to put the engine again in BARATINOO_READY state.
     */
    BCdelete(engine);

    
    /* BCterminatelib() : Library termination.
     *  Every BCinitLib call must be matched by a call to BCterminateLib.
     */
    BCterminatelib();

    /* BCinputTextBufferDelete() : Delete input buffer */
    BCinputTextBufferDelete(input);

    f = fopen(WAV_FILE, "wb");
    if (f != NULL) {
        char *pHeader = NULL;
        int headerLength;
        headerLength = BCoutputSignalBufferGetSignalHeader(output, OUTPUTSIGNALBUFFER_WAV_HEADER, &pHeader);
        if (headerLength <= 0) {
            fprintf(stderr, "Error while getting signal header\n");
        } else {
            fwrite(pHeader, headerLength, sizeof(char), f);
            BCoutputSignalBufferDeleteSignalHeader(pHeader);
            fwrite(BCoutputSignalBufferGetSignalBuffer(output), BCoutputSignalBufferGetSignalLength(output), sizeof(char), f);
            fprintf(stderr, "\nSignal was stored in WAV file '%s'\n", WAV_FILE);
        }
        fclose(f);
    } else {
        fprintf(stderr, "Error while opening WAV file '%s'\n", WAV_FILE);
    }

    /* BCinputTextBufferDelete() : Delete output buffer */
    BCoutputSignalBufferDelete(output);
    return(0);

 error:
    if(engine) {
        BCdelete(engine);
    }
    if (input) {
        BCinputTextBufferDelete(input);
    }
    if (output) {
        BCoutputSignalBufferDelete(output);
    }
    BCterminatelib();
    return(1);
}



/* Trace callback
 *   level : trace level [error, init, warning, info, debug]. See baratinoo.h
 *   engineNumber : 0 if trace comes from baratinoo library itself, otherwise the number of the engine which generated the trace (use BCgetEngineNumber(engine) to get it)
 *   format et args : trace message format and arguments (as used by vprintf function)
 *
 * It's up to your application to device what to do with those traces, no filtering is done by baratinoo library.
 */
void traceCB(BaratinooTraceLevel level, int engineNumber, const char *source, const void *privatedata, const char *format, va_list args)
{
    FILE *ptf;
    const char *prefix = "";
    (void)engineNumber;

    switch(level) {
    case BARATINOO_TRACE_ERROR:
        prefix = "ERROR";
        break;
    case BARATINOO_TRACE_INIT:
        prefix = "INIT";
        break;
    case BARATINOO_TRACE_WARNING:
        prefix = "WARNING";
        break;
    case BARATINOO_TRACE_INFO:
        prefix = "INFO";
        break;
    case BARATINOO_TRACE_DEBUG:
        prefix = "DEBUG";
        break;
    }

    ptf=fopen(LOG_FILE, "a");
    fprintf(ptf, "%s : %s ", prefix, source);
    vfprintf(ptf, format, args);
    fprintf(ptf, "\n");
    fflush(ptf);
    fclose(ptf);
}

#if 0
/* Callback which return the shared secret key (specific use case)
 * Use with BCinitlibWithKey()
 */
static const char *keyCB(int index)
{
  return (0 == index ? "shared_secret_key_provided_by_Voxygen" : NULL);
}
#endif

#if 0
/* Customized input / output sample callbacks */

/* Input callback
 *   privateData : private data which were passed as 3rd parameter of BCsetInputText()
 *   address and length : buffer into which the callback shall write the input text (up to length bytes)
 *
 *   return value : number of bytes really written
 *            returning 0 means there is no more text to process.
 *            That will end the TTS processing and the callback won't be called anymore
 */
int inCB(void *privateData, void *address, int length)
{
    /* we could have used the privateData field, here we used a static variable for simplicity */
    static char *src = DEMO_TEXT;
    char *dst = (char*)address;
    int len = 0;
    (void)privateData;

    /* copies up to 'length' bytes of 'src' string to 'address' buffer */
    while(length && *src) {
        *dst++ = *src++;
        len++;
    }
    return(len);
}


/* Output callback
 *   privateData : private data which were passed as 3rd parameter of BCsetOutputSignal()
 *   address and length : buffer which contains generated sound signal (length bytes).
 *
 *   return value : must be 0
 */
int outCB(void *privateData, const void *address, int length)
{
    
    /* we could have used the privateData field, here we used a static variable for simplicity */
    static FILE *f = 0;
    if(!f) {
        f = fopen(WAV_FILE, "wb");
    }
    (void)privateData;

    /* write 'length' bytes into output file */
    fwrite(address, length, 1, f);

    return(0);
}

#endif



/* State diagram                                                                  */
/* ~~~~~~~~~~~~~                                                                  */
/*   Here are the 7 possible states of a BaratinooEngine:                         */
/*     o UNINITIALIZED. This state is the state of a BaratinooEngine after        */
/*       creation. The only allowed function call in this state is init().        */
/*     o ERROR. This state is reach when a fatal error occurs. Probably because   */
/*       of lack of memory. More detail is given in an ERROR trace. In that case, */
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
