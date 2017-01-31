/* This example shows how to use the WhoaSense library to view 
 *  the raw signal measured by the Whoa Board.  
 *  
 *  This is useful to understand for doing more advanced signal 
 *  processing tricks
 *  
 *  Selected refers to the measurement in the window that the algorithm takes 
 *  as the "smoothed" measurement.
 *  
 *  Index Records the "index" of this measurement in the sorted list. 
 *  
 *  Copyright (C) 2017 Josh Vekhter
 *  This example code released under the Apache License
 */

#include <whoasense.h>

void setup() {
  Serial.begin(9600); 
  // This initializes the hardware configuration of the board
  initWhoaBoard();

  // This enables raw (sorted) logging on channel 1
   whoaConfig.ENABLE_rawLogging = true;
   whoaConfig.rawLoggingChannel = 1;

  // View the past "rawSenseSize" on the selected channel (sorted)
  whoaConfig.rawSenseSize = 13;
  whoaConfig.sortedRawWindowSize = 15; // Increase to make more responsive
  whoaConfig.sortedRaw_slackToIncrease = 3; // Decrease to make more responsive
}

int* rawSenseResults;

void loop() {
  // The first argument here controls the "charging time".  
  // Increase the number, get a larger signal (but lose a bit of brightness).
  rawSenseResults = senseAll(1700, true);

  if (rawSenseHistoryIter == 0) { 
      Serial.println(rawSenseBuffer);
  }
  
}



