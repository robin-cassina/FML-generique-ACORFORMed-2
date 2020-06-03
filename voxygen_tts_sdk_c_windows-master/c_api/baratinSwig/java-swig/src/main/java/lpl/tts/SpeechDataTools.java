package lpl.tts;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import lpl.tools.ContinuousEventImpl;
import lpl.tools.ContinuousEventIfce;
import lpl.tools.TimeStampEventIfce;
import lpl.tts.fml.SpeechSSML;

public class SpeechDataTools {
	public static Logger logger = LoggerFactory.getLogger(SpeechDataTools.class);

	/**
	 * Write the SpeechData data in an OutputStream
	 * @param speechResult	(required) the SpeechData
	 * @param output	(required) the output
	 * @param withHeader	write the header data.
	 * @return	number of written bytes.
	 * @throws IOException
	 */
	public static long writeSound(SpeechData speechResult, OutputStream output, boolean withHeader)
		throws IOException
	{
		assert speechResult!=null;
		assert output!=null;
		long bytes = 0;
		if (withHeader) {
			output.write(speechResult.getHeader());	// write the header
			bytes += speechResult.getHeaderLength();
		}
		output.write(speechResult.getRawData());	// write the raw data
		bytes += speechResult.getRawDataLength();
		return bytes;
	}

	/**
	 * Write the SpeechData data into an File
	 * @param speechResult	(required) the SpeechData
	 * @param file	(required) the output file
	 * @param withHeader	write the header data.
	 * @return	number of written bytes
	 *  or a negative number if an error occurs. 
	 */
	public static long writeSoundFile(SpeechData speechResult, File file, boolean withHeader)
	{
		assert speechResult!=null;
		assert file!=null;
		long bytes = 0;
		try {
			OutputStream output = new BufferedOutputStream(new FileOutputStream(file));
			try {
				bytes += writeSound(speechResult, output, withHeader);
			} finally {
				output.close();	// this can generate an IOException too
			}
	     	return bytes;
		} catch(FileNotFoundException e){
	     	logger.error("Can't find the sound output file {}.", file);
		} catch(IOException e){
			logger.error("Error writting the sound output file:", e);
	    }
	    return -1-bytes;
	}

	/**
	 * Write the SpeechData header into an File
	 * @param speechResult	(required) the SpeechData
	 * @param output	(required) the output
	 * @return	number of written bytes.
	 * @throws IOException
	 */
	public static long writeHeader(SpeechData speechResult, OutputStream output)
		throws IOException
	{
		assert speechResult!=null;
		assert output!=null;
		output.write(speechResult.getHeader());	// write the header
		return speechResult.getHeaderLength();
	}

	/**
	 * Write the SpeechData header into an File
	 * @param speechResult	(required) the SpeechData
	 * @param file	(required) the output file
	 * @return	number of written bytes.
	 *  or a negative number if an error occurs. 
	 */
	public static long writeHeaderInFile(SpeechData speechResult, File file)
	{
		assert speechResult!=null;
		assert file!=null;
		long bytes = 0;
		try {
			OutputStream output = new BufferedOutputStream(new FileOutputStream(file));
			try {
				bytes += writeHeader(speechResult, output);
			} finally {
				output.close();	// this can generate an IOException too
			}
			return bytes;
		} catch(FileNotFoundException e){
	     	logger.error("Can't find the sound output file {}.", file);
		} catch(IOException e){
			logger.error("Error writting the sound output file:", e);
	    }
	    return -1-bytes;
	}
	
	/**
	 * Get a marker timestamp mapping
	 * @param dest (optional) the destination mapping
	 * 	if <code>null</code> create a LinkedHashMap => order is the markers in the input (SSMLs and so the timestamp order)
	 * @param speechResult	(required) a single SpeechData
	 * @param prefix	(optional) a prefix for the marker name
	 * @param startTime	an offset time to apply to each timestamp
	 * @return	a mapping between marker's name and timestamp
	 */
	public static Map<String,Double> getMarkersTime(Map<String,Double> dest 
			, SpeechData speechResult
			, String prefix
			, double startTime
			) {
		if (dest==null) dest = new LinkedHashMap<String,Double>();
		for (TimeStampEventIfce e : speechResult.getMarkerEvents()) {
			String mark = prefix==null ? e.getName() : prefix+e.getName(); // append prefix
			double time = startTime + e.getMillisecond(); // correct timestamp with startTime  
			dest.put(mark, time);
		}
		return dest;
	}
	/**
	 * Get a marker timestamp mapping
	 * @param dest (optional) the destination mapping
	 * 	if <code>null</code> create a LinkedHashMap => order is the markers in the input (SSMLs and so the timestamp order)
	 * @param speechResult	(required) a single SpeechData
	 * @param prefix	(optional) a prefix for the marker name
	 * @return	a mapping between marker's name and timestamp
	 */
	public static Map<String,Double> getMarkersTime(Map<String,Double> dest 
			, SpeechData speechResult
			, String prefix
			) {
		return getMarkersTime(dest, speechResult, prefix, 0.);
	}

	/**
	 * Get a marker timestamp mapping for an array of SpeechData (result of the array of SSMLs).
	 * 
	 * The array of SSMLs give the prefix ("id:") and the startTime of each SpeechData
	 * 
	 * @param dest (optional) the destination mapping
	 * 	if <code>null</code> create a LinkedHashMap => order is the markers in the SSMLs (and so the timestamp order)
	 * @param speechResults	(required) an array of SpeechData
	 * @param speechSSMLs	(required) the corresponding array of SpeechSSMLs
	 * @return	a mapping between marker's name and timestamp
	 */
	public static Map<String,Double> getMarkersTime(Map<String,Double> dest 
			, SpeechData[] speechResults
			, SpeechSSML[] speechSSMLs
			) {
		assert speechResults!=null;
		assert speechSSMLs!=null;
		assert speechSSMLs.length >= speechResults.length;
		 //Map<String,Float> dest = null;
		double lastEnd=0;
		for (int i=0; i<speechResults.length; ++i) {
			 // the prefix based on <speech id="">
			 String id = speechSSMLs[i].getSpeechId();
			 String prefix = (id==null||id.isEmpty()) ? null : id+":";
			 // the start time
			 double startTime = speechSSMLs[i].getSpeechStart();//TODO: check if it's the start attribute or another ('speech offset') that determine the start time
			 if (startTime==0) startTime = lastEnd;
			 dest = getMarkersTime(dest, speechResults[i] , prefix , startTime);
			 lastEnd = startTime + speechResults[i].getDuration();// adjust start time
		 }
		 return dest;
	}

	/**
	 * Get the (reindexed) visemes list for a SpeechData
	 * @param dest (optional) the destination list
	 * 	if <code>null</code> create a ArrayList => order is the markers in the viseme order
	 * @param speechResult	(required) a single SpeechData
	 * @param startTime	a millisecond offset time to apply to each timestamp (only the getMilisecond() result)
	 * @return	the list of visemes
	 */
	public static List<ContinuousEventIfce> getVisemesList(List<ContinuousEventIfce> dest 
			, SpeechData speechResult
			, double startTime
			) {
		if (dest==null) dest = new ArrayList<ContinuousEventIfce>();
		if (startTime!=0) {
			for (ContinuousEventIfce e : speechResult.getVisemeEvents()) {
				ContinuousEventImpl copy = new ContinuousEventImpl(e);
				copy.setMillisecond((float) (e.getMillisecond()+startTime));
				dest.add(copy);
			}
		} else {
			for (ContinuousEventIfce e : speechResult.getVisemeEvents()) dest.add(e);
		}
		return dest;
	}

	/**
	 * Get the (reindexed) visemes list for an array of SpeechData (result of the array of SSMLs).
	 * 
	 * The array of SSMLs give the startTime of each SpeechData
	 * 
	 * @param dest (optional) the destination list
	 * 	if <code>null</code> create a ArrayList => order is the markers in the viseme order
	 * @param speechResults	(required) an array of SpeechData
	 * @param speechSSMLs	(required) the corresponding array of SpeechSSMLs
	 * @return	the list of visemes
	 */
	public static List<ContinuousEventIfce> getVisemesList(List<ContinuousEventIfce> dest 
			, SpeechData[] speechResults
			, SpeechSSML[] speechSSMLs
			) {
		assert speechResults!=null;
		assert speechSSMLs!=null;
		assert speechSSMLs.length >= speechResults.length;
		double lastEnd=0;
		for (int i=0; i<speechResults.length; ++i) {
			// the start time
			double startTime = speechSSMLs[i].getSpeechStart();//TODO: check if it's the start attribute or another ('speech offset') that determine the start time
			if (startTime==0) startTime = lastEnd;
			dest = getVisemesList(dest, speechResults[i], startTime);
			lastEnd = startTime + speechResults[i].getDuration();// adjust start time
		}
		return dest;
	}
}
