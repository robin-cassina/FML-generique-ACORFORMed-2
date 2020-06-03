package lpl.tts.ssml;

import java.nio.charset.Charset;

public interface SSMLBytes {

	/**
	 * Get the (default) encoding.
	 * This is the default encoding for getBytes() methods (except the 2-parameters).
	 * @return An explicit encoding for getBytes() methods
	 *   or <code>null</code> (=> use the machine default and no encoding in the xml declaration)
	 */
	Charset getEncoding();

	/**
	 * Get the SSML byte[]
	 *  with the &lt;?xml ... ?&gt; declaration
	 *   using the original FML encoding (if any).
	 * @return the bytes for this SSML
	 * 	nota: It's the exact bytes content (proper to write it into a file).
	 *        If you need a null-terminated byte[] (to pass it to a C/C++ function)
	 *         use a {@link SSMLNullTermBytes} instead.
	 * @see SSMLNullTermBytes#getNullTermBytes()
	 */
	byte[] getBytes();


	/**
	 * A SSMLBytes with the possibility to manage the xml declaration and encoding. 
	 *
	 */
	public interface SSMLBytesPlus extends SSMLBytes {

		/**
		 * Get the SSML byte[]
		 *  using a specific encoding.
		 * @param withXmlDecl	prepend the &lt;?xml ... ?&gt; declaration (with encoding).
		 * @param enc	(optional) explicit SSML encoding.
		 * 	If <code>null</code> the default machine encoding will be used.
		 * @return the bytes for this SSML using the charset.
		 * 	nota: It's the exact bytes content (proper to write it into a file).
		 *        If you need a null-terminated byte[] (to pass it to a C/C++ function)
		 *         use a {@link SSMLNullTermBytes} instead.
		 * @see SSMLNullTermBytes#getNullTermBytes(boolean, Charset)
		 */
		byte[] getBytes(boolean withXmlDecl, Charset enc);

		/**
		 * Get the SSML byte[]
		 *  using a the original FML encoding (if any).
		 * @param withXmlDecl	prepend the &lt;?xml ... ?&gt; declaration (with encoding).
		 * @return the bytes for this SSML
		 * 	nota: It's the exact bytes content (proper to write it into a file).
		 *        If you need a null-terminated byte[] (to pass it to a C/C++ function)
		 *         use a {@link SSMLNullTermBytes} instead.
		 * @see SSMLNullTermBytes#getNullTermBytes(boolean)
		 */
		byte[] getBytes(boolean withXmlDecl);

	}
}