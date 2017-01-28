/* merged different examples for Teensy by Paul Stoffregen
   to test MIDI IN & OUT
   
   This example code is in the public domain.
*/
#include <Bounce.h>

const int channel = 1;
Bounce button5 = Bounce(5, 5);  // if a button is too "sensitive"

// the MIDI continuous controller for each analog input
const int controllerA0 = 10; // 10 = pan position

int previousA0 = -1;

elapsedMillis msec = 0;

void setup() {
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleVelocityChange(OnVelocityChange);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandleProgramChange(OnProgramChange);
  usbMIDI.setHandleAfterTouch(OnAfterTouch);
  usbMIDI.setHandlePitchChange(OnPitchChange);
  
   pinMode(5, INPUT_PULLUP);
 
}

void loop() {
  usbMIDI.read(); // USB MIDI receive
   // only check the analog inputs 50 times per second,
  // to prevent a flood of MIDI messages
  
   button5.update();
  if (button5.fallingEdge()) {
    usbMIDI.sendNoteOn(65, 99, channel);  // 65 = F4
  }
  if (button5.risingEdge()) {
    usbMIDI.sendNoteOff(65, 0, channel);  // 65 = F4
  }
  
  if (msec >= 20) {
    msec = 0;
    int n0 = analogRead(0) / 8;
    // only transmit MIDI messages if analog input changed
    if (n0 != previousA0) {
      usbMIDI.sendControlChange(controllerA0, n0, channel);
      previousA0 = n0;
    }
   
  }
  
}


void OnNoteOn(byte channel, byte note, byte velocity) {
  digitalWrite(7,1);
  digitalWrite(13,1);
}

void OnNoteOff(byte channel, byte note, byte velocity) {
  digitalWrite(7,0);
  digitalWrite(13,0);
}

void OnVelocityChange(byte channel, byte note, byte velocity) {
  }

void OnControlChange(byte channel, byte control, byte value) {
}

void OnProgramChange(byte channel, byte program) {
}

void OnAfterTouch(byte channel, byte pressure) {
}

void OnPitchChange(byte channel, int pitch) {
}


