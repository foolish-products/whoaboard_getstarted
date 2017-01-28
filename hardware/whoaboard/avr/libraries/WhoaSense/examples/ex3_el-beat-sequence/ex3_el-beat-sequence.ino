/* This example shows how to use the WhoaSense library to sequence the 
 *  EL channels on the board.  
 *  
 *  We also show how the main EL supply can be turned on and off for a bit of 
 *  extra sequencing potential!
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

void loop() {
  for (int i = 0; i < 4; i++) { 
    glow[i] = false; 
  }

  glowIndex = (glowIndex + 1) % 5;
  if (glowIndex == 4) { 
    // This function disables the EL supply in a consistent way
    disableELSupply_withSync();
  }
  else { 
    glow[glowIndex] = true;
    switchOutputs(glow);
  }

  delay(500);

  if (glowIndex == 4) { 
    // To understand what this does, plug a peice of EL into the fifth miniJST port
    // (the one on the side of the board, away from the feet).  
    enableELSupply();
  }
}
