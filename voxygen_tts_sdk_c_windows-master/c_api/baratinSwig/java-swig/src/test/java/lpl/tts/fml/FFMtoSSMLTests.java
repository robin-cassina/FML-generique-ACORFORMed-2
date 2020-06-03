package lpl.tts.fml;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.IOException;
import java.nio.charset.Charset;
import java.util.regex.Matcher;

import org.apache.commons.io.Charsets;
import org.apache.commons.lang3.StringEscapeUtils;
import org.apache.commons.lang3.StringUtils;
import org.apache.commons.lang3.SystemUtils;
import org.junit.Rule;
import org.junit.Test;
//import org.junit.runner.RunWith;
//import org.junit.runners.Suite;
//import org.junit.runners.Suite.SuiteClasses;

//@RunWith(Suite.class)
//@SuiteClasses({})
public class FFMtoSSMLTests {
	@SuppressWarnings("deprecation")
	public static final Charset ISO_8859_1 = Charsets.ISO_8859_1;
	@SuppressWarnings("deprecation")
	public static final Charset UTF_8 = Charsets.UTF_8;
	

	public static final Charset FML1_CHARSET= ISO_8859_1;
	@Rule
    public ResourceFile fml1_nospeech = new ResourceFile("/fml-samples/A01-Incompris-01-silent.xml", FML1_CHARSET);
	public static final String FML1_XML_DECL = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>";
	
	public static final Charset FML2_CHARSET=ISO_8859_1;
	@Rule
    public ResourceFile fml2_calm = new ResourceFile("/fml-samples/A01-Incompris-02-pas_compris.xml", FML2_CHARSET);
	@Rule
    public ResourceFile ssml2_calm = new ResourceFile("/fml-samples/A01-Incompris-02-pas_compris.ssml", FML2_CHARSET);
	public static final String FML2_XML_DECL = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>";
	
	public static final Charset XML2_U8_CHARSET=UTF_8;
	
	/** The EOL value (as GIT would change it with the system). */
	public static final String EOL = SystemUtils.IS_OS_WINDOWS ? "\r\n" : "\n";
	public static final String XML2_U8_CONTENT = ""+
			"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"+EOL+
			"<fml-apml>"+EOL+
			"	<bml>"+EOL+
			"		<speech id=\"s1\" start=\"0.0\" language=\"english\" voice=\"openmary\" type=\"SAPI4\" text=\"\">"+EOL+
			"			<description level=\"1\" type=\"gretabml\">"+EOL+
			"				<reference>tmp/from-fml-apml.pho</reference>"+EOL+
			"			</description>"+EOL+
		    ""+EOL+
			"			<tm id=\"tm1\"/>"+EOL+
			"				<voice emotion='calm'>J'ai pas compris</voice>"+EOL+
			"			<tm id=\"tm2\"/>"+EOL+
			"		</speech>"+EOL+
			"	</bml>"+EOL+
			"	<fml>"+EOL+
			"		<backchannel id=\"bc1\" type=\"refusal\" start=\"s1:tm1\" end=\"s1:tm2\" importance=\"1.0\"/>"+EOL+
		    ""+EOL+
			"		<emotion id=\"e1\" type=\"questioning\" start=\"s1:tm1\" end=\"s1:tm2\" importance=\"1.0\"/>"+EOL+
			"	</fml>"+EOL+
			"</fml-apml>";
	public static final String XML2_U8_XML_DECL = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
	public static final String XML2_SPEECH_OPEN = "<speech id=\"s1\" start=\"0.0\" language=\"english\" voice=\"openmary\" type=\"SAPI4\" text=\"\">";
	public static final String XML2_SPEECH_CLOSE = "</speech>";
	public static final String XML2_SPEECH_INSIDE = EOL+
			"			<description level=\"1\" type=\"gretabml\">"+EOL+
			"				<reference>tmp/from-fml-apml.pho</reference>"+EOL+
			"			</description>"+EOL+
		    ""+EOL+
			"			<tm id=\"tm1\"/>"+EOL+
			"				<voice emotion='calm'>J'ai pas compris</voice>"+EOL+
			"			<tm id=\"tm2\"/>"+EOL+
			"		";
	public static final String XML2_SPEECH_CLEANINSIDE = 
			"<tm id=\"tm1\"/>"+EOL+
			"				<voice emotion='calm'>J'ai pas compris</voice>"+EOL+
			"			<tm id=\"tm2\"/>";
	public static final String XML2_SPEECH_TM2MARK = 
			"<mark name=\"tm1\"/>"+EOL+
			"				<voice emotion='calm'>J'ai pas compris</voice>"+EOL+
			"			<mark name=\"tm2\"/>";
	public static final String XML2_SPEECH_VOXVOICE = 
			"<mark name=\"tm1\"/>"+EOL+
			"				<voice name=\"Fabienne_fatigue\">J'ai pas compris</voice>"+EOL+
			"			<mark name=\"tm2\"/>";

	public static final String XML2_SPEAK = ""
			//TODO "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"+EOL+
			+FMLtoSSML.SPEAK_OPEN
			+"<mark name=\"tm1\"/>"+EOL
			+"				<voice name=\"Fabienne_fatigue\">J'ai pas compris</voice>"+EOL
			+"			<mark name=\"tm2\"/>"
			+FMLtoSSML.SPEAK_CLOSE;
	
	public static final Charset FML3_CHARSET=ISO_8859_1;
	@Rule
    public ResourceFile fml3_normal = new ResourceFile("/fml-samples/A01-Incompris-06-No_understanding-comprends_pas.xml", FML3_CHARSET);
	@Rule
    public ResourceFile ssml3_normal = new ResourceFile("/fml-samples/A01-Incompris-06-No_understanding-comprends_pas.ssml", FML3_CHARSET);
	public static final String FML3_XML_DECL = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>";
	
	public static final String XML3_SPEAK = ""
			//TODO "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"+EOL+
			+FMLtoSSML.SPEAK_OPEN
			+"<mark name=\"tm1\"/>"+EOL
			+"				Je ne comprends pas !"+EOL
			+"			<mark name=\"tm2\"/>"
			+FMLtoSSML.SPEAK_CLOSE;
	
	public static final Charset FML4_CHARSET=ISO_8859_1;
	@Rule
    public ResourceFile fml4_maintenant = new ResourceFile("/fml-samples/P02-03_Terrain-01-On_fait_quoi_maintenant.xml", FML4_CHARSET);
	//@Rule
    //public ResourceFile ssml4_maintenant = new ResourceFile("/fml-samples/P02-03_Terrain-01-On_fait_quoi_maintenant.ssml", FML4_CHARSET);
	
	public static final Charset FML5_CHARSET=ISO_8859_1;
	@Rule
    public ResourceFile fml5_maintenant_multi = new ResourceFile("/fml-samples/P02-03_Terrain-01-On_fait_quoi_maintenant-multi.xml", FML5_CHARSET);
	///------------------------
	
	@Test
	public void testRegex() throws IOException {
		// XML_DECL & ENCODING_IN_XML_DECL
		{ // - xml2ContentU8 
			Matcher xml_m2 = FMLtoSSML.XML_DECL_RE.matcher(XML2_U8_CONTENT);
			assertTrue(xml_m2.find());
			String xml_decl2 = xml_m2.group();
			assertEquals(XML2_U8_XML_DECL, xml_decl2);
			Matcher enc_m2 = FMLtoSSML.ENCODING_IN_XML_DECL_RE.matcher(xml_decl2);
			assertTrue(enc_m2.find());
			String enc2 = enc_m2.group("encoding");
			//assertEquals(Charsets.ISO_8859_1, Charset.forName(enc2));
			assertEquals(XML2_U8_CHARSET, Charset.forName(enc2));
		}
		// SPEECH
		{ // - xml2ContentU8
			Matcher speech_m2 = FMLtoSSML.SPEECH_PART_RE.matcher(XML2_U8_CONTENT);
			assertTrue(speech_m2.find());
			String speech_open_m2 = speech_m2.group("open");
			assertEquals(XML2_SPEECH_OPEN, speech_open_m2);
			String speech_inside_m2 = speech_m2.group("inside");
			assertEquals(XML2_SPEECH_INSIDE, speech_inside_m2);
			String speech_close_m2 = speech_m2.group("close");
			assertEquals(XML2_SPEECH_CLOSE, speech_close_m2);
		}
	}
	
	@Test
	public void testRegexOnFile() throws IOException {
		// [0] Get the content of files
		String fml1_content = fml1_nospeech.getContent();
		String fml2_content = fml2_calm.getContent();
		String fml3_content = fml3_normal.getContent();
		
		FMLtoSSML fmlToSSML = new FMLtoSSML();
		
		// XML_DECL & ENCODING_IN_XML_DECL
		{ // - fml1
			Matcher xml_m1 = FMLtoSSML.XML_DECL_RE.matcher(fml1_content);
			assertTrue(xml_m1.find());
			String xml_decl1 = xml_m1.group();
			assertEquals(FML1_XML_DECL, xml_decl1);
			Matcher enc_m1 = FMLtoSSML.ENCODING_IN_XML_DECL_RE.matcher(xml_decl1);
			assertTrue(enc_m1.find());
			String enc1 = enc_m1.group("encoding");
			assertEquals(FML1_CHARSET, Charset.forName(enc1));
		}
		{ // - fml2
			Matcher xml_m2 = FMLtoSSML.XML_DECL_RE.matcher(fml2_content);
			assertTrue(xml_m2.find());
			String xml_decl2 = xml_m2.group();
			assertEquals(FML2_XML_DECL, xml_decl2);
			Matcher enc_m2 = FMLtoSSML.ENCODING_IN_XML_DECL_RE.matcher(xml_decl2);
			assertTrue(enc_m2.find());
			String enc2 = enc_m2.group("encoding");
			assertEquals(FML2_CHARSET, Charset.forName(enc2));
		}
		{ // - fml3
			Matcher xml_m3 = FMLtoSSML.XML_DECL_RE.matcher(fml3_content);
			assertTrue(xml_m3.find());
			String xml_decl3 = xml_m3.group();
			assertEquals(FML3_XML_DECL, xml_decl3);
			Matcher enc_m3 = FMLtoSSML.ENCODING_IN_XML_DECL_RE.matcher(xml_decl3);
			assertTrue(enc_m3.find());
			String enc3 = enc_m3.group("encoding");
			assertEquals(FML3_CHARSET, Charset.forName(enc3));
		}

		// SPEECH
		{ // - fml2_calm => xml2ContentU8
			// - the <speech> element
			Matcher speech_m2 = FMLtoSSML.SPEECH_PART_RE.matcher(fml2_content);
			assertTrue(speech_m2.find());
			String speech_open2 = speech_m2.group("open");
			assertEquals(XML2_SPEECH_OPEN, speech_open2);
			String speech_inside2 = speech_m2.group("inside");
			//showDiff(speech_inside_m2, XML2_SPEECH_INSIDE, 10);
			assertEquals(XML2_SPEECH_INSIDE, speech_inside2);
			String speech_close2 = speech_m2.group("close");
			assertEquals(XML2_SPEECH_CLOSE, speech_close2);
			// - id and start attributes
			Matcher speech_id_m2 = FMLtoSSML.ID_ATTRIB_RE.matcher(speech_open2);
			assertTrue(speech_id_m2.find());
			String speech_id2 = speech_id_m2.group("value");
			assertEquals("s1", speech_id2);
			Matcher speech_start_m2 = FMLtoSSML.START_ATTRIB_RE.matcher(speech_open2);
			assertTrue(speech_start_m2.find());
			String speech_start2 = speech_start_m2.group("value");
			assertEquals("0.0", speech_start2);
			Matcher speech_end_m2 = FMLtoSSML.END_ATTRIB_RE.matcher(speech_open2);
			assertFalse(speech_end_m2.find());
			// cleanSpeechInside
			String clean_inside2 = fmlToSSML.cleanSpeechInside(speech_inside2);
			//showDiff(XML2_SPEECH_CLEANINSIDE, clean_inside2, 10);
			assertEquals(XML2_SPEECH_CLEANINSIDE, clean_inside2);
			// replace <tm> --> <mark>
			String tm2mark_inside2 = fmlToSSML.replaceTMtoMark(clean_inside2);
			//showDiff(XML2_SPEECH_TM2MARK, tm2mark_inside2, 10);
			assertEquals(XML2_SPEECH_TM2MARK, tm2mark_inside2);
			// replace <tm> --> <mark>
			String voxvoice_inside2 = fmlToSSML.replaceVoice(tm2mark_inside2);
			//showDiff(XML2_SPEECH_VOXVOICE, voxvoice_inside2, 10);
			assertEquals(XML2_SPEECH_VOXVOICE, voxvoice_inside2);
		}
	}
	
	@Test
	public void testFMLFilesContentToSSML() throws IOException {
		// [0] Get the content of files
		String fml1_content = fml1_nospeech.getContent();
		String fml2_content = fml2_calm.getContent();
		String fml3_content = fml3_normal.getContent();
		// [1] Get the SpeechSSML[]
		FMLtoSSML fmlToSSML = new FMLtoSSML();
		SpeechSSML[] fml1_ssmls = fmlToSSML.fmlToSSML(fml1_content);
		SpeechSSML[] fml2_ssmls = fmlToSSML.fmlToSSML(fml2_content);
		SpeechSSML[] fml3_ssmls = fmlToSSML.fmlToSSML(fml3_content);
		
		// Speech element: id, start
		// - fml1
		assertEquals(0, fml1_ssmls.length);
		// - fml2
		assertEquals(1, fml2_ssmls.length);
		assertEquals(FML2_CHARSET, fml2_ssmls[0].getEncoding());// FML encoding
		assertEquals("s1", fml2_ssmls[0].getSpeechId());
		assertEquals("0.0", fml2_ssmls[0].getSpeechStartAtt());
		assertEquals(0.0, fml2_ssmls[0].getSpeechStart(), 0.001);
		// - fml3
		assertEquals(1, fml3_ssmls.length);
		assertEquals(FML3_CHARSET, fml3_ssmls[0].getEncoding());// FML encoding
		assertEquals("s1", fml3_ssmls[0].getSpeechId());
		assertEquals("0.0", fml3_ssmls[0].getSpeechStartAtt());
		assertEquals(0.0, fml3_ssmls[0].getSpeechStart(), 0.001);

		// <speech> --> <speak>
		// - fml2
		assertEquals(XML2_SPEAK, fml2_ssmls[0].getSpeakElement());
		StringBuilder fml2_ssml = new StringBuilder();
		fml2_ssml.append(FMLtoSSML.xmlDeclaration(FML2_CHARSET));
		if (SpeechSSML.XML_DECL_EOL!=null) fml2_ssml.append(SpeechSSML.XML_DECL_EOL);
		fml2_ssml.append(fml2_ssmls[0].getSpeakElement());
		if (SpeechSSML.XML_END_OF_FILE!=null) fml2_ssml.append(SpeechSSML.XML_END_OF_FILE);
		assertArrayEquals(fml2_ssml.toString().getBytes(FML2_CHARSET), fml2_ssmls[0].getBytes());
		// compare with ssml2_calm
		//showDiff(ssml2_calm.getBytes(), fml2_ssmls[0].getBytes(),10);
		assertArrayEquals(ssml2_calm.getBytes(true), fml2_ssmls[0].getNullTermBytes());
		// - fml3
		assertEquals(XML3_SPEAK, fml3_ssmls[0].getSpeakElement());
		StringBuilder fml3_ssml = new StringBuilder();
		fml3_ssml.append(FMLtoSSML.xmlDeclaration(FML3_CHARSET));
		if (SpeechSSML.XML_DECL_EOL!=null) fml3_ssml.append(SpeechSSML.XML_DECL_EOL);
		fml3_ssml.append(fml3_ssmls[0].getSpeakElement());
		if (SpeechSSML.XML_END_OF_FILE!=null) fml3_ssml.append(SpeechSSML.XML_END_OF_FILE);
		assertArrayEquals(fml3_ssml.toString().getBytes(FML3_CHARSET), fml3_ssmls[0].getBytes());
		// compare with ssml3_normal
		//showDiff(ssml3_normal.getBytes(), fml3_ssmls[0].getBytes(),10);
		assertArrayEquals(ssml3_normal.getBytes(true), fml3_ssmls[0].getNullTermBytes());
		
		SpeechSSML[] fml4_ssmls = fmlToSSML.fmlToSSML(fml4_maintenant.getContent());
		assertEquals(1, fml4_ssmls.length);
		/*{//TMP: convert P02-03_Terrain-01-On_fait_quoi_maintenant.xml --> SSML
			String outputFile="P02-03_Terrain-01-On_fait_quoi_maintenant.ssml";
			OutputStream fout = new BufferedOutputStream(new FileOutputStream(new File(outputFile)));
			fml4_ssmls[0].writeTo(fout);
			fout.close();
			System.err.println("Write fml4 SSML into "+outputFile);
		}*/
		
		SpeechSSML[] fml5_ssmls = fmlToSSML.fmlToSSML(fml5_maintenant_multi.getContent());
		assertEquals(4, fml5_ssmls.length);
		/*{//TMP: convert P02-03_Terrain-01-On_fait_quoi_maintenant-multi.xml --> SSMLs
			String outputFileFormat="P02-03_Terrain-01-On_fait_quoi_maintenant-multi_%s.ssml";
			for (int i=0; i<fml5_ssmls.length; ++i) {
				String outputFile = String.format(outputFileFormat, fml5_ssmls[i].getSpeechId());
				OutputStream fout = new BufferedOutputStream(new FileOutputStream(new File(outputFile)));
				fml5_ssmls[i].writeTo(fout);
				fout.close();
				System.err.println("Write fml5 SSML["+i+"] into "+outputFile);
			}
		}*/
	}
	
	
	public static void showDiff(String s1, String s2, int ctxt) {
		int diffIdx = StringUtils.indexOfDifference(s1, s2);
		if (diffIdx<0) return;
		int before = diffIdx - ctxt; if (before<0) before = 0;
		int after = diffIdx + ctxt;
		System.err.println("Strings differs at index:"+diffIdx);
		for (int i=before; i<after; ++i) {
			String sc1, sc2;
			if (i<s1.length()) {
				char c1 = s1.charAt(i);
				sc1 = Character.isWhitespace(c1) ? StringEscapeUtils.escapeJava(Character.toString(c1)) : Character.toString(c1);
			} else sc1="\\0";
			if (i<s2.length()) {
				char c2 = s2.charAt(i);
				sc2 = Character.isWhitespace(c2) ? StringEscapeUtils.escapeJava(Character.toString(c2)) : Character.toString(c2);
			} else sc2="\\0";
			System.err.println("["+i+"] s1: "+sc1+" | s2: "+sc2);
		}
	}

	public static void showDiff(byte[] bytes1, byte[] bytes2, int ctxt) {
		int i = -1;
		int pos = -1;
		do {
			++i;
			if (i>=bytes1.length) {
				if (i>=bytes2.length) {
					//System.err.println("BOTH equals, length:"+i);
					return;//break; // both equals 
				} else {
					System.err.println("SHORTER bytes1 length:"+i+" than bytes2 length:"+bytes2.length);
					pos = i;
					break;
				}
			} else if (i>=bytes2.length) {
				System.err.println("BIGGER bytes1 length:"+bytes1.length+" than bytes2 length:"+i);
				pos = i;
				break;
			}
			if (bytes1[i] != bytes2[i]) {
				System.err.println("DIFFER at "+i);
				pos = i;
				break;
			}
		} while (true);
		
		if (pos>=0) {
			int before = pos - ctxt; if (before<0) before = 0;
			int after = pos + ctxt;
			for (i=before; i<after; ++i) {
				String sc1, sc2;
				if (i<bytes1.length) {
					byte b1 = bytes1[i];
					char c1 = (char) (b1 & 0xFF);
					sc1 = String.format("%02x (%s)"
							, b1 
							, Character.isWhitespace(c1) ? StringEscapeUtils.escapeJava(Character.toString(c1)) : Character.toString(c1)
							);
				} else sc1="\\0";
				if (i<bytes2.length) {
					byte b2 = bytes2[i];
					char c2 = (char) (b2 & 0xFF);
					sc2 = String.format("%02x (%s)"
							, b2 
							, Character.isWhitespace(c2) ? StringEscapeUtils.escapeJava(Character.toString(c2)) : Character.toString(c2)
							);
				} else sc2="\\0";
				System.err.println("["+i+"] bytes1: "+sc1+" | bytes1: "+sc2);
			}	
		}
	}
}
