/* This example shows how to use the WhoaSense to turn EL materials into a visual theramin.  
 *  It combines concepts from example 1 (Dimming via pulse width modulation) and example 5 
 *  (hand tuning a parameter to trigger effects).  
 *  
 *  This example works best with wound up wire or EL strips/panels.  Surface area helps get a reliable
 *  signal at a distance.  Tune the following parameters: 
 *  low_threshold, mid_threshold, midDutyCycle_us, high_threshold, period_us.
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
int threshold = 210;

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


int low_threshold = 300;
int mid_threshold = 370;
int midDutyCycle_us = 3000;
int high_threshold = 450;
int period_us = 5000;

// This temporary variable keeps track of the time on (in microseconds) 
int dutyCycle_us = 0;

int* rawSenseResults;

void loop() {
  // The first argument here controls the "charging time".  
  // Increase the number, get a larger signal (but lose a bit of brightness).
  rawSenseResults = senseAll(1700, false);
  
  Serial.println(signalBuffer);
  int measurement = getProcessedSense(1); 
  if (measurement < low_threshold) { 
    dutyCycle_us = 0;
  }
  else if (measurement > high_threshold) 
  {
    dutyCycle_us = period_us;
  }
  else { 
    // Simple way for compensating for the way that measurements at a distance effect the signal less
    double frac;
    if (measurement > mid_threshold)
    {
      frac = (measurement - mid_threshold) / ((double) high_threshold - mid_threshold);
      dutyCycle_us = (period_us - midDutyCycle_us) * frac + midDutyCycle_us;
    }
    else 
    {
      frac = (measurement - low_threshold) / ((double) mid_threshold - low_threshold);
      dutyCycle_us = midDutyCycle_us * frac;
    }

    Serial.println(frac);
  }

//  Serial.println(dutyCycle_us);

  // PWM code (like in example 1)
  switchOutputs(glow);
  delayMicroseconds(dutyCycle_us);
  switchOutputs(noGlow);
  delayMicroseconds(period_us - dutyCycle_us);
  
}
