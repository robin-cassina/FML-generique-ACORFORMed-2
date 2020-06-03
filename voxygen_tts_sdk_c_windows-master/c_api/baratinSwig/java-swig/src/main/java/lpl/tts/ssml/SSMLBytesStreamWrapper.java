package lpl.tts.ssml;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.charset.Charset;

import lpl.tts.ssml.SSMLBytesStream.SSMLBytesStreamPlus;


/**
 * Class to wrap a SSMLBytesStream into a SSMLBytes
 *
 */
public class SSMLBytesStreamWrapper<T extends SSMLBytesStream>
implements SSMLBytes
{
	protected final T ssmlByteStream;


	public SSMLBytesStreamWrapper(T ssmlByteStream) {
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
	public static byte[] getBytesFromByteStream(SSMLBytesStream bs) {
		ByteArrayOutputStream bytes = new ByteArrayOutputStream();
		try {
			bs.writeTo(bytes);
			return bytes.toByteArray();
		} catch (IOException e) {
			return null;
		}
	}

	@Override
	public byte[] getBytes() {
		return getBytesFromByteStream(this.ssmlByteStream);
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
	public static class SSMLBytesStreamPlusWrapper<T extends SSMLBytesStreamPlus>
	extends SSMLBytesStreamWrapper<T>
	implements SSMLBytesPlus
	{
		public SSMLBytesStreamPlusWrapper(T ssmlByteStream) {
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
		public static byte[] getBytesFromByteStream(SSMLBytesStreamPlus bs, boolean withXmlDecl, Charset enc) {
			ByteArrayOutputStream bytes = new ByteArrayOutputStream();
			try {
				bs.writeTo(bytes, withXmlDecl, enc);
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
		public static byte[] getBytesFromByteStream(SSMLBytesStreamPlus bs, boolean withXmlDecl) {
			ByteArrayOutputStream bytes = new ByteArrayOutputStream();
			try {
				bs.writeTo(bytes, withXmlDecl);
				return bytes.toByteArray();
			} catch (IOException e) {
				return null;
			}
		}

		@Override
		public byte[] getBytes(boolean withXmlDecl, Charset enc) {
			return getBytesFromByteStream(this.ssmlByteStream, withXmlDecl, enc);
		}

		@Override
		public byte[] getBytes(boolean withXmlDecl) {
			return getBytesFromByteStream(this.ssmlByteStream, withXmlDecl);
		}

	}

}
