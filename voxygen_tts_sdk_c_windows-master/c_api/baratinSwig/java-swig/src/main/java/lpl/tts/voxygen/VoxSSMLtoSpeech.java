package lpl.tts.voxygen;

import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.EnumSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.io.Charsets;
import org.apache.commons.io.FileUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.fasterxml.jackson.databind.ObjectMapper;

import lpl.tools.ContinuousEventIfce;
import lpl.tts.SpeechData;
import lpl.tts.SpeechDataTools;
import lpl.tts.fml.FMLtoSSML;
import lpl.tts.fml.SpeechSSML;
import lpl.tts.ssml.SSMLBytesStream;
import lpl.tts.ssml.SSMLBytesStream.SSMLBytesStreamPlus;
import lpl.tts.ssml.SSMLNullTermBytes;
import lpl.tts.ssml.SSMLNullTermBytes.SSMLNullTermBytesPlus;
import lpl.tts.ssml.SSMLNullTermBytesStreamWrapper;
import lpl.tts.ssml.SSMLNullTermBytesStreamWrapper.SSMLNullTermBytesStreamPlusWrapper;

/**
 * A class to convert (Voxygen) SSML (byte[]) into Speech using Voxygen. 
 */

public class VoxSSMLtoSpeech {
	public static Logger logger = LoggerFactory.getLogger(VoxSSMLtoSpeech.class);
	
	/** The Baratinoo Engine Wrapper.*/
	protected BaratinooSwig batatinooSwig;
	
	/** The library root directory.
	 * Voxygen libraries are in {@link VoxygenLibraries#VOXYGEN_PATH} inside the root path.
	 * The default is <code>null</code> &rArr; the current directory.
	 */
	protected static File librariesRootDir = null;
	
	/** Are Voxygen libraries loaded ? */
	protected static boolean voxygenLibLoaded = false; 
	
	/** The logging directory.*/
	protected File logDirectory = new File("test-vox");
	/** The Baratinoo log file name.*/
	protected String logBaratinooFileName = "BaratinooSSMLtoSpeech.log";
	/** (Des)activate the Baratinoo log file.*/
	protected boolean logBaratinoo = true;
	
	/** The Voxygen Config path (from working directory).*/
	protected String voxygenConfigPath = VoxygenLibraries.CFG_FILE;
	
	// (!) Greta expect  RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16 bit, mono 48000 Hz
	/** The wav output frequency.*/
	protected int wavFrequency=48000;
	
	/** The set of wanted events.*/
	Set<EVENT_TYPE> wantedEvents = EnumSet.of(EVENT_TYPE.MARKER_EVENT, EVENT_TYPE.VISEME_EVENT);
	
	// JSON writer
	protected static ObjectMapper mapper = new ObjectMapper();
	
	// Load the Voxygen libraries
	public static synchronized boolean loadVoxygenLibraries() {
		//B. Load the libraries
        // load the baratinSwig DLL
		if (! voxygenLibLoaded) {
			if (! VoxygenLibraries.loadVoxygenLibraries(librariesRootDir)) {
				logger.error("Failed to load the Voxygen libraries from ", librariesRootDir==null ? new File(".") : librariesRootDir);
				return false;
			}
			voxygenLibLoaded = true;
			logger.debug("Voxygen libraries loaded ;-)");
		} else {
			logger.warn("Voxygen libraries yet loaded !");
		}
		return voxygenLibLoaded;
	}
	
	/**
	 * Initialize 
	 */
	public void init()
			throws IllegalStateException	//TODO? better exception
	{
		//B. Load the libraries
        // load the baratinSwig DLL
		if (!voxygenLibLoaded && ! loadVoxygenLibraries()) {
			throw new IllegalStateException("Can't load Voxygen libraries");//TODO: exception
		}

        //C. Create a BaratinooSwig and init the BaratinooEngine
        if (logBaratinoo) {
        	if (logDirectory==null) logDirectory = new File("."); // default => current directory
        	else if (! logDirectory.exists()) {
            	logger.debug("Create log directory:"+logDirectory);
        		try {
        			logDirectory.mkdirs();
        		} catch (SecurityException e) {
        			logger.warn("Can't create the log directory '"+logDirectory+"': "+e.getMessage());
        		}
        	}
        	batatinooSwig = new BaratinooSwig(logDirectory.getPath(), logBaratinooFileName);
        	logger.debug("(Baratinoo) logs are stored in {}", new File(logDirectory, logBaratinooFileName)); //TODO batatinooSwig.getLogFile()...
        } else {//TMP: no log file
        	batatinooSwig = new BaratinooSwig(logDirectory==null? null : logDirectory.getPath(), null);
        	logger.warn("NO (Baratinoo) log file");
        }
        //batatinooSwig.setLogLevel(LogLevel.INIT); //TMP
        int init = batatinooSwig.init(voxygenConfigPath);
        if (init!=0) {
        	logger.error("Baratinoo Engine initialization error (code:{})",init);
			throw new IllegalStateException("Can't init Baratinoo engine");//TODO: exception
        }
        logger.debug("Baratinoo Engine initialized ;-)");
        
        //D. Configure BaratinooSwig
        // (!) Greta expect  RIFF (little-endian) data, WAVE audio, Microsoft PCM, 16 bit, mono 48000 Hz
        if (wavFrequency>0) batatinooSwig.setFrequency(wavFrequency); // set the output frequency
        if (wantedEvents!=null) {
        	batatinooSwig.unsetAllWantedEvent();
        	for (EVENT_TYPE type: wantedEvents) batatinooSwig.setWantedEvent(type);
        } // else: let as default
        
	}
	
	public SpeechResult ssmlToSpeech(SSMLNullTermBytes ssmlReader) {
		// TODO: check init
		byte[] text;
		if (ssmlReader instanceof SSMLNullTermBytesPlus) {
		// TODO: adapt encoding and xml declaration option to the baratinoo engine
			text = ((SSMLNullTermBytesPlus) ssmlReader).getNullTermBytes(true);
		} else {
		// TODO: adapt baratinoo encoding setting 
			text = ssmlReader.getNullTermBytes();
		}
		//TODO: adapt baratinoo setting to parse SSML
		return batatinooSwig.textToSpeech(text);
	}
	
	public SpeechData ssmlToSpeech(SSMLBytesStream ssmlReader) {
		return ssmlToSpeech(ssmlReader instanceof SSMLBytesStreamPlus
				? new SSMLNullTermBytesStreamPlusWrapper<SSMLBytesStreamPlus>((SSMLBytesStreamPlus)ssmlReader)
				: new SSMLNullTermBytesStreamWrapper<SSMLBytesStream>(ssmlReader)
				);
	}
	

	public static void main(String[] args )
    {
		@SuppressWarnings("deprecation")
		Charset fmlEncoding = Charsets.ISO_8859_1;
		//TODO: Options
		// list of FML file names to process
		List<String> fmlFileNames = Arrays.asList(args);
		
		// (A) Create a VoxSSMLtoSpeech
		VoxSSMLtoSpeech ssmlToSpeech = new VoxSSMLtoSpeech();
		// (B) Adapt the configuration
		//TODO
		// (C) Init the engine
		try {
			ssmlToSpeech.init();
		} catch (IllegalStateException e) {
			System.err.println("Can't initialize the TTS Engine: "+e);
			return;
		}
		
		// (D) Create a FML --> SSML(s) converter
		FMLtoSSML fmlToSSML = new FMLtoSSML();
		
		// (E) Run on each FML files
		SimpleDateFormat formatter = new SimpleDateFormat("yyMMdd-HHmm");
		File outputDir = new File(String.format("%s/%s","test-fml2speech", formatter.format(new Date())));
		if (!outputDir.exists()) outputDir.mkdirs();
		for (String fmlFilename : fmlFileNames) {
			// (a) read the FML file
			File fmlFile = new File(fmlFilename);
			String fmlContent;
			try {
				fmlContent = FileUtils.readFileToString(fmlFile, fmlEncoding);
			} catch (IOException e) {
				logger.error("Can't read FML file {} : {}", fmlFile, e);
				continue;
			}
			// (b) convert into a SSML(s) /!\ 1 FML produce 0, 1 or various SSMLs (1 per <speech> section) 
			SpeechSSML[] fmlSSMLs = fmlToSSML.fmlToSSML(fmlContent);
			if (fmlSSMLs.length<=0) { // nothing
				logger.info("FML {} had any speech section", fmlFilename);
				continue;
			} else logger.info("FML {} had {} speech section(s)", fmlFilename, fmlSSMLs.length);
			// (c) convert into speech
			SpeechResult[] fmlSpeechs = new SpeechResult[fmlSSMLs.length];
			for (int i=0; i<fmlSSMLs.length; ++i) {
				fmlSpeechs[i] = ssmlToSpeech.ssmlToSpeech((SSMLNullTermBytes) fmlSSMLs[i]); 
			}
			// (d) write results
			String fmlBasename = outputDir.getPath()+"/"+ // output dir
					fmlFile.getName().replaceAll("\\.\\w+$", "") // filename without extension
					;
			// (d.1) individual speech output
			if (fmlSSMLs.length>1) {
				for (int i=0; i<fmlSpeechs.length; ++i) {
					// numbered basename
					String outBasename = String.format("%s-%d", fmlBasename, i);
					String logPrefix = String.format("[%s[%d]] ", fmlFilename, i);
					// the intermediate SSML
					writeSSML(fmlSSMLs[i], outBasename, logPrefix);
					// the current SpeechResult
					SpeechResult speechResult = fmlSpeechs[i];
					writeSpeechResult(speechResult, outBasename, logPrefix);
				}
			} else {
					// basename
					String outBasename = fmlBasename;
					String logPrefix = String.format("[%s] ", fmlFilename);
					// the intermediate SSML
					writeSSML(fmlSSMLs[0], outBasename, logPrefix);
					// the current SpeechResult
					SpeechResult speechResult = fmlSpeechs[0];
					writeSpeechResult(speechResult, outBasename, logPrefix);
			}
			// (d.2) global FML output
			{	String logPrefix = String.format("[%s] ", fmlFilename);
				// - list of all marker timestamp
				{	File out = new File(fmlBasename+".allmarkers.json");
					try {
						mapper.writeValue(out, SpeechDataTools.getMarkersTime(null, fmlSpeechs, fmlSSMLs));
						logger.info("{}Output all markers timestamps {} writted", logPrefix, out);
					} catch (Exception e) {
						logger.error("{}Can't writting the all markers timestamps {}: {}", logPrefix, out, e);
					}
				}
				// - list of all visemes
				{	File out = new File(fmlBasename+".allvisemes.json");
					try {
						mapper.writeValue(out, SpeechDataTools.getVisemesList(null, fmlSpeechs, fmlSSMLs));
						logger.info("{}Output all visemes list {} writted", logPrefix, out);
					} catch (Exception e) {
						logger.error("{}Can't writting the all visemes list {}: {}", logPrefix, out, e);
					}
				}
			}
		}
    }
	
	@SuppressWarnings("deprecation")
	private static void writeSSML(SpeechSSML speechSSML, String outBasename, String logPrefix) {
		// - SSML output
		{	File out = new File(outBasename+".ssml");
			try {
				speechSSML.writeToFile(out, true, Charsets.UTF_8);
				logger.info("{}SSML file {} writted", logPrefix, out);
			} catch (IOException e) {
				logger.error("{}Error writing the SSML file {}: {}", logPrefix, out, e.getMessage());
			}
		}
	}

	protected static void writeSpeechResult(SpeechResult speechResult, String outBasename
			, String logPrefix
			) {
		// - Wav output
		{	File out = new File(outBasename+".wav");
			long bytes = SpeechDataTools.writeSoundFile(speechResult, out, true);
			if (bytes<0) logger.error("{}Can't writting the waw file {}", logPrefix, out); 
			else logger.info("{}Output wav file {} writted, {} bytes", logPrefix, out, bytes);
		}
		// - Raw output (allow md5sum test)
		{	File out = new File(outBasename+".raw");
			long bytes = SpeechDataTools.writeSoundFile(speechResult, out, false);
			if (bytes<0) logger.error("{}Can't writting the raw file {}", logPrefix, out); 
			else logger.info("{}Output raw file {} writted, {} bytes", logPrefix, out, bytes);
		}
		// - list of marker timestamp
		{	File out = new File(outBasename+".markers.json");
		 	Map<String,Double> markers = SpeechDataTools.getMarkersTime(null, speechResult, null);  
			try {
				mapper.writeValue(out, markers);
				logger.info("{}Output ({}) markers timestamps {} writted", logPrefix, markers.size(), out);
			} catch (Exception e) {
				logger.error("{}Can't writting the ({}) markers timestamps {}: {}", logPrefix, markers.size(), out, e);
			}
		}
		// - list of visemes
		{
			File out = new File(outBasename+".visemes.json");
			ContinuousEventIfce[] visemes = speechResult.getVisemeEvents(); 
			try {
				mapper.writeValue(out, visemes);
				logger.info("{}Output ({}) visemes list {} writted", logPrefix, visemes.length, out);
			} catch (Exception e) {
				logger.error("{}Can't writting the ({}) visemes list {}: {}", logPrefix, visemes.length, out, e);
			}
			
		}
		/*// - TMP: list of visemes with offset (test ContinuousEventIfce copy)
		{
			File out = new File(outBasename+".visemes+100.json");
			List<ContinuousEventIfce> visemes = SpeechDataTools.getVisemesList(null, speechResult, 100.); 
			try {
				mapper.writeValue(out, visemes);
				logger.info("{}Output ({}) visemes+100 list {} writted", logPrefix, visemes.size(), out);
			} catch (Exception e) {
				logger.error("{}Can't writting the ({}) visemes+10 list {}: {}", logPrefix, visemes.size(), out, e);
			}
			
		}*/
	}
	
}
