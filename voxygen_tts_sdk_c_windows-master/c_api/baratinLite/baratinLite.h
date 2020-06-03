/*
 * A little C++ interface for the Voxigen baratinoo library.
 * Based on the barademo_CPP example.
 *
 *  Created on: 5 avr. 2017
 *      Author: montcheuil
 */

#ifndef BARATINLITE_H_
#define BARATINLITE_H_

#include <stdio.h>
#include "baratinoo.h"
#include "baratinooio.h"

// Define LiteDllExport based on LITE_MAKE_DLL (similar to baratinoo.h)
#if defined(_WIN32) || defined(_WIN64)
  #if defined (LITE_MAKE_DLL)
    #pragma message ( "[WIN] LITE_MAKE_DLL is defined")
    #define LiteDllExport __declspec( dllexport )
  #else
    #pragma message ( "[WIN] LITE_MAKE_DLL isn't defined")
    //#define LiteDllExport
    #define LiteDllExport __declspec( dllimport )
  #endif
#else
  #if defined (LITE_MAKE_DLL) && __GNUC__ >= 4
    #pragma message ( "[!WIN] LITE_MAKE_DLL is defined")
    #define LiteDllExport __attribute__ ((visibility("default")))
  #else
    #pragma message ( "[!WIN] LITE_MAKE_DLL isn't defined (or GNUC<4)")
    #define LiteDllExport
  #endif
#endif

class BaratinooLite : public BaratinooTrace, private BaratinooKey {

    private:
        BaratinooEngine         *_engine;
        char                    *_configFile;
        char                    *_logfile;
        //char                    *_soundfile;
        FILE                    *_log;

        BaratinooLite(const BaratinooLite& lite); // private, not supported or implemented
        BaratinooLite& operator= (const BaratinooLite& lite); // private, not supported or implemented

        // begin share secret key : specific use case
        using BaratinooKey::get; // unhide base class virtual functions
        const char * get(int index) const { return (0 == index ? "shared_secret_key_provided_by_Voxygen" : NULL); }
        // end share secret key

    protected:
	BaratinooEngine::BARATINOO_STATE reset(BaratinooEngine::BARATINOO_STATE state);

    public:
        LiteDllExport BaratinooLite(const char *outputPath, const char *logFile);
        LiteDllExport ~BaratinooLite();

        LiteDllExport int init(const char *configurationFile);
        LiteDllExport void shutdown();

        LiteDllExport int textToSpeech(InputText *pInputText, OutputSignal *pOutputSignal);

        LiteDllExport void traceError(const char *source, const char *format, ...);
        LiteDllExport void traceInfo (const char *source, const char *format, ...);
        using BaratinooTrace::write; // unhide base class virtual functions
        void write(TraceLevel level, int engineNumber, const char *source, const char *format, va_list args);
};

// just in case, undefine LiteDllExport
#undef LiteDllExport

#endif /* BARATINLITE_H_ */
