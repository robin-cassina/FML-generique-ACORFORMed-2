package lpl.tts;

import lpl.tools.ContinuousEventIfce;
import lpl.tools.TimeStampEventIfce;
import lpl.tts.voxygen.EVENT_TYPE;
import lpl.tts.voxygen.SIGNAL_CODING;

public interface SpeechData {

	SIGNAL_CODING getCoding();

	int getFrequency();

	int isError();

	byte[] getRawData();

	long getRawDataLength();

	byte[] getHeader();

	long getHeaderLength();

	byte[] getSound();

	long getSoundLength();
	  
	int getNbSamples();

	float getDuration();

	TimeStampEventIfce[] getAllEvents();

	ContinuousEventIfce[] getVisemeEvents();

	TimeStampEventIfce[] getTypedEvents(EVENT_TYPE type);

	TimeStampEventIfce[]  getMarkerEvents();

}