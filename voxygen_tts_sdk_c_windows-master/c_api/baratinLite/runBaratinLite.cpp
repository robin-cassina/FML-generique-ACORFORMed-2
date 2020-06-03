/*
 * Main method to test baratinLite.
 *
 * Usage : runBaratinLite.exe [textFile [wavOutFile [configFile]]]
 * Default parameters :
 *   textFile = TXT_FILE
 *   wavOutFile = OUTPUT_PATH + OUTPUT_WAV_FILE
 *   configFile = CFG_FILE
 *
 *  Created on: 5 avr. 2017
 *      Author: montcheuil
 */


#include "baratinLite.h"
#include <string.h>

#define CFG_FILE         "../../config/baratinoo.cfg"	// (default) TTS Engine config file, redefined by the 3rd parameter
#define TXT_FILE         "./texte.txt"	// (default) texte input file, redefined by the first parameter
#define OUTPUT_PATH      "./"

#define OUTPUT_LOG_FILE "runBaratinLite.log"
#define OUTPUT_WAV_FILE "runBaratinLite.wav"	// (default) wav output file (in OUTPUT_PATH), redefined (the whole path) by the 2nd parameter
#define OUTPUT_FREQUENCY 16000
//#define TRUE 1
//#define FALSE 0

/*==================================================================================================*/
/*====================================================================================================*/
//---------------------------------------------------------------------------------
int run(BaratinooLite *lite, const char *txtFile, const char *soundFile) {
    InputTextBuffer inputTextBuffer(BARATINOO_ALL_PARSING, BARATINOO_UTF8);
    OutputSignalBuffer outputSignalBuffer(BARATINOO_PCM, OUTPUT_FREQUENCY);
    char Buf[4000];
    memset(Buf, 0, 4000);
    FILE *f;

    // First synthesis using helper classes InputTextBuffer and OutputSignalBuffer
    // initialise input object
    f = fopen(txtFile, "rb");
    if (f != NULL) {
        fread(Buf, 4000, sizeof(char), f);
        fclose(f);
    } else {
        strcpy(Buf, "No text file found. Using default text.");
    }
    lite->traceInfo("Run BaratinooLite", "Text to be synthesized : %s", Buf);
    if (!inputTextBuffer.init(Buf)) {
        lite->traceError("Run BaratinooLite", "InputTextBuffer::init() error.\n");
        return -1;
    }

    //done in the BaratinooLite::init() _engine->setWantedEvent(BARATINOO_MARKER_EVENT);
    // synthesize
    if (0 == lite->textToSpeech(&inputTextBuffer, &outputSignalBuffer)) {
        // synthesis ok : retrieve sound data from OutputSignalBuffer.
        if (NULL != soundFile) {
            f = fopen(soundFile, "wb");
            if (f != NULL) {
                char *pHeader = NULL;
                int headerLength;
                headerLength = outputSignalBuffer.getSignalHeader(OutputSignalBuffer::WAV_HEADER, &pHeader);
                if (headerLength <= 0) {
                    lite->traceError("BaratinooLite", "couldn't retrieve generated signal wav header\n");
                } else {
                    fwrite(pHeader, headerLength, sizeof(char), f);
                    outputSignalBuffer.deleteSignalHeader(pHeader);
                    fwrite(outputSignalBuffer.getSignalBuffer(), outputSignalBuffer.getSignalLength(), sizeof(char), f);
                }
                fclose(f);
    		fprintf(stderr, "Signal was stored in WAV file '%s'\n", soundFile);
            }
        }
    }
    lite->traceInfo("BaratinooLite", "First synthesis done.");


    return 0;
}  // end run
int main(int argc, char** argv) {
    BaratinooLite *lite = new BaratinooLite(OUTPUT_PATH, OUTPUT_LOG_FILE);
    fprintf(stderr, "Logs BARATINOO are stored in file '%s'\n\n", OUTPUT_LOG_FILE);
    
    // (a) The input text file
    char *txtFile = argc>1 ? argv[1] : TXT_FILE;
    fprintf(stderr, "Input text/SSML file is '%s'\n\n", txtFile);

    // (b) The output sound file
    char *soundFile;
    if (argc>2) {
        soundFile = new  char[strlen(argv[2]) + 1];
        if (soundFile == NULL)
            return 1;
	strcpy(soundFile,argv[2]);	
    } else if (NULL != OUTPUT_WAV_FILE && '\0' != *OUTPUT_WAV_FILE) {
        soundFile = new  char[strlen(OUTPUT_PATH) + strlen(OUTPUT_WAV_FILE) + 1];
        if (soundFile == NULL)
            return 1;
        sprintf(soundFile, "%s%s", OUTPUT_PATH, OUTPUT_WAV_FILE);
    }

    // (c) The config file file
    char *cfgFile;
    if (argc>3) {
	cfgFile = argv[3];
    	fprintf(stderr, "TTS Engine config file is '%s'\n\n", cfgFile);
    } else {
	cfgFile = CFG_FILE;
    	fprintf(stderr, "Default TTS Engine config file : '%s'\n\n", cfgFile);
    }

    if (0 == lite->init(cfgFile)) {
        //lite->run();
    	run(lite, txtFile, soundFile);
        lite->shutdown();
    } else {
        fprintf(stderr, "Initialisation failed\n");
    }

    delete lite;
    if (soundFile != NULL) {
        delete[] soundFile;
    }

    return 0;
}

