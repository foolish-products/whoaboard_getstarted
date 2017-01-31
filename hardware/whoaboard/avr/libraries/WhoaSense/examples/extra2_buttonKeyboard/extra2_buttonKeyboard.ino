/* This example shows how the button example can be used to send keyboard messages to a computer.  
 *  
 *  Note, after flashing the program on the board, you may need to unplug and plug in the board before it 
 *  starts working.  It may also take a moment for your computer to recongize the board as a "valid" keyboard.  
 *  
 *  Once you see it printing symbols, head over to http://patatap.com/, and make your computer make some 
 *  animations in response to EL touches! 
 *  
 *  Also note, you may need to press the reset button after you press the upload button in order to successfully 
 *  flash this program onto the board.
 *  
 *  Copyright (C) 2017 Josh Vekhter
 *  This example code released under the Apache License
 */
#include <Keyboard.h>
 
#include <whoasense.h>
#include "whoaButton.h"



// If you want to add some custom functionality, to this script, put it in this function
void doSomeOtherNonsense() { return; }

bool toLog = false;
void setup() {
  Serial.begin(9600); 
  // This is new!
  Keyboard.begin();
  
  initWhoaBoard();

  ENABLE_logOnTransition = false;
  whoaConfig.ENABLE_logging = false;
  toLog = whoaConfig.ENABLE_logging || ENABLE_logOnTransition;
}

char channelToKeyMapping[] = {87, 65, 83, 68};// {'W', 'A', 'S', 'D'};

void loop() {
    senseAll(1700, true);

    updateTouchState();

    if (isTouched && switchedCount == 0) { 
      isTouched = false;
      for (int chan = 0; chan < channelCount; chan++) {
        if (whereTouched[chan]) { 
          whereTouched[chan] = false;
          glow[chan] = !glow[chan];

          // Note Serial.println calls seem to confuse the Keyboard.write, so you need to alternate 
          // between the two when debugging!
          if (!toLog) { 
            // This is the keyboard step: 
            Keyboard.write(channelToKeyMapping[chan]);
          }
        }
      }
      if (ENABLE_logOnTransition) {
        Serial.println(transitionBuffer);
      }
      switchedCount = whoaConfig.rawSenseSize + 
                      whoaConfig.senseHistorySize + 
                      whoaConfig.sortedRawWindowSize;
    }

    if (whoaConfig.ENABLE_logging) { 
       Serial.println(logBuffer);
    }
    

    


    // Doing this twice helps with sensitivity, for some arcane timing reason.  
    disableELSupply_withSync();
    switchOutputs(glow);
    enableELSupply();
    disableELSupply_withSync();
    switchOutputs(glow);
    enableELSupply();

    if (switchedCount > 0) {
      switchedCount--;
    }

}
