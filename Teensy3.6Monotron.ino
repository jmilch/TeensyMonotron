float kammertonA = 440.00;
float schritt = 1.059463094359;
float frequenzen[128];

const int subOscPin = 3;
const int ledPin = 13;
const int pitchCVPin = A12; //DAC1 A21 on Teensy 3.6 and A12 on Teensy LC
//Filter Pin on second DAC uncomment for Teensy 3.6
//const int filterCutoffCVPin = A22;
const int gatePin = 17;
byte subOscTranspose = 0;
float bend;
float pitch_value;
float divider = 38; 


void setup() {
  kammertonA = kammertonA*pow(2,5);
  kammertonA = kammertonA/pow(schritt,2);
  for (int i = 127; i > -1; i--) {
    frequenzen[i] = kammertonA;
    kammertonA = kammertonA/schritt;
  }
  analogWriteResolution(12);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandlePitchChange(OnPitchChange);
  pinMode(subOscPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(gatePin, OUTPUT);
}

void loop() {
  usbMIDI.read();
}

void OnNoteOn(byte kanal, byte note, byte velocity) {
  if(velocity==0) {
    digitalWrite(ledPin, LOW);
    digitalWriteFast(gatePin, LOW);
    noTone(subOscPin);
  }
  else {
    digitalWrite(ledPin, HIGH);
    pitch_value = constrain(note, 60, 99);
    analogWrite(pitchCVPin, ((pitch_value + bend - 60.0) / divider) * 4095.0);
    //Filter Pin on second DAC uncomment for Teensy 3.6
    //analogWrite(filterCutoffCVPin, map(velocity, 0, 127, 0, 4095));
    digitalWriteFast(gatePin, HIGH);
    tone(subOscPin, frequenzen[note+subOscTranspose]);
  }
}

void OnNoteOff(byte kanal, byte note, byte velocity) {
  digitalWrite(ledPin, LOW);
  digitalWriteFast(gatePin, LOW);
  noTone(subOscPin);
}

void OnPitchChange (byte channel, int pitch_change) {
     bend = map(pitch_change, 0, 16383, -12.0, 12.0);
    analogWrite(pitchCVPin, ((pitch_value + bend - 60.0) / divider) * 4095.0);
  }
