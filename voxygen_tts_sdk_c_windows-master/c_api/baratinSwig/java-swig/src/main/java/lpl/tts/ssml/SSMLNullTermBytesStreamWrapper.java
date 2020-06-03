package lpl.tts.ssml;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.charset.Charset;

import lpl.tts.ssml.SSMLBytesStream.SSMLBytesStreamPlus;

/**
 * Class to wrap a SSMLBytesStream into a SSMLNullTermBytes
 *
 */
public class SSMLNullTermBytesStreamWrapper<T extends SSMLBytesStream>
implements SSMLNullTermBytes
{
	protected final T ssmlByteStream;


	public SSMLNullTermBytesStreamWrapper(T ssmlByteStream) {
		super();
		this.ssmlByteStream = ssmlByteStream;
	}

	/**
	 * Get the SSML byte[] <strong>terminated by null</code>
	 *  from a SSMLBytesStream
	 * @return the bytes for this SSML plus a null byte.
	 * 	nota: This method produce a null-terminated byte[] (to pass it to a C/C++ function).
	 *        If you only need the byte[] content (to write in into a file) use {@link #getBytes(boolean, Charset)} instead.
	 * @see #getBytes(boolean, Charset)
	 */
	public static byte[] getNullTermBytesFromByteStream(SSMLBytesStream bs) {
		ByteArrayOutputStream bytes = new ByteArrayOutputStream();
		try {
			bs.writeTo(bytes);
			bytes.write(0);// append a null byte to terminate
			return bytes.toByteArray();
		} catch (IOException e) {
			return null;
		}
	}

	@Override
	public byte[] getNullTermBytes() {
		return getNullTermBytesFromByteStream(this.ssmlByteStream);
	}

	@Override
	public Charset getEncoding() {
		return this.ssmlByteStream.getEncoding();
	}

	/**
	 * Get the wrapped SSMLBytesStream
	 * @return
	 */
	public T getSSMLBytesStream() {
		return this.ssmlByteStream;
	}


	/**
	 * Class to wrap a SSMLBytesStreamPlus into a SSMLBytesPlus
	 *
	 */
	public static class SSMLNullTermBytesStreamPlusWrapper<T extends SSMLBytesStreamPlus>
	extends SSMLNullTermBytesStreamWrapper<T>
	implements SSMLNullTermBytesPlus
	{

		public SSMLNullTermBytesStreamPlusWrapper(T ssmlByteStream) {
			super(ssmlByteStream);
		}

		/**
		 * Get the SSML byte[] <strong>terminated by null</code>
		 *  from a SSMLBytesStream
		 * @param withXmlDecl	prepend the &lt;?xml ... ?&gt; declaration (with encoding).
		 * @param enc	(optional) explicit SSML encoding.
		 * 	If <code>null</code> the default machine encoding will be used.
		 * @return the bytes for this SSML plus a null byte.
		 * 	nota: This method produce a null-terminated byte[] (to pass it to a C/C++ function).
		 *        If you only need the byte[] content (to write in into a file) use {@link #getBytes(boolean, Charset)} instead.
		 * @see #getBytes(boolean, Charset)
		 */
		public static byte[] getNullTermBytesFromByteStream(SSMLBytesStreamPlus bs, boolean withXmlDecl, Charset enc) {
			ByteArrayOutputStream bytes = new ByteArrayOutputStream();
			try {
				bs.writeTo(bytes, withXmlDecl, enc);
				bytes.write(0);// append a null byte to terminate
				return bytes.toByteArray();
			} catch (IOException e) {
				return null;
			}
		}

		/**
		 * Get the SSML byte[] <strong>terminated by null</code>
		 *  from a SSMLBytesStream
		 * @param withXmlDecl	prepend the &lt;?xml ... ?&gt; declaration (with encoding).
		 * @return the bytes for this SSML plus a null byte.
		 * 	nota: This method produce a null-terminated byte[] (to pass it to a C/C++ function).
		 *        If you only need the byte[] content (to write in into a file) use {@link #getBytes(boolean, Charset)} instead.
		 * @see #getBytes(boolean, Charset)
		 */
		public static byte[] getNullTermBytesFromByteStream(SSMLBytesStreamPlus bs, boolean withXmlDecl) {
			ByteArrayOutputStream bytes = new ByteArrayOutputStream();
			try {
				bs.writeTo(bytes, withXmlDecl);
				bytes.write(0);// append a null byte to terminate
				return bytes.toByteArray();
			} catch (IOException e) {
				return null;
			}
		}


		@Override
		public byte[] getNullTermBytes(boolean withXmlDecl, Charset enc) {
			return getNullTermBytesFromByteStream(this.ssmlByteStream, withXmlDecl, enc);
		}

		@Override
		public byte[] getNullTermBytes(boolean withXmlDecl) {
			return getNullTermBytesFromByteStream(this.ssmlByteStream, withXmlDecl);
		}
	}
}
