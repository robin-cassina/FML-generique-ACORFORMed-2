/* $Id: baratinooio.h 27655 2016-10-08 13:27:33Z paul $ */

/*  public interface for BARATINOO inputTextBuffer and outputSignalBuffer objects */

#ifndef _BARATINOOIO_H
#define _BARATINOOIO_H

#include "baratinoo.h"



/* ==============================================================================
 *  BaratinooEngine objects take their input from an input object.
 *  InputTextBuffer is an implementation of the pure virtual InputText class.
 *  This implementation have a init() method which is used to set the text to synthesize.
 *  The text is stored in a internal buffer. Then, the readText() member function
 *  will be automatically called each time the engine needs datas.
 *  When no more datas available, the engine will stop.
 *    -'_parsing' member tells BaratinooEngine what kind of parsing is expected
 *    -'_encoding' member tells BaratinooEngine how text is encoded
 * ============================================================================== */

#if !defined(BARATINOO_C_API) || defined(BARATINOOIOC_CPP)
class
#if defined (MAKE_A_DLL) && __GNUC__ >= 4
  __attribute__((visibility("default")))
#endif
InputTextBuffer :public InputText
{
  private:
    char *_buffer;
    char *_p;
    size_t _length;

    InputTextBuffer(const InputTextBuffer& inputTextBuffer); /* private, not supported or implemented */
    InputTextBuffer& operator= (const InputTextBuffer& inputTextBuffer); /* private, not supported or implemented */

  public:
    /* use newInstance/deleteInstance instead of new/delete operators to create and destroy InputTextBuffer objects on the heap.
       However if your own class inherits InputTextBuffer, you must use new/delete operators to create or delete objects of that class. */
    DllExport static InputTextBuffer *newInstance(BARATINOO_PARSING parsing = BARATINOO_ALL_PARSING, BARATINOO_TEXT_ENCODING encoding = BARATINOO_DEFAULT_ENCODING, int voiceIndex = 0, char *voiceModules = 0);
    DllExport static void deleteInstance(InputTextBuffer *instance);

    DllExport InputTextBuffer(BARATINOO_PARSING parsing=BARATINOO_ALL_PARSING, BARATINOO_TEXT_ENCODING encoding=BARATINOO_DEFAULT_ENCODING, int voiceIndex=0, char *voiceModules=0);
    DllExport ~InputTextBuffer();
    /* init() method returns 0 if an error occurs (text is null or allocation error) or 1 if all is right. */
    DllExport int init(const char *text);
    DllExport int readText(void *address, int length);
};
#endif

#if defined(BARATINOO_C_API)
typedef void* BCinputTextBuffer;

#ifdef __cplusplus
extern "C" {
#endif
  DllExport BCinputTextBuffer BCinputTextBufferNew(BARATINOO_PARSING parsing, BARATINOO_TEXT_ENCODING encoding, int voiceIndex, char *voiceModules);
  DllExport void BCinputTextBufferDelete(BCinputTextBuffer inputTextBuffer);
  DllExport int BCinputTextBufferInit(BCinputTextBuffer inputTextBuffer, const char *text);
  DllExport BARATINOOC_STATE BCinputTextBufferSetInEngine(BCinputTextBuffer inputTextBuffer, BCengine engine);
#ifdef __cplusplus
}
#endif
#endif

/* ==============================================================================
 *  BaratinooEngine objects produce their output signal in an output object.
 *  OutputSignalBuffer is an implementation of the pure virtual OutputSignal class.
 *  This implementation have a writeSignal() member function which is called when
 *  the BaratinooEngine has data to give. This function stores the signal in an internal buffer.
 *  When the engine has processed all the text, it is possible to get the whole signal
 *  by calling the getSignalBuffer() and getSignalLength() methods.
 *  Before getting signal, it should be a goo idea to test if no error occurs by calling
 *  isError() method (if an error occurs, it is a memory allocation error so the signal is incomplete
 *  and the system will be probably instable)
 *    -'_coding' member tells BaratinooEngine what kind of signal coding is
 *     expected
 *    -'_frequency' member tells BaratinooEngine which signal frequency in Hertz
 *     is expected (in [6000 ; 48000])
 * ==============================================================================*/

#if !defined(BARATINOO_C_API) || defined(BARATINOOIOC_CPP)
class
#if defined (MAKE_A_DLL) && __GNUC__ >= 4
  __attribute__((visibility("default")))
#endif
OutputSignalBuffer:public OutputSignal
{
  private:
    char *_buffer;
    int   _length;
    int   _error;
    
  public:
    enum HEADER_FORMAT {
        WAV_HEADER,
        AU_HEADER
    };
  
  private:
    int codeWavHeader( char **pHeader);
    int codeAuHeader( char **pHeader);
  
    OutputSignalBuffer(const OutputSignalBuffer& outputSignalBuffer); /* private, not supported or implemented */
    OutputSignalBuffer& operator= (const OutputSignalBuffer& outputSignalBuffer); /* private, not supported or implemented */

  public:
    /* use newInstance/deleteInstance instead of new/delete operators to create and destroy OutputSignalBuffer objects on the heap.
       However if your own class inherits OutputSignalBuffer, you must use new/delete operators to create or delete objects of that class. */
    DllExport static OutputSignalBuffer *newInstance(BARATINOO_SIGNAL_CODING coding = BARATINOO_PCM, int frequency = BARATINOO_OUTPUT_FREQUENCY);
    DllExport static void deleteInstance(OutputSignalBuffer *instance);

    DllExport OutputSignalBuffer(BARATINOO_SIGNAL_CODING coding=BARATINOO_PCM, int frequency=BARATINOO_OUTPUT_FREQUENCY);
    DllExport ~OutputSignalBuffer();
    DllExport int   writeSignal(const void *address, int length);
    DllExport int   isError()                                 { return _error;  }
    DllExport char *getSignalBuffer()                         { return _buffer; }
    DllExport int   getSignalLength()                         { return _length; }  /* in bytes */
    DllExport void  resetSignal();
    DllExport int   getSignalHeader(HEADER_FORMAT format, char **pHeader);
    DllExport void  deleteSignalHeader(char *header);
    DllExport void* detachSignal();

};
#endif

#if defined(BARATINOO_C_API)
typedef void* BCoutputSignalBuffer;

typedef enum {
  OUTPUTSIGNALBUFFER_WAV_HEADER,
  OUTPUTSIGNALBUFFER_AU_HEADER
} OUTPUTSIGNALBUFFER_HEADER_FORMAT;


#ifdef __cplusplus
extern "C" {
#endif
  DllExport BCoutputSignalBuffer BCoutputSignalBufferNew(BARATINOO_SIGNAL_CODING coding, int frequency);
  DllExport void BCoutputSignalBufferDelete(BCoutputSignalBuffer outputSignalBuffer);
  DllExport void BCoutputTextBufferSetInEngine(BCoutputSignalBuffer outputSignalBuffer, BCengine engine);
  DllExport int BCoutputSignalBufferIsError(BCoutputSignalBuffer outputSignalBuffer);
  DllExport char * BCoutputSignalBufferGetSignalBuffer(BCoutputSignalBuffer outputSignalBuffer);
  DllExport int BCoutputSignalBufferGetSignalLength(BCoutputSignalBuffer outputSignalBuffer);
  DllExport void BCoutputSignalBufferResetSignal(BCoutputSignalBuffer outputSignalBuffer);
  DllExport int BCoutputSignalBufferGetSignalHeader(BCoutputSignalBuffer outputSignalBuffer, OUTPUTSIGNALBUFFER_HEADER_FORMAT format, char **pHeader);
  DllExport void BCoutputSignalBufferDeleteSignalHeader(char *header);
  DllExport void * BCoutputSignalBufferDetachSignal(BCoutputSignalBuffer outputSignalBuffer);
  DllExport void BCoutputSignalBufferDeleteSignal(void *signal);
#ifdef __cplusplus
}
#endif
#endif


#endif      /* _BARATINOOIO_H */
