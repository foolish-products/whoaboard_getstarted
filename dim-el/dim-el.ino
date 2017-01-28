/* This example shows how to use the WhoaSense library to 
 *  dim EL elements using Pulse Width Modulation (PWM).  
 *  
 *  Copyright (C) 2017 Josh Vekhter
 *  This example code released under the MIT License - do what thy wilt
 */

#include "whoasense.h"

void setup() {
  // This makes it possible to send messages back to the computer from the board
  // Not strictly necessary for this example, but nice to have around for debugging
  Serial.begin(9600); 

  // This initializes the hardware configuration of the board
  initWhoaBoard();

  // Sanity check, we will be using the Glow array to turn the EL channels on and off
  for (int i = 0; i < 4; i++) { 
    glow[i] = true; 
  }
}

// We will be alternating between having the EL channels on and off
// this controls the total time of the cycle.
int period_us = 5000;
// This controls the time on (in microseconds) 
int dutyCycle_us = 0;
// This steps the dutyCycle up to demonstrate how the brightness can be faded
int stepSize_us = 25;

void loop() {
  switchOutputs(glow);
  delayMicroseconds(dutyCycle_us);
  switchOutputs(noGlow);
  delayMicroseconds(period_us - dutyCycle_us);

  dutyCycle_us = (dutyCycle_us + stepSize_us) % period_us;
}
