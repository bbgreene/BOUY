# MyTremolo

An audio tremolo vst plugin made with JUCE.

![MyTremolo](https://github.com/bbgreene/MyTremolo/blob/master/Screenshots/MyTremolo%20-%20pre%20gui.png?raw=true
 "MyTremolo")
 
 
 
** Parameters**
 
 LFO one (amplitude modulation/tremolo):
 
 Depth One: the amount of amplitude modulation applied to an incoming signal by LFO one\
 Freq One: the rate of amplitude modulation applied to an incoming signal by LFO one (0-500 Hz)\
 Waveform: Choice of waveforms (sine, triangle, sloped square, ring mod) for LFO one\
 Multiplier: Multiplies LFO one's frequency by whole numbers\
 
 LFO two (frequency modulation of LFO one):
 
 Depth Two: Defines the frequency range that LFO one will deviate from its set frequency\
 Freq Two: LFO two frequency controlling the rate of frequency modulation (0-10 Hz)
 
Next steps: BPM sync for LFO one frequency, adjust multiplier values and create GUI
