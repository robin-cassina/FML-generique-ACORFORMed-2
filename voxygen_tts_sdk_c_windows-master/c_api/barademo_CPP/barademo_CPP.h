/* $Id: barademo_CPP.h 28171 2017-03-02 14:23:06Z paul $ -*- coding: utf-8-unix -*- */

#include <stdio.h>
#include "baratinooio.h"

#define CFG_FILE         "../../config/baratinoo.cfg"
#define TXT_FILE         "./texte.txt"
#define OUTPUT_PATH      "./"

#define OUTPUT_LOG_FILE "barademo_CPP.log"
#define OUTPUT_WAV_FILE "barademo_CPP.wav"
#define OUTPUT_FREQUENCY 24000
#define TRUE 1
#define FALSE 0

class BaratinooDemo : public BaratinooTrace, private BaratinooKey {
    private :
        class SampleOutputSignal : public OutputSignal {
        private :
            BaratinooDemo *_baratinooDemo;
        public:
            SampleOutputSignal(BARATINOO_SIGNAL_CODING coding, int frequency, BaratinooDemo *pBaratinooDemo) : OutputSignal(coding, frequency), _baratinooDemo(pBaratinooDemo) {}
            virtual int writeSignal(const void *address, int length);
        };

    private:
        BaratinooEngine         *_engine;
        char                    *_logfile;
        char                    *_soundfile;
        FILE                    *_log;

        BaratinooDemo(const BaratinooDemo& demo); // private, not supported or implemented
        BaratinooDemo& operator= (const BaratinooDemo& demo); // private, not supported or implemented

        // begin share secret key : specific use case
        using BaratinooKey::get; // unhide base class virtual functions
        const char * get(int index) const { return (0 == index ? "shared_secret_key_provided_by_Voxygen" : NULL); }
        // end share secret key
    
    public:
        BaratinooDemo(const char *outputPath, const char *logFile, const char *pcmFile);
        ~BaratinooDemo();

        int init(const char *configurationFile);
        void shutdown();

        int run();
        int textToSpeech(InputText *pInputText, OutputSignal *pOutputSignal);

        void traceError(const char *source, const char *format, ...);
        void traceInfo (const char *source, const char *format, ...);
        using BaratinooTrace::write; // unhide base class virtual functions
        void write(TraceLevel level, int engineNumber, const char *source, const char *format, va_list args);
};

