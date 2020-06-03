package lpl.tools;

import lpl.tts.voxygen.EVENT_TYPE;

public interface TimeStampEventIfce
	extends TimeStampIfce
{

	EVENT_TYPE getType();

	String getName();

}