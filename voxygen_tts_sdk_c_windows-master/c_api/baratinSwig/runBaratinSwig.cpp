/*
 * Main method to test baratinSwig.
 *
 * Usage : runBaratinSwig.exe [textFile [wavOutFile [configFile]]]
 * Default parameters :
 *   textFile = TXT_FILE
 *   wavOutFile = OUTPUT_PATH + OUTPUT_WAV_FILE
 *   configFile = CFG_FILE
 *
 *  Created on: 5 avr. 2017
 *      Author: montcheuil
 */


#include "baratinSwig.h"
#include <string.h>

#define CFG_FILE         "../../config/baratinoo.cfg"	// (default) TTS Engine config file, redefined by the 3rd parameter
#define TXT_FILE         "./texte.txt"	// (default) texte input file, redefined by the first parameter
#define OUTPUT_PATH      "./"

#define OUTPUT_LOG_FILE "runBaratinSwig.log"
#define OUTPUT_WAV_FILE "runBaratinSwig.wav"	// (default) wav output file (in OUTPUT_PATH), redefined (the whole path) by the 2nd parameter
#define OUTPUT_FREQUENCY 16000
//#define TRUE 1
//#define FALSE 0

static const char* EVENT_NAMES[BARATINOO_EVENT_TYPE::BARATINOO_NUMBER_OF_EVENT+1] = {"Marker", "WaitMarker", "Paragraphe", "Sentence", "Word", "Punctuation", "Separator", "Syllabe", "Phoneme", "Silence", "NewVoice", "Raw", "--NUMBER_OF_EVENT--"};

/*==================================================================================================*/
/*====================================================================================================*/
//---------------------------------------------------------------------------------
int run(BaratinooSwig &swig, const char *txtFile, const char *soundFile) {
    byte Buf[4000];
    memset(Buf, 0, 4000);
    FILE *f;

    // First synthesis using helper classes InputTextBuffer and OutputSignalBuffer
    // initialise input object
    f = fopen(txtFile, "rb");
    if (f != NULL) {
        fread(Buf, 4000, sizeof(byte), f);	//TODO: check nb. of bytes read and extend buffer if 4000 or more /!\ Buf should ends with 0
        fclose(f);
    } else {
        strcpy(Buf, "No text file found. Using default text.");
    }
    swig.traceInfo("runBaratinooSwig", "Text to be synthesized : %s", Buf);

    //done in the BaratinooSwig::init() _engine->setWantedEvent(BARATINOO_MARKER_EVENT);
    // synthesize
    SpeechResult *speechResult = swig.textToSpeech(Buf);
    if (speechResult != NULL) {
    	//TODO: check isError()
    	if (speechResult->isError()>0) {
    		swig.traceError("runBaratinooSwig", "Error in the OutputSignal(Buffer): %d", speechResult->isError());
    	} else {
    		// Show events informations
    		if (true) {
    			if (int nbEvents = speechResult->getAllEventsSize()) {
    				fprintf(stderr, "%d collected events:\n", nbEvents);
    				for (const auto &event : speechResult->getAllEvents()) {
    					//fprintf(stderr, "[\n");
    					fprintf(stderr, "\t[%8.2fms] name:%s (type:%s)\n", event->millisecond, event->name.c_str(), EVENT_NAMES[event->type]);
    					//fprintf(stderr, "]\n");
    				}
    				fprintf(stderr, "---\n");
    				if (int nbMarkers = speechResult->getMarkerEventsSize()) {
    					fprintf(stderr, "%d collected markers:\n", nbMarkers);
    					for (const auto &event : speechResult->getMarkerEvents()) {
    						//fprintf(stderr, "[\n");
    						fprintf(stderr, "\t[%8.2fms] Marker id:%s (type:%s)\n", event->millisecond, event->name.c_str(), EVENT_NAMES[event->type]);
    						//fprintf(stderr, "]\n");
    					}
    					fprintf(stderr, "---\n");
    				}
    				if (int nbVisemes = speechResult->getVisemeEventsSize()) {
    					fprintf(stderr, "%d collected visemes:\n", nbVisemes);
    					for (const auto &event : speechResult->getVisemeEvents()) {
    						//fprintf(stderr, "[\n");
    						fprintf(stderr, "\t[%8.2fms] Viseme id:%s (type:%s), duration:%.2fms\n", event->millisecond, event->name.c_str(), EVENT_NAMES[event->type], event->duration);
    						//fprintf(stderr, "]\n");
    					}
    					fprintf(stderr, "---\n");
    				}
    				{
    					const BARATINOO_EVENT_TYPE types[] = {BARATINOO_EVENT_TYPE::BARATINOO_WORD_EVENT, BARATINOO_EVENT_TYPE::BARATINOO_PHONEME_EVENT};
    					for (BARATINOO_EVENT_TYPE type: types) {
    						if (int nbTypes = speechResult->getTypedEventsSize(type)) {
    							fprintf(stderr, "%d collected %s events:\n", nbTypes, EVENT_NAMES[type]);
    							for (const auto &event : speechResult->getTypedEvents(type)) {
    								//fprintf(stderr, "[\n");
    								fprintf(stderr, "\t[%8.2fms] event name:%s (type:%s)\n", event->millisecond, event->name.c_str(), EVENT_NAMES[event->type]);
    								//fprintf(stderr, "]\n");
    							}
    							fprintf(stderr, "---\n");
    						} else  {
    							fprintf(stderr, "Any collected %s events\n", EVENT_NAMES[type]);
    						}
    					}
    				}
    			} else {
    				fprintf(stderr, "Any collected events.\n");
    			}
    		}
    		// synthesis ok : retrieve sound data from OutputSignalBuffer.
    		if (NULL != soundFile) {
    			f = fopen(soundFile, "wb");
    			if (f != NULL) {
    				//byte *pHeader = NULL;
    				//int headerLength;
    				size_t headerLength, rawLength;
    				headerLength = speechResult->getHeaderLength();//outputSignalBuffer.getSignalHeader(OutputSignalBuffer::WAV_HEADER, &pHeader);
    				if (headerLength <= 0) {
    					swig.traceError("runBaratinooSwig", "couldn't retrieve generated signal wav header\n");
    				} else {
    					fwrite(speechResult->getHeader().data(), sizeof(byte), headerLength, f);
    					//outputSignalBuffer.deleteSignalHeader(pHeader);
    					//fwrite(outputSignalBuffer.getSignalBuffer(), outputSignalBuffer.getSignalLength(), sizeof(byte), f);
    					rawLength = speechResult->getRawDataLength();
    					fwrite(speechResult->getRawData().data(), sizeof(byte), rawLength, f);
    				}
    				fclose(f);
    				fprintf(stderr, "Signal was stored in WAV file '%s (raw bytes=%d; freq:%d; duration=%.2fms)'\n", soundFile, rawLength, speechResult->getFrequency(), speechResult->getDuration());
    			}
    		}
    	}
        SpeechResult::deleteInstance(speechResult); // delete SpeechResult
    } else swig.traceError("runBaratinooSwig", "NULL SeepchResult %p", speechResult);
    swig.traceInfo("runBaratinooSwig", "First synthesis done.");


    return 0;
}  // end run
int main(int argc, char** argv) {
    BaratinooSwig *swig = new BaratinooSwig(OUTPUT_PATH, OUTPUT_LOG_FILE);
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

    if (0 == swig->init(cfgFile)) {
        //swig->run();
    	swig->setWantedEvent(BARATINOO_EVENT_TYPE::BARATINOO_WORD_EVENT);
    	run(*swig, txtFile, soundFile);
        swig->shutdown();
    } else {
        fprintf(stderr, "Initialisation failed\n");
    }

    delete swig;
    if (soundFile != NULL) {
        delete[] soundFile;
    }

    return 0;
}

