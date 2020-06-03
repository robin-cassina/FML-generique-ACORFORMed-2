package lpl.tts.fml;


import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.Charset;

import org.apache.commons.io.Charsets;
import org.junit.rules.ExternalResource;

import lpl.tools.ByteArrays;

/**
 * Class to get some files as resource in JUnit tests. 
 * FROM : http://stackoverflow.com/questions/2597271/easy-way-to-get-a-test-file-into-junit
 */
public class ResourceFile extends ExternalResource
{
	// The resource name
    String res;
    // The (default) resource charset
    @SuppressWarnings("deprecation")
	Charset charset = Charsets.UTF_8;
    
    // the resource file
    File file = null;
    
    // the resource input stream
    InputStream stream;

    /**
     * Create a new resource with its name.
     * Default charset is UTF-8.
     * @param res	the resource path (from ResourceFile package).
     */
    public ResourceFile(String res) {
        this.res = res;
    }

    /**
     * Create a new resource with its name and charset.
     * @param res	the resource path (from ResourceFile package).
     * @param charset the resource charset
     */
    public ResourceFile(String res, Charset charset) {
        this.res = res;
        this.charset = charset;
    }
    
    /**
     * Get the File associate to this resource.
     * @return
     * @throws IOException
     */
    public File getFile() throws IOException {
        if (file == null) {
            createFile();
        }
        return file;
    }

    /**
     * Get an InputStream associate to this resource.
     * @see #before()
     */
    public InputStream getInputStream() {
        return stream;
    }

    
    /**
     * Create an InputStream for this resource.
     */
    public InputStream createInputStream() {
        return getClass().getResourceAsStream(res); //TODO: choose the class (or sub-class ResourceFile)
    }

    /**
     * Get the resource content (default charset).
     * @return
     * @throws IOException
     */
    public String getContent() throws IOException {
        return getContent(charset);
    }

    /**
     * Get the resource content with an explicit charset.
     * @param charSet
     * @return
     * @throws IOException
     */
    public String getContent(String charSet) throws IOException {
    	return getContent(Charset.forName(charSet));
    }

    /**
     * Get the resource content with an explicit charset.
     * @param charSet
     * @return
     * @throws IOException
     */
    public String getContent(Charset charSet) throws IOException {
        InputStreamReader reader = new InputStreamReader(createInputStream(), charSet);
        char[] tmp = new char[4096];
        StringBuilder b = new StringBuilder();
        try {
            while (true) {
                int len = reader.read(tmp);
                if (len < 0) {
                    break;
                }
                b.append(tmp, 0, len);
            }
            reader.close();
        } finally {
            reader.close();
        }
        return b.toString();
    }

    /**
     * Get the resource bytes.
	 * @param nullTerminated	return a null-terminated byte[] (i.e. append \0 at end of the file content)
     * @return
     * @throws IOException
     */
    public byte[] getBytes(boolean nullTerminated) throws IOException {
    	//return IOUtils.toByteArray(createInputStream());
    	return ByteArrays.readInputBytes(createInputStream(), nullTerminated);
    }

    @Override
    protected void before() throws Throwable {
        super.before();
        //stream = getClass().getResourceAsStream(res); //TODO: choose the class (or sub-class ResourceFile)
    }

    @Override
    protected void after() {
        //try { stream.close(); } catch (IOException e) { } // ignore
        if (file != null) {
            file.delete();
        }
        super.after();
    }

    /**
     * Create a new local file with the resource content.
     * @throws IOException
     */
    private void createFile() throws IOException {
        file = new File(".",res);
        InputStream stream = getClass().getResourceAsStream(res);
        try {
            file.createNewFile();
            FileOutputStream ostream = null;
            try {
                ostream = new FileOutputStream(file);
                byte[] buffer = new byte[4096];
                while (true) {
                    int len = stream.read(buffer);
                    if (len < 0) {
                        break;
                    }
                    ostream.write(buffer, 0, len);
                }
            } finally {
                if (ostream != null) {
                    ostream.close();
                }
            }
        } finally {
            stream.close();
        }
    }

}