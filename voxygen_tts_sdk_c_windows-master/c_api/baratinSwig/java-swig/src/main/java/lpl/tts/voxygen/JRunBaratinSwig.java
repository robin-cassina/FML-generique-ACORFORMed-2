package lpl.tts.voxygen;

import java.io.File;
import java.io.IOException;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import lpl.tools.ByteArrays;
import lpl.tools.ContinuousEventIfce;
import lpl.tools.TimeStampEventIfce;
import lpl.tts.SpeechDataTools;

/**
 * Hello world!
 *
 */
public class JRunBaratinSwig 
{
	public static Logger logger = LoggerFactory.getLogger(JRunBaratinSwig.class);
	
	public static final String TXT_FILE="texte.txt";
	public static final String OUTPUT_WAV_FILE="jRunBaratinSwig.wav";
	public static final String OUTPUT_LOG_FILE="jRunBaratinSwig.log";
	public static boolean NO_BARATINOO_LOG=false;
	
	// (!) Greta expect  RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16 bit, mono 48000 Hz
	public static final int OUTPUT_FREQUENCY=48000;
	
	//TMP:DEBUG
	public static final EVENT_TYPE[] WANTED_EVENTS = {EVENT_TYPE.WORD_EVENT, EVENT_TYPE.VISEME_EVENT};
	public static final EVENT_TYPE[] SHOWN_EVENTS = {EVENT_TYPE.WORD_EVENT, EVENT_TYPE.PHONEME_EVENT, EVENT_TYPE.VISEME_EVENT};
	public static final boolean SHOW_SOME_TIMEEVENTS = true; 
	
	public static boolean WRITE_RAW_FILE=true;
	public static boolean WRITE_HEADER_FILE=true;
	
	/**
	 * (Similar to runBaratinSwig.cpp).
	 * Usage : RunBaratinSwig [input.txt [output.wav [config.cfg]]]
	 * 
	 * @param args
	 */
	public static void main( String[] args )
    {
        String txtFile=TXT_FILE, outputWav=OUTPUT_WAV_FILE, config=VoxygenLibraries.CFG_FILE;
        
        //A. The Main arguments
        
        // Get the input text file
        if (args.length>0)
        	txtFile = args[0];
        logger.info("Input text file: {}",txtFile);
        // Get the output wav file
        if (args.length>1)
        	outputWav = args[1];
        logger.info("Output wav file: {}",outputWav);
        // Get the config file
        if (args.length>2)
        	config = args[2];
        logger.info("Config file: {}",config);
        
        //B. Load the libraries
        // load the baratinSwig DLL
        if (! VoxygenLibraries.loadVoxygenLibraries(null)) {
        	logger.error("Failed to load the Voxygen libraries ;-(");
        	return;
        }
        logger.debug("Voxygen libraries loaded ;-)");
        
        //C. Create a BaratinooSwig and init the BaratinooEngine
        File outputPath = new File("./");
        BaratinooSwig baraSwig;
        if (NO_BARATINOO_LOG) {//TMP: no log file
        	baraSwig = new BaratinooSwig(outputPath.getPath(), null);//TODO:pass path with '/'
        	logger.warn("NO (Baratinoo) logs in {}", new File(outputPath, OUTPUT_LOG_FILE)); //TODO baraSwig.getLogFile()...
        } else {
        	baraSwig = new BaratinooSwig(outputPath.getPath(), OUTPUT_LOG_FILE);//TODO:pass path with '/'
        	logger.debug("(Baratinoo) logs are stored in {}", new File(outputPath, OUTPUT_LOG_FILE)); //TODO baraSwig.getLogFile()...
        }
        int init = baraSwig.init(config);
        if (init!=0) {
        	logger.error("Baratinoo Engine initialization error (code:{})",init);
        }
        logger.debug("Baratinoo Engine initialized ;-)");
        
        //D. Configure BaratinooSwig
        // (!) Greta expect  RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16 bit, mono 48000 Hz
        baraSwig.setFrequency(OUTPUT_FREQUENCY); // set the (default) frequency
        //TMP:DEBUG
        if (WANTED_EVENTS!=null) for (EVENT_TYPE type: WANTED_EVENTS) baraSwig.setWantedEvent(type); // set WANTED_EVENTS
        
        //E. Load the text file
        File inputFile = new File(txtFile); 
        byte[] text;
		try {
			text = ByteArrays.readFileBytes(inputFile, true);
		} catch (IOException e) {
        	logger.error("Can't read input texte file",e);
        	return;
		}
        SpeechResult speechResult = baraSwig.textToSpeech(text);
        logger.debug("Text converted => {} raw data bytes, freq:{}, duration={}ms", speechResult.getRawDataLength(), speechResult.getFrequency(), speechResult.getDuration());
        
        //F. write the output file(s)
        File outputFile; long bytes;
        {// - the wav file
        	outputFile = new File(outputWav);
        	bytes = SpeechDataTools.writeSoundFile(speechResult, outputFile, true);
        	if (bytes<0) logger.error("Can't writting the waw file {}", outputFile); 
        	else logger.info("Output wav file {} writted, {} bytes", outputFile, bytes);
        }
        if (WRITE_RAW_FILE) {// - a raw file
        	outputFile = new File(outputWav+".raw");
        	bytes = SpeechDataTools.writeSoundFile(speechResult, outputFile, false);
        	if (bytes<0) logger.error("Can't writting the raw file {}", outputFile); 
        	else logger.info("Output raw file {} writted, {} bytes", outputFile, bytes);
        }
        if (WRITE_HEADER_FILE) {// - an header file
        	outputFile = new File(outputWav+".header");
        	bytes = SpeechDataTools.writeHeaderInFile(speechResult, outputFile);
        	if (bytes<0) logger.error("Can't writting the header file {}", outputFile); 
        	else logger.info("Output header file {} writted, {} bytes", outputFile, bytes);
        }
        
        //G. Show the TimeStampEvent
        int nbAllEvents = speechResult.getAllEventsSize();
        if (nbAllEvents>0) {
        	// (a) all events
        	logger.debug("{} collected events:", nbAllEvents);
        	for (TimeStampEventIfce event : speechResult.getAllEvents()) {
        		logger.debug("\t[{}ms] event name:{} (type:{}, class:{})", event.getMillisecond(), event.getName(), event.getType(), event.getClass());
        	}
        	logger.debug("---");
        	// (b) marker events
        	{	int nbMarkers = speechResult.getMarkerEventsSize();
        		if (nbMarkers>0) {
        			logger.debug("{} collected marker events", nbMarkers);
        			for (TimeStampEventIfce event : speechResult.getMarkerEvents()) {
        				logger.debug("\t[{}ms] event name:{} (type:{}, class:{})", event.getMillisecond(), event.getName(), event.getType(), event.getClass());
        			}
        			logger.debug("---");
        		} else logger.debug("Any collected marker events");
        	}
        	// (b) viseme events
        	{	int nbVisemes = speechResult.getVisemeEventsSize();
        		if (nbVisemes>0) {
        			logger.debug("{} collected viseme events", nbVisemes);
        			for (ContinuousEventIfce event : speechResult.getVisemeEvents()) {
        				logger.debug("\t[{}ms] event name:{} (type:{}, class:{}), duration:{}ms", event.getMillisecond(), event.getName(), event.getType(), event.getClass(), event.getDuration());
        			}
        			logger.debug("---");
        		} else logger.debug("Any collected viseme events");
        	}
        	// (c) SHOWN_EVENTS
        	for (EVENT_TYPE type: SHOWN_EVENTS) {
        		int nbTypes = speechResult.getTypedEventsSize(type);
        		if (nbTypes>0) {
        			logger.debug("{} collected events of type {}", nbTypes, type);
        			for (TimeStampEventIfce event : speechResult.getTypedEvents(type)) {
        				logger.debug("\t[{}ms] event name:{} (type:{}, class:{})", event.getMillisecond(), event.getName(), event.getType(), event.getClass());
        			}
        			logger.debug("---");
        		} else logger.debug("Any collected event of type {}", type);
        	}
        } else logger.debug("Any collected event.");
        
        if (SHOW_SOME_TIMEEVENTS) {//TMP:test jdowncast
        	logger.debug("Some events:");
        	for (TimeStampEventIfce event : lpl.tts.voxygen.TimeStamp.someTimeEvents()) {
        		if (event instanceof ContinuousEventIfce) 
        			logger.debug("\t[{}ms] continous event name:{}, duration:{} (type:{}, class:{})", event.getMillisecond(), event.getName(), ((ContinuousEventIfce) event).getDuration(), event.getType(), event.getClass());
        		else
        			logger.debug("\t[{}ms] event name:{} (type:{}, class:{})", event.getMillisecond(), event.getName(), event.getType(), event.getClass());
        	}
        	logger.debug("---");

        }
        
        //Z. shutdown the BaratinooEngine
        baraSwig.shutdown();
    }
    
}
