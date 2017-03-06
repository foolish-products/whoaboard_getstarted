/* This example shows how to control the EL channels of the Whoa Board by sending Keyboard commands
 *  
 *  To use this, open the serial monitor, type a number 1 - 4, and press enter. 
 *  
 *  Copyright (C) 2017 Josh Vekhter
 *  This example code released under the Apache License
 */
#include <whoasense.h>

unsigned long time;
void setup() {
  Serial.begin(9600); 
  initWhoaBoard();
  time = millis();
}

void loop() {
  // Don't update all the time yo!
  if (millis() - time > 100 || millis() - time < 0) {
    switchOutputs(glow);

    // Note the off by 1!  
    char inp = Serial.read();
    if (inp == '1') { 
      glow[0] = !glow[0];
    }
    else if (inp == '2') { 
      glow[1] = !glow[1];
    }
    else if (inp == '3') { 
      glow[2] = !glow[2];
    }
    else if (inp == '4') { 
      glow[3] = !glow[3];
    }
    else if (inp != -1) { 
      ledsOn();
      delay(100);
      ledsOff();
    }
    
    // Print state
    Serial.print(glow[0]);
    Serial.print(glow[1]);
    Serial.print(glow[2]);
    Serial.println(glow[3]);
  }

}
