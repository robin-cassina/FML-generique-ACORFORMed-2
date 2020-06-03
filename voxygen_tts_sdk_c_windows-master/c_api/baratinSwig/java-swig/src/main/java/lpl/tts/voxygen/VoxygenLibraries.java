package lpl.tts.voxygen;

import java.io.File;

import org.apache.commons.lang3.SystemUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class VoxygenLibraries {
	/** The VOXYGEN library/config root path.
	 * 	- DLL(s) are in VOXYGEN_PATH/Win{32,64}
	 *  - Configuration are in VOXYGEN_PATH/config
	 */
	public static final String VOXYGEN_PATH="Common/Lib/External/Voxygen/";
	/** The VOXYGEN config file. */
	public static final String CFG_FILE=VOXYGEN_PATH+"config/baratinoo.cfg";
	
	public static Logger logger = LoggerFactory.getLogger(VoxygenLibraries.class);

    public static boolean loadVoxygenLibraries(File root) {
    	try {
    		// (a) Get the Voxygen libraries path
    		File voxygenLibDir = getVoxygenLibDirectory(root);
    		if (!voxygenLibDir.exists() || !voxygenLibDir.isDirectory()) {
    			logger.error("The Voxygen libraries path {} didn't exist or isn't a directory", voxygenLibDir.getAbsolutePath());
    			return false;
    		}
    		// (b) Load the Voxygen "baratinoo" DLL
    		File baratinooLibPath = new File(voxygenLibDir, getBaratinooLibFilename());
    		try {
    			System.load(baratinooLibPath.getAbsolutePath());
    			logger.debug("Baratinoo library {} loaded", baratinooLibPath);
    		} catch (SecurityException e) {
    			logger.error("Security exception loading the Voxygen's Baratinoo library", e);
    			return false;
    		} catch (UnsatisfiedLinkError e) {
    			logger.error("UnsatisfiedLinkError loading the Voxygen's Baratinoo library", e);
    			return false;
    		}
    		// (c) Load the Swig wrap for the Voxygen "baratinoo" DLL
    		File swigLibPath = new File(voxygenLibDir, VoxygenLibraries.getBaratinooSwigLibFilename());
    		try {
    			System.load(swigLibPath.getAbsolutePath());
    			logger.debug("Swig wrapper for the baratinoo library {} loaded", swigLibPath);
    		} catch (SecurityException e) {
    			logger.error("Security exception loading the Swig wrapper for the baratinoo library", e);
    			return false;
    		} catch (UnsatisfiedLinkError e) {
    			logger.error("UnsatisfiedLinkError loading the Swig wrapper for the baratinoo library", e);
    			return false;
    		}
    		
    	} catch (Exception e) {
    		logger.error("Error when loading Voxygen libraries", e);
    		return false;
    	}
    	return true;
    }
    
    
    /**
     * Get the Voxygen shared library directory
     *  based on the OS (Windows only) and the JVM architecture (32/64 bit) 
     * @param root	(optional) A root directory to append the Voxygen directory.
     * 	<code>null</code> => from the current (working) directory.  
     * @return
     */
    public static File getVoxygenLibDirectory(File root) {
    	File voxygenDir = new File(root, VOXYGEN_PATH); 
    	if (SystemUtils.IS_OS_WINDOWS) {
    		return new File(voxygenDir, VoxygenLibraries.isJVM64bit() ? "Win64" : "Win32"); 
    	} else {
    		logger.error("Only Windows system are currently supported by the Voxygen DLLs");
    		throw new UnsupportedOperationException("Voxygen DLL only for Windows plateforms");
    	}
    }
    
    public static String getBaratinooLibFilename() {
    	if (SystemUtils.IS_OS_WINDOWS) {
    		return "libbaratinoo.dll";
    	} else {//TODO:other systems
    		logger.error("Only Windows system are currently supported by the Voxygen DLLs");
    		throw new UnsupportedOperationException("Voxygen DLL only for Windows plateforms");
    	}
    }
    

	public static String getBaratinooSwigLibFilename() {
		if (SystemUtils.IS_OS_WINDOWS) {
			return "baratinSwig.dll";
		} else {//TODO:other systems
			logger.error("Only Windows system are currently supported by the Voxygen DLLs");
			throw new UnsupportedOperationException("Voxygen DLL only for Windows plateforms");
		}
	}

	
	///----- TMP : Tools ----//
	
	/**
	 * Say if the JVM is a 64 bit platform.
	 */
	public static boolean isJVM64bit() {
		// (a) Try with "sun.arch.data.model", see http://stackoverflow.com/questions/2062020/how-can-i-tell-if-im-running-in-64-bit-jvm-or-32-bit-jvm-from-within-a-program
		try {	// nota: not supported in all JVM
			String sunArchDataModel = System.getProperty("sun.arch.data.model");
			if ("64".equals(sunArchDataModel)) return true;
			if ("32".equals(sunArchDataModel)) return false;
		} catch (Exception e) { }
		// (b) Try with "os.arch" (that is the JVM arch), see http://stackoverflow.com/questions/10846105/all-possible-values-os-arch-in-32bit-jre-and-in-64bit-jre
		try {
			String osName = System.getProperty("os.arch");
			if (osName.endsWith("64")) return true;	// x86_64, amd64, ppc64, ...
		} catch (Exception e) { }
		return false;
	}

}
