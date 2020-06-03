package lpl.tts.fml;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.Writer;
import java.nio.charset.Charset;

import org.apache.commons.lang3.SystemUtils;

import lpl.tts.ssml.SSMLBytes.SSMLBytesPlus;
import lpl.tts.ssml.SSMLBytesStream.SSMLBytesStreamPlus;
import lpl.tts.ssml.SSMLBytesStreamWrapper.SSMLBytesStreamPlusWrapper;
import lpl.tts.ssml.SSMLNullTermBytes.SSMLNullTermBytesPlus;
import lpl.tts.ssml.SSMLNullTermBytesStreamWrapper.SSMLNullTermBytesStreamPlusWrapper;

/**
 * Class that represent the SSML content representing a FML &lt;speech&gt; elements.
 *
 */
public class SpeechSSML
	implements SSMLBytesStreamPlus	// write byte[] into OutputStream (File, ByteArrayOutputStream)
		, SSMLNullTermBytesPlus	// getNullTermBytes() : null-terminated byte[] => C/C++ char*
		, SSMLBytesPlus	// getBytes() : byte[]
{
	
	
	public static String XML_DECL_EOL=SystemUtils.IS_OS_WINDOWS ? "\r\n" : "\n"; // EOL added after the XML declaration
	public static String XML_END_OF_FILE=SystemUtils.IS_OS_WINDOWS ? "\r\n" : "\n"; // END OF FILE added after the <speak> element
	
	
	/** Value of the &lt;speech&gt; element's <i>id</i> attribute (if any).*/
	public String speechId=null;
	/** Value of the &lt;speech&gt; element's <i>start</i> attribute (if any).*/
	public String speechStartAtt=null;
	/** Double value of the &lt;speech&gt; element's <i>start</i> attribute (if any).*/
	protected double speechStart=0.0;
	
	/** The (original FML) encoding or <code>null</code>. */
	protected Charset encoding=null;
	
	/** The SSML (root) &lt;speak&gt; element.*/
	protected String speakElement=null;
	
	/**
	 * Constructor
	 */
	public SpeechSSML() {
		super();
	}

	public SpeechSSML(Charset encoding) {
		super();
		this.encoding = encoding;
	}



	/**
	 * Constructor
	 * @param speechId	(optinal) the speech id attribute
	 * @param speechStartAtt	(optional) the speech start attribute
	 */
	public SpeechSSML(String speechId, String speechStartAtt) {
		super();
		this.speechId = speechId;
		this.setSpeechStart(speechStartAtt);
	}

	/**
	 * Constructor
	 * @param speechId	(optinal) the speech id attribute
	 * @param speechStart	the speech start value
	 */
	public SpeechSSML(String speechId, double speechStart) {
		super();
		this.speechId = speechId;
		this.setSpeechStart(speechStart);
	}
	public String getSpeechId() {
		return speechId;
	}

	public void setSpeechId(String speechId) {
		this.speechId = speechId;
	}

	/** Get the <i>start</i> attribute (string).
	 * @return the <i>start</i> attribute (string) or <code>null</code> if undefined.
	 */
	public String getSpeechStartAtt() {
		return speechStartAtt;
	}

	/** Get the <i>start</i> double value.
	 * @return the <i>start</i> double value (<code>0.0</code> by default).
	 */
	public double getSpeechStart() {
		return speechStart;
	}

	/**
	 * Set the <i>start</i> attribute and double value.
	 * @param speechStartAtt	(optional) the start attribute 
	 */
	public void setSpeechStart(String speechStartAtt) {
		this.speechStartAtt = speechStartAtt;
		if (speechStartAtt==null) this.speechStart=0.;
		else {
			try {
				this.speechStart = Double.parseDouble(speechStartAtt);
			} catch (NumberFormatException e) {
				this.speechStart = 0.;
				this.speechStartAtt = null;
			}
		}
	}

	/**
	 * Set the <i>start</i> double value and attribute.
	 * @param speechStart	the start value 
	 */
	public void setSpeechStart(double speechStart) {
		this.speechStart = speechStart;
		this.speechStartAtt = Double.toString(speechStart);
	}

	
	/**
	 * Get the (original FML explicit) encoding if any.
	 * This is the default encoding for writeTo()/getBytes()/getNullTermBytes() methods.
	 */
	@Override 	//SSMLNullTermBytes, SSMLBytesStream, SSMLBytes
	public Charset getEncoding() {
		return encoding;
	}

	/**
	 * Set the (original FML) encoding.
	 * @param encoding
	 */
	protected void setEncoding(Charset encoding) {
		this.encoding = encoding;
	}

	/**
	 * Get the &lt;speak&gt; element (SSML root element).
	 */
	public String getSpeakElement() {
		return speakElement;
	}

	/**
	 * Set the &lt;speak&gt; element (SSML root element).
	 */
	protected void setSpeakElement(String speakContent) {
		this.speakElement = speakContent;
	}

	/* (non-Javadoc)
	 * @see lpl.tts.ssml.SSMLBytesStream#writeTo(java.io.OutputStream, boolean, java.nio.charset.Charset)
	 */
	@Override	//SSMLBytesStream
	public int writeTo(OutputStream out, boolean withXmlDecl, Charset enc)
			throws IOException
	{
		int written=0;
		// xml decl
		if (withXmlDecl) {
			byte[] xmlDeclBytes = FMLtoSSML.xmlDeclarationBytes(enc);
			out.write(xmlDeclBytes); written+=xmlDeclBytes.length;
			if (XML_DECL_EOL!=null && !XML_DECL_EOL.isEmpty()) {
				byte[] xmlDeclEOLBytes = enc==null ? XML_DECL_EOL.getBytes() : XML_DECL_EOL.getBytes(enc);
				out.write(xmlDeclEOLBytes); written+=xmlDeclEOLBytes.length;
			}
		}
		// the <speak> (root) element
		byte[] speakBytes = enc==null ? speakElement.getBytes() : speakElement.getBytes(enc);
		out.write(speakBytes); written+=speakBytes.length;
		// the end of file
		if (XML_END_OF_FILE!=null && !XML_END_OF_FILE.isEmpty()) {
			byte[] xmlEOFBytes = enc==null ? XML_END_OF_FILE.getBytes() : XML_END_OF_FILE.getBytes(enc);
			out.write(xmlEOFBytes); written+=xmlEOFBytes.length;
		}

		return written;
	}
	
	/**
	 * Write the SSML string into a Writer.
	 * @param withXmlDecl	prepend the &lt;?xml ... ?&gt; declaration (with encoding).
	 * @param enc	(optional) encoding in the xml declaration
	 * 	If <code>null</code> no explicit encoding.
	 * @throws IOException
	 * @return	number of written characters
	 */
	public int writeTo(Writer out, boolean withXmlDecl, Charset enc)
			throws IOException
	{
		int written=0;
		// xml decl
		if (withXmlDecl) {
			String xmlDecl = FMLtoSSML.xmlDeclaration(enc);
			out.write(xmlDecl); written+=xmlDecl.length();
			if (XML_DECL_EOL!=null && !XML_DECL_EOL.isEmpty()) {
				out.write(XML_DECL_EOL); written+=XML_DECL_EOL.length();
			}
		}
		// the <speak> (root) element
		out.write(speakElement); written+=speakElement.length();
		// the end of file
		if (XML_END_OF_FILE!=null && !XML_END_OF_FILE.isEmpty()) {
			out.write(XML_END_OF_FILE); written+=XML_END_OF_FILE.length();
		}

		return written;
	}
	
	/* (non-Javadoc)
	 * @see lpl.tts.ssml.SSMLBytesStream#writeTo(java.io.OutputStream, boolean)
	 */
	@Override	//SSMLBytesStream
	public int writeTo(OutputStream out, boolean withXmlDecl)
			throws IOException
	{
		return writeTo(out, withXmlDecl, this.encoding);
	}

	/* (non-Javadoc)
	 * @see lpl.tts.ssml.SSMLBytesStream#writeTo(java.io.OutputStream)
	 */
	@Override	//SSMLBytesStream
	public int writeTo(OutputStream out)
			throws IOException
	{
		return writeTo(out, true, this.encoding);
	}
	
	public void writeToFile(File out,  boolean withXmlDecl, Charset enc)
			throws IOException
	{
		OutputStream output= new BufferedOutputStream(new FileOutputStream(out));
		try {
			this.writeTo(output, withXmlDecl, enc);
		} finally {
			output.close();
		}
	}

	/**
	 * Get the SSML byte[]
	 *  using a specific encoding.
	 * @param withXmlDecl	prepend the &lt;?xml ... ?&gt; declaration (with encoding).
	 * @param enc	(optional) explicit SSML encoding.
	 * 	If <code>null</code> the default machine encoding will be used.
	 * @return the bytes for this SSML using the charset.
	 * 	nota: It's the exact bytes content (proper to write it into a file).
	 *        If you need a null-terminated byte[] (to pass it to a C/C++ function) use {@link #getNullTermBytes(boolean, Charset)} instead.
	 * @see #getNullTermBytes(boolean, Charset)
	 */
	@Override	//SSMLBytes
	public byte[] getBytes(boolean withXmlDecl, Charset enc) {
		return SSMLBytesStreamPlusWrapper.getBytesFromByteStream(this, withXmlDecl, enc);
	}
	
	/**
	 * Get the SSML byte[]
	 *  using a the original FML encoding (if any).
	 * @param withXmlDecl	prepend the &lt;?xml ... ?&gt; declaration (with encoding).
	 * @return the bytes for this SSML
	 * 	nota: It's the exact bytes content (proper to write it into a file).
	 *        If you need a null-terminated byte[] (to pass it to a C/C++ function) use {@link #getNullTermBytes(boolean)} instead.
	 * @see #getNullTermBytes(boolean)
	 */
	@Override	//SSMLBytes
	public byte[] getBytes(boolean withXmlDecl) {
		return getBytes(withXmlDecl, this.encoding);
	}
	
	/**
	 * Get the SSML byte[]
	 *  with the &lt;?xml ... ?&gt; declaration
	 *   using the original FML encoding (if any).
	 * @return the bytes for this SSML
	 * 	nota: It's the exact bytes content (proper to write it into a file).
	 *        If you need a null-terminated byte[] (to pass it to a C/C++ function) use {@link #getNullTermBytes()} instead.
	 * @see #getNullTermBytes()
	 */
	@Override	//SSMLBytes
	public byte[] getBytes() {
		return getBytes(true, this.encoding);
	}
	
	/**
	 * Get the SSML byte[] <strong>terminated by null</code>
	 *  using a specific encoding.
	 * @param withXmlDecl	prepend the &lt;?xml ... ?&gt; declaration (with encoding).
	 * @param enc	(optional) explicit SSML encoding.
	 * 	If <code>null</code> the default machine encoding will be used.
	 * @return the bytes for this SSML plus a null byte.
	 * 	nota: This method produce a null-terminated byte[] (to pass it to a C/C++ function).
	 *        If you only need the byte[] content (to write in into a file) use {@link #getBytes(boolean, Charset)} instead.
	 * @see #getBytes(boolean, Charset)
	 */
	@Override	//SSMLNullTermBytes
	public byte[] getNullTermBytes(boolean withXmlDecl, Charset enc) {
		return SSMLNullTermBytesStreamPlusWrapper.getNullTermBytesFromByteStream(this, withXmlDecl, enc);
	}
	
	/**
	 * Get the SSML byte[] <strong>terminated by null</code>
	 *  using a the original FML encoding (if any).
	 * @param withXmlDecl	prepend the &lt;?xml ... ?&gt; declaration (with encoding).
	 * @return the bytes for this SSML plus a null byte.
	 * 	nota: This method produce a null-terminated byte[] (to pass it to a C/C++ function).
	 *        If you only need the byte[] content (to write in into a file) use {@link #getBytes(boolean)} instead.
	 * @see #getBytes(boolean)
	 */
	@Override	//SSMLNullTermBytes
	public byte[] getNullTermBytes(boolean withXmlDecl) {
		return getNullTermBytes(withXmlDecl, this.encoding);
	}
	
	/**
	 * Get the SSML byte[] <strong>terminated by null</code>
	 *  with the &lt;?xml ... ?&gt; declaration
	 *   using the original FML encoding (if any).
	 * @return the bytes for this SSML plus a null byte.
	 * 	nota: This method produce a null-terminated byte[] (to pass it to a C/C++ function).
	 *        If you only need the byte[] content (to write in into a file) use {@link #getBytes()} instead.
	 * @see #getBytes()
	 */
	@Override	//SSMLNullTermBytes
	public byte[] getNullTermBytes() {
		return getNullTermBytes(true, this.encoding);
	}
	
}
