// Teensy-Synth 12 Bit DAC
// Version 1.0
// By Notes and Volts
// www.notesandvolts.com

// This program uses the built-in 12 Bit DAC pin
// on the Teensy 3.2 board

// Requires 'MIDI Library by Forty Seven Effects' 
// Install in Arduino IDE using 'Sketch/Include Library/Manage Libraries'

// Tools menu Settings:
// Board: "Teensy3.2/3.1"
// USB Type: "MIDI"

#include <Audio.h>
#include <MIDI.h>

//Mux Pins
#define MS0 A3
#define MS1 A6
#define MS2 8
#define MS3 A7
#define MZ A2
#define MnumControls 15

#define MUXosc1 5
#define MUXosc2 6
#define MUXdetune 7
#define MUXmix1 9
#define MUXmix2 10
#define MUXmix3 11
#define MUXattack 3
#define MUXdecay 4
#define MUXsustain 2
#define MUXrelease 1
#define MUXlfospeed 0
#define MUXlfodepth 14
#define MUXlfomode 8
#define MUXfilterres 12
#define MUXfilterfreq 13

//Switch pin numbers
#define numSwitch 3
#define SWosc1 2
#define SWosc2 3
#define SWlfo 4

//MIDI CC control numbers
#define CCmixer1 100
#define CCmixer2 101
#define CCmixer3 102
#define CCoctave 103
#define CCattack 104
#define CCdecay 105
#define CCsustain 106
#define CCrelease 107
#define CCosc1 108
#define CCosc2 109
#define CCdetune 110
#define CCfilterfreq 111
#define CCfilterres 112
#define CCbendrange 113
#define CClfospeed 114
#define CClfodepth 115
#define CClfomode 116

// GUItool: begin automatically generated code NEW
AudioSynthWaveform       waveform2;      //xy=414,383
AudioSynthWaveform       waveform1;      //xy=418,324
AudioSynthNoisePink      pink1;          //xy=426,434
AudioMixer4              mixer1;         //xy=609,384
AudioFilterStateVariable filter1;        //xy=757,387
AudioEffectEnvelope      envelope1;      //xy=920,388
AudioOutputAnalog        dac1;           //xy=1059,388
AudioConnection          patchCord1(waveform2, 0, mixer1, 1);
AudioConnection          patchCord2(waveform1, 0, mixer1, 0);
AudioConnection          patchCord3(pink1, 0, mixer1, 2);
AudioConnection          patchCord4(mixer1, 0, filter1, 0);
AudioConnection          patchCord5(filter1, 0, envelope1, 0);
AudioConnection          patchCord6(envelope1, dac1);
// GUItool: end automatically generated code


// GLOBAL VARIABLES
const byte BUFFER = 8; //Size of keyboard buffer
const float noteFreqs[128] = {8.176, 8.662, 9.177, 9.723, 10.301, 10.913, 11.562, 12.25, 12.978, 13.75, 14.568, 15.434, 16.352, 17.324, 18.354, 19.445, 20.602, 21.827, 23.125, 24.5, 25.957, 27.5, 29.135, 30.868, 32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55, 58.27, 61.735, 65.406, 69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.826, 110, 116.541, 123.471, 130.813, 138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220, 233.082, 246.942, 261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440, 466.164, 493.883, 523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609, 880, 932.328, 987.767, 1046.502, 1108.731, 1174.659, 1244.508, 1318.51, 1396.913, 1479.978, 1567.982, 1661.219, 1760, 1864.655, 1975.533, 2093.005, 2217.461, 2349.318, 2489.016, 2637.02, 2793.826, 2959.955, 3135.963, 3322.438, 3520, 3729.31, 3951.066, 4186.009, 4434.922, 4698.636, 4978.032, 5274.041, 5587.652, 5919.911, 6271.927, 6644.875, 7040, 7458.62, 7902.133, 8372.018, 8869.844, 9397.273, 9956.063, 10548.08, 11175.3, 11839.82, 12543.85};
byte globalNote = 0;
byte globalVelocity = 0;
int octave1 = 0;
int octave2 = 0;
const float DIV127 = (1.0 / 127.0);
float detuneFactor = 1;
float bendFactor = 1;
int bendRange = 12;

unsigned int LFOspeed = 2000;
float LFOpitch = 1;
float LFOdepth = 0;
byte LFOmodeSelect = 0;

int FILfreq =  10000;
float FILfactor = 1;

byte osc1Mode = 255; // 255 = Nonsense value to force startup read
byte osc2Mode = 255;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void setup() {
  AudioMemory(20);

  pinMode(SWosc1, INPUT_PULLUP);
  pinMode(SWosc2, INPUT_PULLUP);
  pinMode(SWlfo, INPUT_PULLUP);

  pinMode(MS0, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);

  MIDI.begin();

  MIDI.setHandleNoteOn(myNoteOn);
  MIDI.setHandleNoteOff(myNoteOff);
  MIDI.setHandlePitchBend(myPitchBend);
  MIDI.setHandleControlChange(myControlChange);

  usbMIDI.setHandleControlChange(myControlChange);
  usbMIDI.setHandleNoteOff(myNoteOff);
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandlePitchChange(myPitchBend);
  waveform1.begin(WAVEFORM_SAWTOOTH);
  waveform1.amplitude(0.75);
  waveform1.frequency(82.41);
  waveform1.pulseWidth(0.15);

  waveform2.begin(WAVEFORM_SAWTOOTH);
  waveform2.amplitude(0.75);
  waveform2.frequency(123);
  waveform2.pulseWidth(0.15);

  pink1.amplitude(1.0);

  mixer1.gain(0, 1.0);
  mixer1.gain(1, 1.0);
  mixer1.gain(2, 0.0);

  envelope1.attack(0);
  envelope1.decay(0);
  envelope1.sustain(1);
  envelope1.release(500);
}


void loop() {
  usbMIDI.read();
  MIDI.read();
  LFOupdate(false, LFOmodeSelect, FILfactor, LFOdepth);
  checkMux();
}

void myNoteOn(byte channel, byte note, byte velocity) {
  if ( note > 23 && note < 108 ) {
    globalNote = note;
    globalVelocity = velocity;
    keyBuff(note, true);
    LFOupdate(true, LFOmodeSelect, FILfactor, LFOdepth);
  }
}

void myNoteOff(byte channel, byte note, byte velocity) {
  if ( note > 23 && note < 108 ) {
    keyBuff(note, false);
  }
}

void myPitchBend(byte channel, int bend) {
  float bendF = bend;
  bendF = bendF / 8192;
  bendF = bendF * bendRange;
  bendF = bendF / 12;
  bendFactor = pow(2, bendF);
  oscSet();
}

void keyBuff(byte note, bool playNote) {
  static byte buff[BUFFER];
  static byte buffSize = 0;

  // Add Note
  if (playNote == true && (buffSize < BUFFER) ) {
    oscPlay(note);
    buff[buffSize] = note;
    buffSize++;
    return;
  }

  // Remove Note
  else if (playNote == false && buffSize != 0) {
    for (byte found = 0; found < buffSize; found++) {
      if (buff[found] == note) {
        for (byte gap = found; gap < (buffSize - 1); gap++) {
          buff[gap] = buff[gap + 1];
        }
        buffSize--;
        buff[buffSize] = 255;
        if (buffSize != 0) {
          oscPlay(buff[buffSize - 1]);
          return;
        }
        else {
          oscStop();
          return;
        }
      }
    }
  }
}

void oscPlay(byte note) {
  waveform1.frequency(noteFreqs[note + octave1] * bendFactor * LFOpitch);
  waveform2.frequency(noteFreqs[note + octave2] * detuneFactor * bendFactor * LFOpitch);
  float velo = 0.75 * (globalVelocity * DIV127);//TEST velocity limit to 0.75
  waveform1.amplitude(velo);
  waveform2.amplitude(velo);
  pink1.amplitude(velo);
  //envelope1.releaseNoteOn(5);//TEST
  envelope1.noteOn();
}

void oscStop() {
  envelope1.noteOff();
}

void oscSet() {
  waveform1.frequency(noteFreqs[globalNote + octave1] * bendFactor * LFOpitch);
  waveform2.frequency(noteFreqs[globalNote + octave2] * detuneFactor * bendFactor * LFOpitch);
}

void myControlChange(byte channel, byte control, byte value) {
  switch (control) {
    case CCmixer1:
      mixer1.gain(0, 0.3 * (value * DIV127)); //TEST gain limit to 0.3
      break;

    case CCmixer2:
      mixer1.gain(1, 0.3 * (value * DIV127));
      break;

    case CCmixer3:
      mixer1.gain(2, 0.3 * (value * DIV127));
      break;

    case CCoctave:
      switch (value) {
        case 0:
          octave2 = 24;
          break;
        case 1:
          octave2 = 12;
          break;
        case 2:
          octave2 = 0;
          break;
        case 3:
          octave2 = -12;
          break;
        case 4:
          octave2 = -24;
          break;
      }
      oscSet();
      break;

    case CCattack:
      envelope1.attack((3000 * (value * DIV127)) + 10.5);//TEST Attack min limit to 10.5ms
      break;

    case CCdecay:
      envelope1.decay(3000 * (value * DIV127));
      break;

    case CCsustain:
      envelope1.sustain(value * DIV127);
      break;

    case CCrelease:
      envelope1.release(3000 * (value * DIV127));
      break;

    case CCosc1:
      switch (value) {
        case 0:
          waveform1.begin(WAVEFORM_SINE);
          osc1Mode = 0;
          break;
        case 1:
          waveform1.begin(WAVEFORM_TRIANGLE);
          osc1Mode = 1;
          break;
        case 2:
          waveform1.begin(WAVEFORM_SAWTOOTH);
          osc1Mode = 2;
          break;
        case 3:
          waveform1.begin(WAVEFORM_PULSE);
          osc1Mode = 3;
          break;
      }
      break;

    case CCosc2:
      switch (value) {
        case 0:
          waveform2.begin(WAVEFORM_SINE);
          osc2Mode = 0;
          break;
        case 1:
          waveform2.begin(WAVEFORM_TRIANGLE);
          osc2Mode = 1;
          break;
        case 2:
          waveform2.begin(WAVEFORM_SAWTOOTH);
          osc2Mode = 2;
          break;
        case 3:
          waveform2.begin(WAVEFORM_PULSE);
          osc2Mode = 3;
          break;
      }
      break;

    case CCdetune:
      detuneFactor = 1 - (0.05 * (value * DIV127));
      oscSet();
      break;

    case CCfilterfreq:
      FILfactor = value * DIV127;
      FILfreq = 10000 * (value * DIV127);
      if (LFOmodeSelect < 1 || LFOmodeSelect > 5)filter1.frequency(FILfreq);
      break;

    case CCfilterres:
      filter1.resonance((4.3 * (value * DIV127)) + 0.7);
      break;

    case CCbendrange:
      if (value <= 12 && value > 0) {
        bendRange = value;
      }
      break;

    case CClfospeed:
      {
        float xSpeed = value * DIV127;
        xSpeed = pow(100, (xSpeed - 1));
        LFOspeed = (70000 * xSpeed);
        break;
      }

    case CClfodepth:
      LFOdepth = value * DIV127;
      break;

    case CClfomode:
      LFOmodeSelect = value;
      break;
  }
}

void LFOupdate(bool retrig, byte mode, float FILtop, float FILbottom) {
  static float LFO = 0;
  static unsigned long LFOtime = 0;
  static bool LFOdirection = false;
  unsigned long currentMicros = micros();
  static bool LFOstop = false;
  static float LFOrange = 0;
  static byte oldMode = 0;
  static bool retriggered = false;

  if (retrig == true) retriggered = true;


  if (currentMicros - LFOtime >= LFOspeed) {
    LFOtime = currentMicros;

    if (mode != oldMode) {
      if (mode == 0 || mode == 8) {
        LFOpitch = 1;
        oscSet();
        filter1.frequency(FILfreq);
      }
      else if (mode >= 1 || mode <= 7) {
        LFOpitch = 1;
        oscSet();
      }
      else if (mode >= 9 || mode <= 13) {
        filter1.frequency(FILfreq);
      }
      oldMode = mode;
    }

    LFOrange = FILtop - FILbottom;
    if (LFOrange < 0) LFOrange = 0;

    // LFO Modes
    switch (mode) {

      case 0: //Filter OFF
        return;
        break;
      case 1: //Filter FREE
        filter1.frequency(10000 * ((LFOrange * LFO) + LFOdepth));
        break;
      case 2: //Filter DOWN
        if (retriggered == true) {
          LFOdirection = true;
          LFO = 1.0;
        }
        filter1.frequency(10000 * ((LFOrange * LFO) + LFOdepth));
        break;
      case 3: //Filter UP
        if (retriggered == true) {
          LFOdirection = false;
          LFO = 0;
        }
        filter1.frequency(10000 * ((LFOrange * LFO) + LFOdepth));
        break;
      case 4: //Filter 1-DN
        if (retriggered == true) {
          LFOstop = false;
          LFOdirection = true;
          LFO = 1.0;
        }
        if (LFOstop == false) filter1.frequency(10000 * ((LFOrange * LFO) + LFOdepth));
        break;
      case 5: //Filter 1-UP
        if (retriggered == true) {
          LFOstop = false;
          LFOdirection = false;
          LFO = 0;
        }
        if (LFOstop == false) filter1.frequency(10000 * ((LFOrange * LFO) + LFOdepth));
        break;
      case 8: //Pitch OFF
        return;
        break;
      case 9: //Pitch FREE
        LFOpitch = (LFO * LFOdepth) + 1;
        oscSet();
        break;
      case 10: //Pitch DOWN
        if (retriggered == true) {
          LFOdirection = true;
          LFO = 1.0;
        }
        LFOpitch = (LFO * LFOdepth) + 1;
        oscSet();
        break;
      case 11: //Pitch UP
        if (retriggered == true) {
          LFOdirection = false;
          LFO = 0;
        }
        LFOpitch = (LFO * LFOdepth) + 1;
        oscSet();
        break;
      case 12: //Pitch 1-DN
        if (retriggered == true) {
          LFOstop = false;
          LFOdirection = true;
          LFO = 1.0;
        }
        if (LFOstop == false) {
          LFOpitch = (LFO * LFOdepth) + 1;
          oscSet();
        }
        break;
      case 13: //Pitch 1-UP
        if (retriggered == true) {
          LFOstop = false;
          LFOdirection = false;
          LFO = 0;
        }
        if (LFOstop == false) {
          LFOpitch = (LFO * LFOdepth) + 1;
          oscSet();
        }
        break;
    }

    retriggered = false;

    // Update LFO
    if (LFOdirection == false) { //UP
      LFO = (LFO + 0.01);
      if (LFO >= 1) {
        LFOdirection = true;
        LFOstop = true;
      }
    }

    if (LFOdirection == true) { //Down
      LFO = (LFO - 0.01);
      if (LFO <= 0) {
        LFOdirection = false;
        LFOstop = true;
      }
    }
  }
}

void checkMux() {
  static byte muxInput = 0;
  static int muxValues[MnumControls] = {};
  unsigned long currentMicros = micros();
  static unsigned long LFOtime = 0;

  if (currentMicros - LFOtime >= 1000) {
    LFOtime = currentMicros;

    int muxRead = analogRead(MZ);

    if (muxRead > (muxValues[muxInput] + 7) || muxRead < (muxValues[muxInput] - 7)) {
      muxValues[muxInput] = muxRead;
      muxRead = (muxRead >> 3); //Change range to 0-127
      switch (muxInput) {
        case MUXosc1:
          muxRead = muxRead / 32;
          if (muxRead != osc1Mode) myControlChange(0, CCosc1, muxRead);
          break;
        case MUXosc2:
          muxRead = muxRead / 32;
          if (muxRead != osc2Mode) myControlChange(0, CCosc2, muxRead);
          break;
        case MUXdetune:
          myControlChange(0, CCdetune, muxRead);
          break;
        case MUXmix1:
          myControlChange(0, CCmixer1, muxRead);
          break;
        case MUXmix2:
          myControlChange(0, CCmixer2, muxRead);
          break;
        case MUXmix3:
          myControlChange(0, CCmixer3, muxRead);
          break;
        case MUXattack:
          myControlChange(0, CCattack, muxRead);
          break;
        case MUXdecay:
          myControlChange(0, CCdecay, muxRead);
          break;
        case MUXsustain:
          myControlChange(0, CCsustain, muxRead);
          break;
        case MUXrelease:
          myControlChange(0, CCrelease, muxRead);
          break;
        case MUXlfospeed:
          myControlChange(0, CClfospeed, muxRead);
          break;
        case MUXlfodepth:
          myControlChange(0, CClfodepth, muxRead);
          break;
        case MUXlfomode:
          muxRead = muxRead / 24;
          myControlChange(0, CClfomode, muxRead);
          break;
        case MUXfilterres:
          myControlChange(0, CCfilterres, muxRead);
          break;
        case MUXfilterfreq:
          myControlChange(0, CCfilterfreq, muxRead);
          break;
      }
    }

    muxInput++;
    if (muxInput >= MnumControls) muxInput = 0;
    digitalWrite(MS0, muxInput & B0001);
    digitalWrite(MS1, muxInput & B0010);
    digitalWrite(MS2, muxInput & B0100);
    digitalWrite(MS3, muxInput & B1000);

    checkSwitch();
  }
}

void checkSwitch() {

  if (digitalRead(SWosc1)) {
    octave1 = 12;
  } else {
    octave1 = 0;
  }

  if (digitalRead(SWosc2)) {
    octave2 = -12;
  } else {
    octave2 = 0;
  }

  if (digitalRead(SWlfo)) {
    if (LFOmodeSelect < 8) LFOmodeSelect = LFOmodeSelect + 8;
  } else {
    if (LFOmodeSelect >= 8) LFOmodeSelect = LFOmodeSelect - 8;
  }
}

