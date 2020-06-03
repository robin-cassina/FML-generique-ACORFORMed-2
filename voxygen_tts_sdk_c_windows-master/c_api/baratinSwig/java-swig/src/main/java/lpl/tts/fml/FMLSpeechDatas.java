package lpl.tts.fml;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.charset.Charset;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;

import org.apache.commons.io.Charsets;
import org.apache.commons.io.FileUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.fasterxml.jackson.databind.ObjectMapper;

import lpl.tools.ContinuousEventIfce;
import lpl.tts.SpeechData;
import lpl.tts.SpeechDataTools;
import lpl.tts.ssml.SSMLNullTermBytes;
//import lpl.tts.voxygen.ContinuousEvent;
import lpl.tts.voxygen.VoxSSMLtoSpeech;

/**
 * A class to group the SSML and speech results of an FML.
 *
 */
public class FMLSpeechDatas {
	public static Logger logger = LoggerFactory.getLogger(FMLSpeechDatas.class);

	// [1] The FML content

	/** The FML content.*/
	protected final String fmlContent;
	
	// [2] The SSML(s) corresponding to FML's &lt;speech&gt; elements
	/** The SSML(s) corresponding to FML's &lt;speech&gt; elements.*/
	protected SpeechSSML[] fmlSSMLs = null;
	
	/** A FML to SSMLs converter.*/
	protected FMLtoSSML fmlToSSML = null;
	
	/** A default FML to SSMLs converter.*/
	public static final FMLtoSSML DEFAULT_FMLTOSSML = new FMLtoSSML();

	// [3] The SpeeResult(s) corresponding to FML's &lt;speech&gt; elements.
	
	/** The SpeeResult(s) corresponding to FML's &lt;speech&gt; elements.*/
	protected SpeechData[] fmlSpeechs = null;

	/** A SSML to Speech converter.*/
	protected VoxSSMLtoSpeech ssmlToSpeech = null;
	
	/*- A default FML to SSMLs converter.*/
	//public static final VoxSSMLtoSpeech DEFAULT_SSMLTOSPEECH = new FMLtoSSML();
	protected static VoxSSMLtoSpeech defaultSSMLtoSpeech=null;

	//--------- [1] the FML content -------------//
	
	/**
	 * A constructor using the FML content.
	 * @param fmlContent
	 */
	public FMLSpeechDatas(String fmlContent) {
		super();
		this.fmlContent = fmlContent;
	}
	
	
	/**
	 * A constructor reading a FML file. 
	 * @param fmlContent
	 */
	public FMLSpeechDatas(File fml, Charset encoding)
			throws IOException
	{
		this(FileUtils.readFileToString(fml, encoding));
	}
	
	/** Get the FML content.*/
	public String getFmlContent() {
		return fmlContent;
	}

	//--------- [2] FML --> SSMLs conversion -------------//
	
	/**
	 * Convert the FML's &lt;speech&gt; element(s) into SSML(s).
	 * @param fmlToSSML	the FML to SSML converter.
	 * @return	the number of resulting SSMLs (i.e. the number of &lt;speech&gt; element(s))
	 */
	public SpeechSSML[] convertToSSMLs(FMLtoSSML fmlToSSML) {
		assert fmlToSSML!=null;
		this.fmlToSSML = fmlToSSML; // save the fmlToSSML
		fmlSSMLs = fmlToSSML.fmlToSSML(fmlContent);
		fmlSpeechs = null; // reset
		return fmlSSMLs;
	}

	/**
	 * Convert the FML's &lt;speech&gt; element(s) into SSML(s)
	 *  using the defined/default FML to SSML converter.
	 * @return	the number of resulting SSMLs (i.e. the number of &lt;speech&gt; element(s))
	 */
	public SpeechSSML[] convertToSSMLs() {
		return convertToSSMLs(this.fmlToSSML==null ? DEFAULT_FMLTOSSML : this.fmlToSSML);
	}
	
	/** Get the various SSMLs generate for this FML.
	 * @throws IllegalStateException when method is called before the FML to SSMLs conversion.
	 */
	public SpeechSSML[] getSSMLs()
		throws IllegalStateException
	{
		if (fmlSSMLs==null) throw new IllegalStateException("Must convert FML into SSMLs before");
		return fmlSSMLs;
	}

	
	/** Say if the FML content has &lt;speech&gt; element(s).*/
	public boolean hasSpeechElements() {
		// Yet converted into SSML(s) 
		if (fmlSSMLs!=null) return fmlSSMLs.length>0;
		// ELSE
		Matcher mSpeech = FMLtoSSML.SPEECH_PART_RE.matcher(fmlContent);
		return mSpeech.find();		
	}

	/** Say if the FML content has various &lt;speech&gt; element(s).*/
	public boolean hasVariousSpeechElements() {
		// Yet converted into SSML(s) 
		if (fmlSSMLs!=null) return fmlSSMLs.length>1;
		// ELSE
		Matcher mSpeech = FMLtoSSML.SPEECH_PART_RE.matcher(fmlContent);
		return mSpeech.find() && mSpeech.find();		
	}

	/** Get the number of &lt;speech&gt; element(s).*/
	public int countSpeechElements() {
		// Yet converted into SSML(s) 
		if (fmlSSMLs!=null) return fmlSSMLs.length;
		// ELSE
		Matcher mSpeech = FMLtoSSML.SPEECH_PART_RE.matcher(fmlContent);
		int count=0; while (mSpeech.find()) {count++;}
		return count;
	}
	
	//--------- [3] SSMLs --> SpeechData conversion -------------//
	
	/**
	 * Convert the SSMLs into Speech.
	 * @param fmlToSSML	a FML to SSML converter.
	 * @return	the number of resulting SSMLs (i.e. the number of &lt;speech&gt; element(s))
	 */
	public int convertToSpeech(VoxSSMLtoSpeech ssmlToSpeech) {
		assert ssmlToSpeech!=null;
		this.ssmlToSpeech = ssmlToSpeech; // record the SSML to Speech converter
		// (2) convert to SSML(s) first
		if (fmlSSMLs==null) this.convertToSSMLs();
		// (3) convert SSMLs into Speech 
		fmlSpeechs =  new SpeechData[fmlSSMLs.length];
		for (int i=0; i<fmlSSMLs.length; ++i) {
			fmlSpeechs[i] = ssmlToSpeech.ssmlToSpeech((SSMLNullTermBytes) fmlSSMLs[i]); 
		}		
		return fmlSpeechs.length;
	}

	/**
	 * Convert the SSMLs into Speech.
	 * @param fmlToSSML	a FML to SSML converter.
	 * @return	the number of resulting SSMLs (i.e. the number of &lt;speech&gt; element(s))
	 */
	public int convertToSpeech() {
		return this.convertToSpeech(this.ssmlToSpeech==null ? defaultSSMLtoSpeech() : this.ssmlToSpeech);
	}
	
	/** Get a default SSMLtoSpeech. */
	protected static VoxSSMLtoSpeech defaultSSMLtoSpeech() {
		if (defaultSSMLtoSpeech==null) {
			// (A) Create a VoxSSMLtoSpeech
			defaultSSMLtoSpeech = new VoxSSMLtoSpeech();
			// (B) Adapt the configuration
			//TODO
			// (C) Init the engine
			try {
				defaultSSMLtoSpeech.init();
			} catch (IllegalStateException e) {
				System.err.println("Can't initialize the TTS Engine: "+e);
			}	
		}
		return defaultSSMLtoSpeech;
	}
	
	// get individual speech results, marker, visemes, etc.

	/** Get the various SpeechData generate for this FML.
	 * @throws IllegalStateException when method is called before the FML to speeches conversion.
	 */
	protected SpeechData[] getSpeechDatas()
		throws IllegalStateException
	{
		checkSpeechDatas(true);
		return fmlSpeechs;
	}

	
	/** Check the SpeechDatas state, i.e. fmlSpeechs exist.*/
	protected boolean checkSpeechDatas(boolean throwException)
		throws IllegalStateException
	{
		if (fmlSpeechs==null) {
			if (throwException) throw new IllegalStateException("Must convert FML into speech before");
			return false;
		}
		return true;
	}
	/** Check the SpeechDatas index is OK.*/
	protected boolean checkSpeechDataIndex(int index, boolean throwException)
		throws IllegalStateException, IndexOutOfBoundsException
	{
		if (! checkSpeechDatas(throwException)) return false;
		if (index<0 || index>=fmlSpeechs.length) {
			if (throwException) throw new IndexOutOfBoundsException("FML content produce "+fmlSpeechs.length+" speech result(s)");
			return false;
		}
		return true;
	}

	/**
	 * Get one of the speech result.
	 * @param index
	 * @throws IllegalStateException when method is called before the FML to speeches conversion.
	 * @throws IndexOutOfBoundsException when index is incorrect
	 */
	protected SpeechData getSpeechDatas(int index)
		throws IllegalStateException, IndexOutOfBoundsException
	{
		checkSpeechDataIndex(index, true);
		return fmlSpeechs[index];
	}
	
	/**
	 * Write the sound data of a speech block to an output stream.
	 * @param index	index of the speech block.
	 * @param output	output stream.
	 * @param withHeader	write the wav header too.
	 * @return	number of written bytes.
	 * @throws IllegalStateException when method is called before the FML to speeches conversion.
	 * @throws IndexOutOfBoundsException when index is incorrect
	 * @throws IOException	Write error.
	 */
	public long writeSound(int index, OutputStream output, boolean withHeader)
		throws IllegalStateException, IndexOutOfBoundsException, IOException
	{
		checkSpeechDataIndex(index, true);
		return SpeechDataTools.writeSound(fmlSpeechs[index], output, withHeader);
	}
	
	/**
	 * Write the sound data of a speech block to an output stream 
	 *  intercepting exceptions.
	 * @param index	index of the speech block.
	 * @param output	output stream.
	 * @param withHeader	write the wav header too.
	 * @return	number of written bytes, or a negative value if error.
	 */
	public long writeSoundNoExeption(int index, OutputStream output, boolean withHeader)
	{
		if (checkSpeechDataIndex(index, false)) {
			try {
				return SpeechDataTools.writeSound(fmlSpeechs[index], output, withHeader);
			} catch (IOException e) {
				logger.warn("IOException writting sound data: "+e.getMessage());
			}
		}
		return -1;
	}
	
	/**
	 * Write the sound data of a speech block to an file.
	 * @param index	index of the speech block.
	 * @param out	output file.
	 * @param withHeader	write the wav header too.
	 * @return	number of written bytes, or a negative value if error.
	 */
	public long writeSoundFile(int index, File out, boolean withHeader)
		//throws IllegalStateException, IndexOutOfBoundsException, IOException
	{
		if (checkSpeechDataIndex(index, false)) {
			return SpeechDataTools.writeSoundFile(fmlSpeechs[index], out, withHeader);
		}
		return -1;
	}
	
	/**
	 * Get the viseme events for a speech block. 
	 * @param index	index of the speech block.
	 * @return	an array (eventually empty, but not <code>null</code>) of ContinuousEventIfce.
	 * @throws IllegalStateException when method is called before the FML to speeches conversion.
	 * @throws IndexOutOfBoundsException when index is incorrect
	 */
	public ContinuousEventIfce[] getVisemeEvents(int index)
		throws IllegalStateException, IndexOutOfBoundsException
	{
		checkSpeechDataIndex(index, true);
		return fmlSpeechs[index].getVisemeEvents();
	}
	
	
	/**
	 * Get the markers time for a speech block.
	 * @param index	index of the speech block.
	 * @param prefix (optional) a prefix to add before the marker id.
	 * @return	a map (eventually empty, but not <code>null</code>) of marker id and corresponding time.
	 * /!\ the times are relative to the speech block, not the full FML.
	 * @throws IllegalStateException when method is called before the FML to speeches conversion.
	 * @throws IndexOutOfBoundsException when index is incorrect
	 * 
	 * @see #getAllMarkersTime()
	 */
	public Map<String,Double> getMarkersTime(int index, String prefix)
		throws IllegalStateException, IndexOutOfBoundsException
	{
		checkSpeechDataIndex(index, true);
		return SpeechDataTools.getMarkersTime(null, fmlSpeechs[index], prefix);
	}
	
	
	// get global markers/visemes
	
	/**
	 * Get the markers timestamp for all speech blocks.
	 * @return a map (eventually empty, but not <code>null</code>) of marker id and corresponding time.
	 * (!) If speech blocks have an id, it prepends the marker key,
	 *      b.e. "s1:tm1" for the marker 'tm1' in the speech block 's1'. 
	 * (!) Times are 'absolute', i.e. duration of 1st speech block is added to second block time, etc. 
	 * @throws IllegalStateException when method is called before the FML to speeches conversion.
	 */
	public Map<String,Double> getAllMarkersTime()
		throws IllegalStateException
	{
		checkSpeechDatas(true);
		return SpeechDataTools.getMarkersTime(null, fmlSpeechs, fmlSSMLs);
	}
	
	/**
	 * Get the viseme's events for all speech blocks
	 * @return a list (eventually empty, but not <code>null</code>) of viseme events.
	 * (!) Times are 'absolute', i.e. duration of 1st speech block is added to second block time, etc. 
	 * @throws IllegalStateException when method is called before the FML to speeches conversion.
	 */
	public List<ContinuousEventIfce> getAllVisemeEvents()
		throws IllegalStateException
	{
		checkSpeechDatas(true);
		return SpeechDataTools.getVisemesList(null, fmlSpeechs, fmlSSMLs);
	}
	
	//--------- [ALL] FML --> SpeechDatas conversion -------------//
	
	public FMLSpeechDatas convert(FMLtoSSML fmlToSSML, VoxSSMLtoSpeech ssmlToSpeech) {
		// (2)
		if (fmlToSSML==null) this.convertToSSMLs(); // defined/default
		else this.convertToSSMLs(fmlToSSML);
		// (3)
		if (ssmlToSpeech==null) this.convertToSpeech(); // defined/default
		else this.convertToSpeech(ssmlToSpeech);
		
		return this;
	}


	public FMLSpeechDatas convert() {
		return convert(this.fmlToSSML, this.ssmlToSpeech);
	}

	//-------- main ------

	// rewrite of VoxSSMLtoSpeech main method, but using FMLSpeechDatas
	
	// JSON writer
	protected static ObjectMapper mapper = new ObjectMapper();

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
		// - create a output directory test-fml2speech/<yyMMdd-HHmm>
		SimpleDateFormat formatter = new SimpleDateFormat("yyMMdd-HHmm");
		File outputDir = new File(String.format("%s/%s","test-fml2speech", formatter.format(new Date())));
		if (!outputDir.exists()) outputDir.mkdirs();
		// run on each FML
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
			// (b) convert into SSML(s) and speech data
			FMLSpeechDatas fmlResults = (new FMLSpeechDatas(fmlContent)) // create the FMLSpeechDatas with the FML content
					.convert(fmlToSSML, ssmlToSpeech); // convert into SSMLs (with fmlToSSML) and into speech data (with ssmlToSpeech)
			int countSpeech = fmlResults.countSpeechElements(); // number of speech blocks
			if (countSpeech==0) { // nothing
				logger.info("FML {} had any speech section", fmlFilename);
				continue;
			} else logger.info("FML {} had {} speech section(s)", fmlFilename, countSpeech);
			
			// (d) write results
			String fmlBasename = outputDir.getPath()+"/"+ // output dir
					fmlFile.getName().replaceAll("\\.\\w+$", "") // filename without extension
					;
			
			// (d.1) individual speech output
			if (countSpeech>1) {
				// in case of various speech blocks, create files with '-<index>' 
				for (int i=0; i<countSpeech; ++i) {
					// numbered basename
					String outBasename = String.format("%s-%d", fmlBasename, i);
					String logPrefix = String.format("[%s[%d]] ", fmlFilename, i);
					// the intermediate SSML
					writeSSML(fmlResults, i, outBasename, logPrefix); //TMP/TODO
					// the SpeechData
					writeSpeechData(fmlResults, i, outBasename, logPrefix);// TMP/TODO
				}
			} else {
				// case of only one speech block 
					// basename
					String outBasename = fmlBasename;
					String logPrefix = String.format("[%s] ", fmlFilename);
					// the intermediate SSML
					writeSSML(fmlResults, 0, outBasename, logPrefix);
					// the current SpeechData
					writeSpeechData(fmlResults, 0, outBasename, logPrefix);
			}
			
			// (d.2) global FML output
			{	String logPrefix = String.format("[%s] ", fmlFilename);
				// - list of all marker timestamp
				{	File out = new File(fmlBasename+".allmarkers.json");
					try {
						mapper.writeValue(out, fmlResults.getAllMarkersTime());
						logger.info("{}Output all markers timestamps {} writted", logPrefix, out);
					} catch (Exception e) {
						logger.error("{}Can't writting the all markers timestamps {}: {}", logPrefix, out, e);
					}
				}
				// - list of all visemes
				{	File out = new File(fmlBasename+".allvisemes.json");
					try {
						mapper.writeValue(out, fmlResults.getAllVisemeEvents());
						logger.info("{}Output all visemes list {} writted", logPrefix, out);
					} catch (Exception e) {
						logger.error("{}Can't writting the all visemes list {}: {}", logPrefix, out, e);
					}
				}
			}
		}
    }
	
	//TMP
	@SuppressWarnings("deprecation")
	private static void writeSSML(FMLSpeechDatas fmlResults, int index, String outBasename, String logPrefix) {
		// - SSML output
		{	File out = new File(outBasename+".ssml");
			try {
				fmlResults.getSSMLs()[index].writeToFile(out, true, Charsets.UTF_8);
				logger.info("{}SSML file {} writted", logPrefix, out);
			} catch (IOException e) {
				logger.error("{}Error writing the SSML file {}: {}", logPrefix, out, e.getMessage());
			}
		}
	}
	
	//TMP
	protected static void writeSpeechData(FMLSpeechDatas fmlResults, int index
			, String outBasename
			, String logPrefix
			) {
		// - Wav output
		{	File out = new File(outBasename+".wav");
			long bytes = fmlResults.writeSoundFile(index, out, true);
			if (bytes<0) logger.error("{}Can't writting the waw file {}", logPrefix, out); 
			else logger.info("{}Output wav file {} writted, {} bytes", logPrefix, out, bytes);
		}
		// - Raw output (allow md5sum test)
		{	File out = new File(outBasename+".raw");
			long bytes = fmlResults.writeSoundFile(index, out, false);
			if (bytes<0) logger.error("{}Can't writting the raw file {}", logPrefix, out); 
			else logger.info("{}Output raw file {} writted, {} bytes", logPrefix, out, bytes);
		}
		// - list of marker timestamp
		{	File out = new File(outBasename+".markers.json");
		 	Map<String,Double> markers = fmlResults.getMarkersTime(index, null);
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
			ContinuousEventIfce[] visemes = fmlResults.getVisemeEvents(index);
			try {
				mapper.writeValue(out, visemes);
				logger.info("{}Output ({}) visemes list {} writted", logPrefix, visemes.length, out);
			} catch (Exception e) {
				logger.error("{}Can't writting the ({}) visemes list {}: {}", logPrefix, visemes.length, out, e);
			}
			
		}
	}
}
