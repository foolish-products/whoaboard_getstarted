/* This example shows how to use the WhoaSense library to 
 *  sequence the different channels to a beat.  
 *  
 *  Nice for making things sync to electronic music and the like.
 *  
 *  Copyright (C) 2017 Josh Vekhter
 *  This example code released under the Apache License
 */

#include <whoasense.h>

void setup() {
  Serial.begin(9600); 
  initWhoaBoard();
}

int glowIndex = 0;
// This keeps track of the beat we are on.
int beat = 0;
// This defines the number of beats in a "measure"
int beatCount = 8;
// This defines the length of a "beat" (in milliseconds)
int beatDuration = 500;

void loop() {
  for (int i = 0; i < 4; i++) { 
    glow[i] = false; 
  }
  beat = (beat + 1) % beatCount;

// The glow array defines the desired state of a particular 
// EL channel.
// NOTE! Computer arrays begin numbering at 0.  As such: 
// glow[0] controls the state of the EL channel labeled "1" 
// glow[3] controls the state of the EL channel labeled "4"
  if (beat % 2 == 0) {
    glow[0] = true;
  }
  if (beat % 2 == 1) {
    glow[1] = true;
  }
  if (beat == 3 || beat == 4) {
    glow[2] = true;
  }

  if (beat > 3) {
    glow[3] = true;
  }

  // This function "writes" the glow array, switching the 
  // EL outputs to reflect its state.
  switchOutputs(glow);

  delay(beatDuration);

}
