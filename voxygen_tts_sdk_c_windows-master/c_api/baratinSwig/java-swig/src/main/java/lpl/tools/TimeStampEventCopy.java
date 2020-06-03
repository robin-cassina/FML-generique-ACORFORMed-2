package lpl.tools;

import lpl.tts.voxygen.EVENT_TYPE;

public class TimeStampEventCopy 
	extends TimeStampCopy
	implements TimeStampEventIfce 
{
	protected final EVENT_TYPE type;
	protected final String name;
	
	public TimeStampEventCopy(TimeStampEventIfce timeStampEvent) {
		super(timeStampEvent);
		type = timeStampEvent.getType();
		name = timeStampEvent.getName();
	}

	@Override
	public EVENT_TYPE getType() {
		return type;
	}

	@Override
	public String getName() {
		return name;
	}

}
