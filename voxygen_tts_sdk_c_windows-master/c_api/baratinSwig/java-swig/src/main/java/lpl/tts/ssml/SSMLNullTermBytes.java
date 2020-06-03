package lpl.tts.ssml;

import java.nio.charset.Charset;

/**
 * A class that can get/read the SSML content's bytes in a <strong>null-terminated</strong> byte[].
 */
public interface SSMLNullTermBytes {
	/**
	 * Get the (default) encoding.
	 * This is the default encoding for getNullTermBytes() methods (except the 2-parameters).
	 * @return An explicit encoding for getNullTermBytes() methods
	 *   or <code>null</code> (=> use the machine default and no encoding in the xml declaration)
	 */
	Charset getEncoding();
	
	/**
	 * Get the SSML byte[] <strong>terminated by null</code>
	 *  with the &lt;?xml ... ?&gt; declaration
	 *   using the original FML encoding (if any).
	 * @return the bytes for this SSML plus a null byte.
	 * 	nota: This method produce a null-terminated byte[] (to pass it to a C/C++ function).
	 *        If you only need the byte[] content (to write in into a file)
	 *         use a {@link SSMLBytes} instead.
	 * @see SSMLBytes#getBytes()
	 */
	byte[] getNullTermBytes();

	/**
	 * A SSMLNullTermBytes with the possibility to manage the xml declaration and encoding. 
	 *
	 */
	public interface SSMLNullTermBytesPlus extends SSMLNullTermBytes {
		/**
		 * Get the SSML byte[] <strong>terminated by null</code>
		 *  using a specific encoding.
		 * @param withXmlDecl	prepend the &lt;?xml ... ?&gt; declaration (with encoding).
		 * @param enc	(optional) explicit SSML encoding.
		 * 	If <code>null</code> the default machine encoding will be used.
		 * @return the bytes for this SSML plus a null byte.
		 * 	nota: This method produce a null-terminated byte[] (to pass it to a C/C++ function).
		 *        If you only need the byte[] content (to write in into a file)
		 *         use a {@link SSMLBytes} instead.
		 * @see SSMLBytes#getBytes(boolean, Charset)
		 */
		public byte[] getNullTermBytes(boolean withXmlDecl, Charset enc);

		/**
		 * Get the SSML byte[] <strong>terminated by null</code>
		 *  using a the original FML encoding (if any).
		 * @param withXmlDecl	prepend the &lt;?xml ... ?&gt; declaration (with encoding).
		 * @return the bytes for this SSML plus a null byte.
		 * 	nota: This method produce a null-terminated byte[] (to pass it to a C/C++ function).
		 *        If you only need the byte[] content (to write in into a file)
		 *         use a {@link SSMLBytes} instead.
		 * @see SSMLBytes#getBytes(boolean)
		 */
		public byte[] getNullTermBytes(boolean withXmlDecl);
	}
}