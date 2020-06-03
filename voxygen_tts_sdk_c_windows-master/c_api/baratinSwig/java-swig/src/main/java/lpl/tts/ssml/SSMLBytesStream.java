package lpl.tts.ssml;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.charset.Charset;

/**
 * A class that can write SSML content's bytes into an OutputStream (FileOutputStream, ByteArrayOutputStream, ...).
 */
public interface SSMLBytesStream {
	/**
	 * Get the (default) encoding.
	 * This is the default encoding for writeTo() methods (except the 3-parameters).
	 * @return An explicit encoding for writeTo() methods
	 *   or <code>null</code> (=> use the machine default and no encoding in the xml declaration)
	 */
	Charset getEncoding();

	/**
	 * Write the SSML byte[]
	 *   with the &lt;?xml ... ?&gt; declaration
	 *   using the default encoding (if any).
	 * @throws IOException
	 * @return	number of written bytes
	 */
	int writeTo(OutputStream out) throws IOException;

	/**
	 * A SSMLBytesStream with the possibility to manage the xml declaration and encoding. 
	 *
	 */
	public interface SSMLBytesStreamPlus extends SSMLBytesStream {

		/**
		 * Write the SSML byte[]
		 *  using a specific encoding.
		 * @param withXmlDecl	prepend the &lt;?xml ... ?&gt; declaration (with encoding).
		 * @param enc	(optional) explicit SSML encoding.
		 * 	If <code>null</code> the default machine encoding will be used.
		 * @throws IOException
		 * @return	number of written bytes
		 */
		int writeTo(OutputStream out, boolean withXmlDecl, Charset enc) throws IOException;

		/**
		 * Write the SSML byte[]
		 *  using the default encoding (if any).
		 * @param withXmlDecl	prepend the &lt;?xml ... ?&gt; declaration (with encoding).
		 * @throws IOException
		 * @return	number of written bytes
		 */
		int writeTo(OutputStream out, boolean withXmlDecl) throws IOException;
	}

}