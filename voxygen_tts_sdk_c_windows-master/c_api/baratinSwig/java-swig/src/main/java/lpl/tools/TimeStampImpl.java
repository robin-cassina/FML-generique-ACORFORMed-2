package lpl.tools;

public class TimeStampImpl implements TimeStampIfce {
	protected float millisecond;
	protected long bytes, samples;
	
	public TimeStampImpl(TimeStampIfce timeStamp) {
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

	public void setMillisecond(float millisecond) {
		this.millisecond = millisecond;
		//TODO? adjust sample/bytes
	}

	public void setBytes(long bytes) {
		this.bytes = bytes;
	}

	public void setSamples(long samples) {
		this.samples = samples;
	}

	
	
}
