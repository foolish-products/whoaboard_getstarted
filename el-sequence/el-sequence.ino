#include "whoasense.h"

void setup() {
  
  Serial.begin(9600); 
  initWhoaBoard();

}

int glowIndex = 0;

void loop() {
  for (int i = 0; i < 4; i++) { 
    glow[i] = false; 
  }

  glowIndex = (glowIndex + 1) % 4;
  glow[glowIndex] = true;
  switchOutputs(glow);

  delay(500);
}
