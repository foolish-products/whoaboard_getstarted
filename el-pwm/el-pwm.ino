#include "whoasense.h"

void setup() {
  
  Serial.begin(9600); 
  initWhoaBoard();

  for (int i = 0; i < 4; i++) { 
    glow[i] = true; 
  }

}

int period_us = 10000;
int dutyCycle_us = 0;


void loop() {
  switchOutputs(glow);
  delayMicroseconds(dutyCycle_us);
  switchOutputs(noGlow);
  delayMicroseconds(period_us - dutyCycle_us);

  dutyCycle_us = (dutyCycle_us + 100) % period_us;
}
