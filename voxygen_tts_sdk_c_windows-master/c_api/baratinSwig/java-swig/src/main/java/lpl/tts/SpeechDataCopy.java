package lpl.tts;

import java.util.ArrayList;

import lpl.tools.ContinuousEventCopy;
import lpl.tools.ContinuousEventIfce;
import lpl.tools.TimeStampEventIfce;
import lpl.tts.voxygen.EVENT_TYPE;
import lpl.tts.voxygen.SIGNAL_CODING;

public class SpeechDataCopy
	implements SpeechData
{
	// sound encoding
	protected SIGNAL_CODING coding;
	protected int frequency;
	// state
	protected int error;
	// sound data
	protected byte[] rawData, header;
	private byte[] _sound;
	protected int nbSamples;
	protected float duration;
	// events
	protected TimeStampEventIfce[] allEvents;
	private ContinuousEventIfce[] _visemesEvents;
	private TimeStampEventIfce[] _markerEvents;

	private static final ContinuousEventIfce[] EMPTY_VISEMES = new ContinuousEventIfce[0];
	private static final TimeStampEventIfce[] EMPTY_EVENTS = new TimeStampEventIfce[0];
	
	public SpeechDataCopy(SpeechData speechData) {
		// copy all elements
		// - sound encoding
		this.coding = speechData.getCoding();
		this.frequency = speechData.getFrequency();
		// - state
		this.error = speechData.isError();
		// - sound data
		this.rawData = speechData.getRawData();
		this.header = speechData.getHeader();
		this.nbSamples = speechData.getNbSamples();
		this.duration = speechData.getDuration();
		// - events
		//this.allEvents = speechData.getAllEvents();
		TimeStampEventIfce[]  _allEvents = speechData.getAllEvents();
		this.allEvents = new TimeStampEventIfce[_allEvents.length];
		for (int i=0; i<_allEvents.length; ++i) {
			this.allEvents[i] = ContinuousEventCopy.copyTimeStampEvent(_allEvents[i]);
		}
	}
	
	@Override
	public SIGNAL_CODING getCoding() {
		return coding;
	}

	@Override
	public int getFrequency() {
		return frequency;
	}

	@Override
	public int isError() {
		return error;
	}

	@Override
	public byte[] getRawData() {
		return rawData;
	}

	@Override
	public long getRawDataLength() {
		return rawData.length;
	}

	@Override
	public byte[] getHeader() {
		return header;
	}

	@Override
	public long getHeaderLength() {
		return header.length;
	}

	@Override
	public byte[] getSound() {
		if (_sound==null) {
			byte _sound[] = new byte[rawData.length + header.length];
			System.arraycopy(header, 0, _sound, 0, header.length);
			System.arraycopy(rawData, 0, _sound, header.length, rawData.length);
		}
		return _sound;
	}

	@Override
	public long getSoundLength() {
		return header.length + rawData.length;
	}

	@Override
	public int getNbSamples() {
		return nbSamples;
	}

	@Override
	public float getDuration() {
		return duration;
	}

	@Override
	public TimeStampEventIfce[] getAllEvents() {
		return allEvents;
	}

	@Override
	public ContinuousEventIfce[] getVisemeEvents() {
		if (_visemesEvents==null) {
			ArrayList<ContinuousEventIfce> visemesEventsList = new ArrayList<ContinuousEventIfce>();
			for (TimeStampEventIfce e : allEvents) {
				if (e.getType() == EVENT_TYPE.VISEME_EVENT)
					visemesEventsList.add((ContinuousEventIfce) e);
			}
			_visemesEvents = visemesEventsList.toArray(EMPTY_VISEMES);
		}
		return _visemesEvents;
	}

	@Override
	public TimeStampEventIfce[] getTypedEvents(EVENT_TYPE type) {
		ArrayList<TimeStampEventIfce> events = new ArrayList<TimeStampEventIfce>();
		for (TimeStampEventIfce e : allEvents) {
			if (e.getType() == type)
				events.add(e);
		}
		return events.toArray(EMPTY_EVENTS);
	}

	@Override
	public TimeStampEventIfce[] getMarkerEvents() {
		if (_markerEvents==null) {
			_markerEvents = this.getTypedEvents(EVENT_TYPE.MARKER_EVENT);
		}
		return _markerEvents;
	}

}
