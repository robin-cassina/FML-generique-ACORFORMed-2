package lpl.tools;

import lpl.tts.voxygen.EVENT_TYPE;

public class TimeStampEventImpl 
	extends TimeStampImpl
	implements TimeStampEventIfce 
{
	protected EVENT_TYPE type;
	protected String name;
	
	public TimeStampEventImpl(TimeStampEventIfce timeStampEvent) {
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

	public void setType(EVENT_TYPE type) {
		this.type = type;
	}

	public void setName(String name) {
		this.name = name;
	}

	
}
