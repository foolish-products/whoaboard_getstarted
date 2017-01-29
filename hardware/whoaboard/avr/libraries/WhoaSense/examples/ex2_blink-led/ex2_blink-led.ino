/* This example shows how to use the WhoaSense library to 
 *  control the onboard LEDs.  Can be useful for debugging!
 *  
 *  Copyright (C) 2017 Josh Vekhter
 *  This example code released under the Apache License
 */

#include <whoasense.h>

void setup() {
  initWhoaBoard();
}
void loop() {
  // Individually control onboard LEDs. 
  // NOTE: there are other hardware functions which will use these LEDs as indicators.
  digitalWrite(rxled, HIGH);
  delay(500);
  digitalWrite(rxled, LOW);
  delay(500);
  digitalWrite(rstled, HIGH);
  delay(500);
  digitalWrite(txled, HIGH);
  delay(500);

  // Turn all LEDs on and off - useful for error signaling
  ledsOff();
  delay(500);
  ledsOn();
  delay(500);
  ledsOff();
  
}
