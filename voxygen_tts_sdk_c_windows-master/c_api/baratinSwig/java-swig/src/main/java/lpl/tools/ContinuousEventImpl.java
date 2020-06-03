package lpl.tools;

public class ContinuousEventImpl
	extends TimeStampEventImpl
	implements ContinuousEventIfce
{
	protected float duration;
	protected int nb_samples;

	public ContinuousEventImpl(ContinuousEventIfce continuousEvent) {
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


	public void setDuration(float duration) {
		this.duration = duration;
	}


	public void setNb_samples(int nb_samples) {
		this.nb_samples = nb_samples;
	}

	
}
