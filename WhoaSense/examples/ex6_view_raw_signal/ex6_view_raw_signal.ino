/* This example shows how to use the WhoaSense library to view 
 *  the raw signal measured by the Whoa Board.  
 *  
 *  This is useful to understand for doing more advanced signal 
 *  processing tricks
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

// This enables raw channel logging
#define ENABLE_RAW_LOGGING_ON_CHAN 1
// This sets the size of measurements to group and sort on the set channel 
#define rawSenseSize 13



// This controls where the lights turn on and off.  
// Tune this parameter to have the EL turn off with touch!
int threshold = 430;

int* rawSenseResults;

void loop() {
  // The first argument here controls the "charging time".  
  // Increase the number, get a larger signal (but lose a bit of brightness).
  rawSenseResults = senseAll(1700, true);

  if (rawSenseHistoryIter == 0) { 
      Serial.println(rawSenseBuffer);
  }
  
}



