package lpl.tools;

public class TimeStampCopy implements TimeStampIfce {
	protected final float millisecond;
	protected final long bytes, samples;
	
	public TimeStampCopy(TimeStampIfce timeStamp) {
		millisecond = timeStamp.getMillisecond();
		bytes = timeStamp.getBytes();
		samples = timeStamp.getSamples();
	}
	
	@Override
	public float getMillisecond() {
		return millisecond;
	}

	@Override
	public long getBytes() {
		return bytes;
	}

	@Override
	public long getSamples() {
		return samples;
	}

}
