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
int beat = 0;

void loop() {
  for (int i = 0; i < 4; i++) { 
    glow[i] = false; 
  }
  beat = (beat + 1) % 8;

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
  
  switchOutputs(glow);

  delay(500);
