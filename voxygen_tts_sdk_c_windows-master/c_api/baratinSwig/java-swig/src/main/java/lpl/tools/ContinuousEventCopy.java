package lpl.tools;

public class ContinuousEventCopy
	extends TimeStampEventCopy
	implements ContinuousEventIfce
{
	protected final float duration;
	protected final int nb_samples;

	public ContinuousEventCopy(ContinuousEventIfce continuousEvent) {
		super(continuousEvent);
		duration = continuousEvent.getDuration();
		nb_samples = continuousEvent.getNb_samples();
	}
	
	
	@Override
	public float getDuration() {
		return duration;
	}

	@Override
	public int getNb_samples() {
		return nb_samples;
	}

	public static TimeStampEventIfce copyTimeStampEvent(TimeStampEventIfce event) {
		if (event instanceof ContinuousEventIfce)
			return new ContinuousEventCopy((ContinuousEventIfce) event);
		return new TimeStampEventCopy(event);
	}
	
}
