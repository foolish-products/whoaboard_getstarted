/* This example shows how to use the WhoaSense library to view the raw signal 
 *  coming from the whoa board, and to create logic around that.  
 *  
 *  For this example, we will assume your EL element is connected to channel 1.
 *  Please make sure you do this!
 *  
 *  Copyright (C) 2017 Josh Vekhter
 *  This example code released under the Apache License
 */
 
// This is needed to tell the libary to start filling the logging buffers.
// Needs to be defined *BEFORE* the whoasense library is included!
#define ENABLE_LOGGING 

#include <whoasense.h>

void setup() {
  Serial.begin(9600); 
  // This initializes the hardware configuration of the board
  initWhoaBoard();
}

// We change these library level parameters in order to make the raw output signal 
// more responsive.  
// 
// There's a bit of magic here under the hood...
// just know that these are knobs which can be tinkered with.  
#define rawSenseSize 13 // Decreasing this will make signals more responsive/noisy
#define sortedRawWindowSize 15 // Increasing this + decreasing the amount of slack to make the signal more responsive
#define sortedRaw_slackToIncrease 1
#define sortedRaw_slackToDecrease -5 // Make this closer to 0 to make the signal more stable


// This controls where the lights turn on and off.  
// Tune this parameter to have the EL turn off with touch!
int threshold = 430;

int* rawSenseResults;

void loop() {
  // The first argument here controls the "charging time".  
  // Increase the number, get a larger signal (but lose a bit of brightness).
  rawSenseResults = senseAll(1700, true);

  Serial.println(signalBuffer);
  if (getProcessedSense(1) < threshold) { 
    glow[0] = false;
  }
  else { 
    glow[0] = true;
  }
  
}



