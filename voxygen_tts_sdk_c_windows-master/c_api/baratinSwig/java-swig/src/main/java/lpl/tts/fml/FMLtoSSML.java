package lpl.tts.fml;

import java.nio.charset.Charset;
import java.nio.charset.IllegalCharsetNameException;
import java.nio.charset.UnsupportedCharsetException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.lang3.SystemUtils;

/**
 * Tools class to transform FML/BML into SSML
 *
 */
public class FMLtoSSML {
	
	//------------- The XML Declaration -------------//
	// <?xml version="1.0" encoding="ISO-8859-1" ?>

	/** Pattern to match the XML declaration (with the encoding). */
	protected static final Pattern XML_DECL_RE=Pattern.compile("<\\?xml(?:.*?\\?>)", Pattern.DOTALL);
	
	/** Pattern to match the encoding in the XML declaration.
	 * The encoding is in the group-name 'encoding'.
	 */
	protected static final Pattern ENCODING_IN_XML_DECL_RE=Pattern.compile("encoding=(['\"])(?<encoding>.*?)\\1");

	/**
	 * Get the &lt?xml ?&gt; declaration
	 * @param encoding	(optional) the encoding
	 * @return	a xml declaration, with encoding if any.
	 */
	public static String xmlDeclaration(Charset encoding) {
		StringBuffer sb = new StringBuffer("<?xml version=\"1.0\"");
		if (encoding!=null)
			sb.append(" encoding=\"").append(encoding.name()).append('"');
		sb.append(" ?>");
		return sb.toString();
	}
	
	/**
	 * Get the &lt?xml ?&gt; declaration
	 * @param encoding	(optional) the encoding
	 * @return	a xml declaration, with encoding if any.
	 */
	public static byte[] xmlDeclarationBytes(Charset encoding) {
		String xmlDecl = xmlDeclaration(encoding);
		return encoding==null ? xmlDecl.getBytes() : xmlDecl.getBytes(encoding);
	}
	
	/**
	 * Get the &lt?xml ?&gt; declaration
	 * @param encoding	(optional) the encoding
	 * @return	a xml declaration, with encoding if any.
	 */
	public static String xmlDeclaration(String encoding) {
		StringBuffer sb = new StringBuffer("<?xml version=\"1.0\"");
		if (encoding!=null && !encoding.isEmpty())
			sb.append(" encoding=\"").append(encoding).append('"');
		sb.append(" ?>");
		return sb.toString();
	}
	
	//------------- FML <speech> --> SSML <speak> -------------//

	/** Pattern to match the FML &lt;speech&gt; section(s).
	 *  Three named group :
	 *   - open : the open markup
	 *   - inside : the speech content
	 *   - close : the close markup
	 */
	protected static final Pattern SPEECH_PART_RE=Pattern.compile("(?<open>\\<speech(?:.*?)>)(?<inside>.*?)(?<close>\\</speech>)", Pattern.DOTALL);

	/** Pattern to match an <i>id</i> attribute.
	 *  The <i>id</i> value is in the 'value' named group.
	 */
	protected static final Pattern ID_ATTRIB_RE=Pattern.compile("\\bid=([\"'])(?<value>.*?)\\1");

	/** Pattern to match an <i>start</i> attribute.
	 *  The <i>start</i> value is in the 'value' named group.
	 */
	protected static final Pattern START_ATTRIB_RE=Pattern.compile("\\bstart=([\"'])(?<value>.*?)\\1");
	
	/** Pattern to match an <i>end</i> attribute.
	 *  The <i>end</i> value is in the 'value' named group.
	 */
	protected static final Pattern END_ATTRIB_RE=Pattern.compile("\\bend=([\"'])(?<value>.*?)\\1");

	/** Pattern to match the (useless) &lt;description&gt; section inside &lt;speech&gt;
	 */
	protected static final Pattern SPEECH_DESC_RE=Pattern.compile("\\s*<description\\b.*?>.*?</description\\s*>\\s*", Pattern.DOTALL);
	
	/** Pattern to match the &lt;pitchaccent&gt; information(s).
	 * @todo	They are currently simply discarded, TODO: convert them into SSML information
	 */
	protected static final Pattern PITCH_ACCENT_RE=Pattern.compile("\\s*<pitchaccent\\b.*?/>\\s*", Pattern.DOTALL);
	/** Pattern to match the &lt;boundary&gt; information(s).
	 * @todo	They are currently simply discarded, TODO: convert them into SSML information
	 */
	protected static final Pattern BOUNDARY_RE=Pattern.compile("\\s*<boundary\\b.*?/>\\s*", Pattern.DOTALL);
	
	/** Pattern to match the &lt;tm&gt; marker inside &lt;speech&gt; */
	protected static final Pattern SPEECH_TM_RE=Pattern.compile("<tm\\b(.*?)\\bid=([\"'])(?<id>.*?)\\2(.*?)/>", Pattern.DOTALL);
	/** Replacement string for SPEECH_TM_RE in SSML */
	protected static final String SPEECH_TM_SSML_REPLACE="<mark name=\"${id}\"/>";
	
	/** Pattern to match the &lt;voice&gt; open element */
	protected static final Pattern VOICE_OPEN_RE=Pattern.compile("<voice\\b(.*?)\\bemotion=([\"'])(?<emotion>.*?)\\2(.*?)>", Pattern.DOTALL);
	
	public static String SPEAK_EOL=SystemUtils.IS_OS_WINDOWS ? "\r\n" : "\n"; // EOL added after/before <speak> open/close markup

	/** &lt;speak&gt; (default) open mark-up.*/
	public static final String SPEAK_OPEN="<speak"
		+" version=\"1.0\""
		+" xmlns=\"http://www.w3.org/2001/10/synthesis\""
		//TMP Voxygen
	    +" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
	    +" xsi:schemaLocation=\"http://www.w3.org/2001/10/synthesis"
	      +" http://www.w3.org/TR/speech-synthesis/synthesis.xsd"
	      +" http://www.w3.org/2005/01/pronunciation-lexicon"
	      +" http://www.w3.org/TR/pronunciation-lexicon/pls.xsd"
	      +"\""
	    +" xmlns:vox=\"http://www.voxygen.fr/tts\""
	    +" xml:lang=\"fr-FR\""
		+">"
	    +SPEAK_EOL
	    ;

	/** &lt;/speak&gt; close mark-up.*/
	public static final String SPEAK_CLOSE=
			SPEAK_EOL+
			"</speak>";
	
	/**
	 * Return the &lt;speak&gt; open mark-up.
	 * @param speechOpen	(optional) the FML &lt;speech&gt; open mark-up
	 * @return	a &lt;speak&gt; open mark-up.
	 */
	public String speakOpen(String speechOpen) { return SPEAK_OPEN;}
	
	/**
	 * Append the &lt;speak&gt; open mark-up.
	 * @param sb	(optional) a StringBuilder
	 * @param speechOpen	(optional) the FML &lt;speech&gt; open mark-up
	 */
	public StringBuilder appendSpeakOpen(StringBuilder sb, String speechOpen) {
		if (sb==null) sb = new StringBuilder();
		return sb.append(this.speakOpen(speechOpen));
	}
	
	/**
	 * Return the &lt;speak&gt; close mark-up.
	 * @param speechClose	(optional) the FML &lt;speech&gt; close mark-up
	 * @return	a &lt;speak&gt; close mark-up.
	 */
	public String speakClose(String speechClose) { return SPEAK_CLOSE;}
	
	/**
	 * Append the &lt;speak&gt; close mark-up.
	 * @param sb	(optional) a StringBuilder
	 * @param speechClose	(optional) the FML &lt;speech&gt; close mark-up
	 */
	public StringBuilder appendSpeakClose(StringBuilder sb, String speechClose) {
		if (sb==null) sb = new StringBuilder();
		return sb.append(this.speakClose(speechClose));
	}
	
	/**
	 * Append the &lt;speak&gt; close mark-up.
	 * @param sb	(optional) a StringBuilder
	 * @param speechInside	the FML &lt;speech&gt; inside
	 */
	public StringBuilder appendSpeakInside(StringBuilder sb, String speechInside) {
		if (sb==null) sb = new StringBuilder();
		
		// (a) clean the inside
		String speakInside =  cleanSpeechInside(speechInside);
		// (b) replace the <tm id="id"/> --> <mark name="id"/> 
		speakInside = replaceTMtoMark(speakInside);
		// (c) replace <voice>
		speakInside = replaceVoice(speakInside);
		// (d) process extra info: pitchaccent, boundary, etc.
		speakInside = processInfo(speakInside);
				
		return sb.append(speakInside);
	}
	
	public String cleanSpeechInside(String speechInside) {
		String res = speechInside;
		// clean <description>
		res = SPEECH_DESC_RE.matcher(res).replaceAll("");
		
		return res.trim();
	}
	
	public String replaceTMtoMark(String speechInside) {
		String res = speechInside;
		// replace <tm> --> marker
		res = SPEECH_TM_RE.matcher(res).replaceAll(SPEECH_TM_SSML_REPLACE);
		
		return res;
	}
	
	//TMP
	public String processInfo(String speechInside) {
		String res = speechInside;
		//TMP remove <pitchaccent .../>
		res = PITCH_ACCENT_RE.matcher(res).replaceAll("");
		//TMP remove <boundary .../>
		res = BOUNDARY_RE.matcher(res).replaceAll("");
		
		return res;
	}
	
	
	//TMP: special CereProc --> Voxygen
	public String replaceVoice(String speechInside) {
		
	    Matcher matcher = VOICE_OPEN_RE.matcher(speechInside);
		StringBuffer output = new StringBuffer();
	    while (matcher.find()) {
	    	StringBuilder rep = new StringBuilder("<voice");
	    	rep.append(" name=\"");
	    	rep.append(cereProcEmotion2VoxygenName(matcher.group("emotion")));
	    	rep.append('"');
	    	rep.append('>');
	        matcher.appendReplacement(output, rep.toString());
	    }
	    matcher.appendTail(output);
		
		return output.toString();
	}
	
	public static final String VOXYGEN_VOICE_DEFAULT = "Fabienne";
	public static final Map<String,String> CEREPROC_EMOTION_TO_VOXYGEN_VOICE = new HashMap<String,String>();
	static {
		CEREPROC_EMOTION_TO_VOXYGEN_VOICE.put("calm", "Fabienne_fatigue");
		CEREPROC_EMOTION_TO_VOXYGEN_VOICE.put("cross", "Fabienne_tendu");
		//CEREPROC_EMOTION_TO_VOXYGEN_VOICE.put("sad", "Fabienne_tendu");
		//CEREPROC_EMOTION_TO_VOXYGEN_VOICE.put("happy", "Fabienne");
	}
	
	
	public String cereProcEmotion2VoxygenName(String emotion) {
		if (CEREPROC_EMOTION_TO_VOXYGEN_VOICE.containsKey(emotion))
			return CEREPROC_EMOTION_TO_VOXYGEN_VOICE.get(emotion);
		return VOXYGEN_VOICE_DEFAULT;
	}
	
	
	public static final SpeechSSML[] NO_SSML = {};
	
	/**
	 * Convert a FML into an (array of) SpeechSSML.
	 * @param fmlContent	the FML content
	 * @return
	 */
	public SpeechSSML[] fmlToSSML(String fmlContent) {
		ArrayList<SpeechSSML> result = new ArrayList<SpeechSSML>();
		
		// xml declaration/encoding
		Charset fmlEncoding = fmlEncoding(fmlContent);
		
		// extract the <speech> element(s)
		Matcher mSpeech = SPEECH_PART_RE.matcher(fmlContent);
		while (mSpeech.find()) { // iterate on speech elements
			// Convert to a SpeechSSML
			result.add(speechSSML(
					new SpeechSSML(fmlEncoding) // set the original FML encoding
					, mSpeech.group("open"), mSpeech.group("inside"), mSpeech.group("close")
					));
		}
		
		return result.isEmpty() ? NO_SSML : result.toArray(NO_SSML);
	}
	
	public static Charset fmlEncoding(String fmlContent) {
		Matcher mDecl = FMLtoSSML.XML_DECL_RE.matcher(fmlContent);
		if (mDecl.find()) {
			Matcher mEnc = FMLtoSSML.ENCODING_IN_XML_DECL_RE.matcher(mDecl.group());
			if (mEnc.find()) {
				try {
					return Charset.forName(mEnc.group("encoding"));
				} catch (IllegalCharsetNameException e) {
					//TODO: logger
				} catch (UnsupportedCharsetException e) {
					//TODO: logger
				}
			}// else
		} // else
		return null; // default to null
	}
	
	
	/**
	 * Create/adapt a SpeechSSML for this speech element.
	 * @param speechSSML	(optional)	a SpeechSSML.
	 * 	The method create a new one if <code>null</code>
	 * @param speechOpen	the &lt;speech&gt; open mark-up
	 * @param speechInside	the &lt;speech&gt; inside
	 * @param speechClose	the &lt;speech&gt; close mark-up
	 */
	protected SpeechSSML speechSSML(SpeechSSML speechSSML,
			String speechOpen, String speechInside, String speechClose
			) {
		// init speechSSML
		if (speechSSML==null) speechSSML = new SpeechSSML();
		
		// extract id/start attributes
		Matcher mId = ID_ATTRIB_RE.matcher(speechOpen);
		speechSSML.setSpeechId(mId.find() ? mId.group("value") : null); 
		Matcher mStart = START_ATTRIB_RE.matcher(speechOpen);
		speechSSML.setSpeechStart(mStart.find() ? mStart.group("value") : null); 

		// Converting ...
		StringBuilder sb = new StringBuilder();
		// TODO: - append the XML declaration
		// - append the <speak> open mark-up 
		appendSpeakOpen(sb, speechOpen);
		// - append the <speak> inside
		appendSpeakInside(sb, speechInside);
		// - append the <speak> close mark-up 
		appendSpeakClose(sb, speechClose);
		speechSSML.setSpeakElement(sb.toString());
		
		return speechSSML;
	}
	
}
