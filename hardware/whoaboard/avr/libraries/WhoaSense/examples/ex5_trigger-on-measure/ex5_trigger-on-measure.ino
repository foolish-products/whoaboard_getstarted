/* This example shows how to use the WhoaSense library to view the raw signal 
 *  coming from the whoa board, and to create logic around that.  
 *  
 *  For this example, we will assume your EL element is connected to channel 1.
 *  Please make sure you do this!
 *  
 *  To start with this exercise, try tuning the threshold parameter to 
 *  make your EL element turn off when you touch it.
 *  
 *  Copyright (C) 2017 Josh Vekhter
 *  This example code released under the Apache License
 */
 
// This is needed to tell the libary to start filling the logging buffers.
// Needs to be defined *BEFORE* the whoasense library is included!
#define ENABLE_LOGGING 

#include <whoasense.h>

//////////////////////////////////////////////////////////////////
// Open the serial monitor and see what happens when you 
// increase this parameter.
//
// (try setting it between the values that you see when you 
// don't touch the wire and when you do!)
//////////////////////////////////////////////////////////////////
int threshold = 200;

void setup() {
  Serial.begin(9600); 
  initWhoaBoard();

  whoaConfig.ENABLE_logging = true; // Note logging can cause slight flickering.
  
//////////////////////////////////////////////////////////////////
// More advanced, try tuning these parameters and see what happens 
// to signal stability.  
// 
// There's a bit of magic here under the hood...
// just know that these are knobs which can be tinkered with.  
//
// Note: There is a balance between stability and responsiveness 
//////////////////////////////////////////////////////////////////
  whoaConfig.rawSenseSize = 31; // Decrease to make signals more responsive/noisy
  whoaConfig.sortedRawWindowSize = 15; // Increase to make more responsive
  whoaConfig.sortedRaw_slackToIncrease = 3; // Decrease to make more responsive
}

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
