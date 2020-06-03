package lpl.tools;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.IOUtils;
import org.apache.commons.io.output.ByteArrayOutputStream;

public class ByteArrays {

	/** The null byte.*/
	public static final byte NULL_BYTE = 0;


	/**
	 * Copy the content of a file into a ByteArrayOutputStream 
	 * @param output (optional) a ByteArrayOutputStream
	 * @param input	the input to read
	 * @return the output (or a new ByteArrayOutputStream) with the content of the file.
	 */
	public static ByteArrayOutputStream copyInputToByteArray(ByteArrayOutputStream output, final InputStream input)
		throws IOException
	{	// based on IOUtils.toByteArray()
		if (output==null) output = new ByteArrayOutputStream();
		IOUtils.copy(input, output);
		return output;
	}
	
	/**
	 * Copy the content of a file into a ByteArrayOutputStream 
	 * @param output (optional) a ByteArrayOutputStream
	 * @param file	the file to read
	 * @return the output (or a new ByteArrayOutputStream) with the content of the file.
	 */
	public static ByteArrayOutputStream copyFileToByteArray(ByteArrayOutputStream output, final File file)
		throws IOException
	{	// based on FileUtils.readFileToByteArray() and IOUtils.toByteArray()
		if (output==null) output = new ByteArrayOutputStream();
		InputStream in = null;
		try {
			in = FileUtils.openInputStream(file);
			// IOUtils.toByteArray()
			IOUtils.copy(in, output);
			return output;
		} finally {
			IOUtils.closeQuietly(in);
		}
	}
	
	/**
	 * Append a null byte at the end of the ByteArrayOutputStream
	 * @param output	(optional) a ByteArrayOutputStream
	 * @return
	 */
	public static ByteArrayOutputStream nullTerminate(ByteArrayOutputStream output) {
		if (output==null) output = new ByteArrayOutputStream();
		 output.write(ByteArrays.NULL_BYTE); // append the \0
		 return output;
	}

	/**
	 * Copy the content of a file into a byte[] 
	 * @param file	the file to read
	 * @param nullTerminated	return a null-terminated byte[] (i.e. append \0 at end of the file content)
	 */
	public static byte[] readFileBytes(final File file, boolean nullTerminated)
		throws IOException
	{
		ByteArrayOutputStream output = copyFileToByteArray(null, file);
		if (nullTerminated) output.write(ByteArrays.NULL_BYTE); // append the \0
		return output.toByteArray();
	}
	/**
	 * Copy the content of an InputStream into a byte[] 
	 * @param input	the InputStream
	 * @param nullTerminated	return a null-terminated byte[] (i.e. append \0 at end of the file content)
	 */
	public static byte[] readInputBytes(final InputStream input, boolean nullTerminated)
		throws IOException
	{
		ByteArrayOutputStream output = copyInputToByteArray(null, input);
		if (nullTerminated) output.write(ByteArrays.NULL_BYTE); // append the \0
		return output.toByteArray();
	}
}
