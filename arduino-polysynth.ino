#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// this is the current time, that gets updated when you call oscClock()
unsigned long us;

void oscClock() {
    us = micros();
}


// this is the Oscillator - it's a square wave with PWM 
class Osc {
public:

  unsigned int uPeriod = 0;
  unsigned int halfPeriod = 0;
  int out = 0;
  int pin;
  int note = 0;
  float modulationValue = 0.5f;


  float bendValue = 1.f;
  
  void setPin(int pin) {
    this->pin = pin;
    pinMode(pin, OUTPUT);
  }

  void bend(float val) {
    bendValue = val;
    calculatePitch();
  }
  
  void mod(float m) {
    modulationValue = m*0.5 + 0.4;
    halfPeriod = uPeriod * modulationValue;
  }
  
  void play(int note) {
    this->note = note;
    calculatePitch();
    released = false;
  }

  void calculatePitch() {
    float f = mtof(note) * bendValue;
    float u = 1000000.f/f;
    uPeriod = u;
    halfPeriod = uPeriod*modulationValue;
  }

  
  void stop() {
    uPeriod = 0;
    halfPeriod = 0;
    digitalWrite(pin, 0);
    out = 0;
    note = 0;
  }
  
  bool playing() {
    return note!=0;
  }

  void tick() {
    if(note!=0) {
      unsigned long pos = us % uPeriod;
      if(pos>halfPeriod != !out) {
        out ^= 1;
        digitalWrite(pin, out);
      }
    }
  }

  bool released = false;  
  int mtof(int midiNote) {
    float freq = 440.0 * exp( 0.057762265 * ( midiNote-69.0 ) );
    return freq;
  }
};




bool sustain = false;

const int NUM_VOICES = 10;

Osc voices[NUM_VOICES];


void setup() {

  // setting all voices to output on pin 2 for now.
  for(int i = 0; i < NUM_VOICES; i++) {
    voices[i].setPin(2);
  }

  MIDI.setHandleNoteOn(handleNoteOn); 
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleControlChange);
  MIDI.setHandlePitchBend(handlePitchBend);

  MIDI.begin(MIDI_CHANNEL_OMNI);
}


void handleNoteOn(byte channel, byte note, byte velocity) {

  if(sustain) {
    // ignore notes that are the same pitch as other sustained notes
    for(int i = 0; i < NUM_VOICES; i++) {
      if(voices[i].playing() && note==voices[i].note) {
        return;
      }
    }
  }
  for(int i = 0; i < NUM_VOICES; i++) {
    if(!voices[i].playing()) {
      voices[i].play(note);
      return;
    }
  }
  // no free voice, just pick a rando to steal
  voices[rand()%NUM_VOICES].play(note);
}


void handleNoteOff(byte channel, byte note, byte velocity) {
  for(int i = 0; i < NUM_VOICES; i++) {
    if(voices[i].playing() && voices[i].note==note) {
      if(sustain) {
        voices[i].released = true;
      } else {
        voices[i].stop();
      }
      return;
    }
  }
}


void handleControlChange(byte channel, byte number, byte value) {
  
  if(number==1) {
    float modVal = value / 127.f;
    for(int i = 0; i < NUM_VOICES; i++) {
      voices[i].mod(modVal);
    }
  } else if(number==64) {
    sustain = value>0;
    // if sustain pedal gets released, let go 
    // of any notes that aren't currently pressed down.
    if(!sustain) {
      for(int i = 0; i < NUM_VOICES; i++) {
        if(voices[i].released) {
          voices[i].stop();
        }
      }
    }
  }
}

void handlePitchBend(byte channel, int bend) {
  // convert +/- 8192 to speed (e.g. 0.5 to 2.0)
  float bendF = bend / 8192.f;
  bendF = pow(2.f, bendF);
  for(int i = 0; i < NUM_VOICES; i++) {
    voices[i].bend(bendF);
  }
}



void loop() {
  oscClock();
  MIDI.read();
  for(int i = 0; i < NUM_VOICES; i++) {
    voices[i].tick();
  }
}
