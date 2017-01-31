
#include <whoasense.h>
#include "whoaButton.h"
#include "Arduino.h"

bool isTouched = false;
bool whereTouched[] = {false, false, false, false};

bool shouldGlow = true;

int switchedCount = 0;

bool ENABLE_logOnTransition = false;

////////////////////////////////////////
char impulseBuffer[50];
char derivativeBuffer[50];

char logBuffer[PrintLineSize];
char transitionBuffer[PrintLineSize];

int chanImpulse; 
int chanChangeCount;

int* rawSenseResults;
bool isPastThreshold;

////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Here are the main levers which control the operation of this program
////////////////////////////////////////////////////////////////////////
// Things work like this.  We have a stream of measurements on each channel.
// To decide if we want to trigger a channel to switch state (like a push button)
// We look at the past "increaseWindow" number of measurements. 
#define increaseWindow 17 // Increase to increase sensitivity

// Then we track the number of times that our measured signal has decreased 
// within this window.  
#define pureIncreaseThreshold 7 // Decrease this to increase sensitivity

// If we haven't reached the above threshold, we allow a state transition to take place
// if the "impulse" (the magnitude of the change of the signal across the window)
// exceeds the values in this array).  Later entries correspond to lower increase thresholds.
int impulseThresholdBars[] = {0, 3, 3, 17}; // Decrease this to increase sensitivity




void updateTouchState() {
    for (int channel = 0; channel < channelCount; channel++) { 
      chanImpulse =  senseHistory[channel][(senseHistoryIter + 1 + whoaConfig.senseHistorySize) % whoaConfig.senseHistorySize] 
                     - senseHistory[channel][senseHistoryIter];
      chanChangeCount = 0;

      int senseDiff;
      for (int steps = 0; steps < increaseWindow; steps++) {
        senseDiff = senseHistory[channel][(senseHistoryIter - (steps + 1) + whoaConfig.senseHistorySize) % whoaConfig.senseHistorySize]
            - senseHistory[channel][(senseHistoryIter - (steps) + whoaConfig.senseHistorySize) % whoaConfig.senseHistorySize];
        
        if (senseDiff > 0) {
          chanChangeCount += 1; 
        }
        if (senseDiff < -2) {
          chanChangeCount = -1; 
          break;
        }
      }
      

      if (chanChangeCount >= pureIncreaseThreshold) { 
        isPastThreshold = true;
      }
      else if (chanChangeCount > pureIncreaseThreshold - 3) { 
        if (impulseThresholdBars[pureIncreaseThreshold - chanChangeCount] < chanImpulse) { 
          isPastThreshold = true;
        }
        else { 
          isPastThreshold = false;
        }
      }
      else { 
        isPastThreshold = false;
      }

      
      if (switchedCount == 0 && isPastThreshold){
        isTouched = true;
        whereTouched[channel] = true;
        
        if (ENABLE_logOnTransition == true) { 
          snprintf(transitionBuffer, PrintLineSize, "Switched channel: %d Measurement: %d Impulse: %d Adj transition count: %d",
            channel, senseHistory[channel][senseHistoryIter], chanImpulse, chanChangeCount);
      //    Serial.println(transitionBuffer);
        }
      }

      if (whoaConfig.ENABLE_logging) { 
        // EXEX - Logging inline - pull into a function!
        int derivativelen;
        int impulselen;
    
        switch (channel) {
          case 0:
            impulselen = sprintf(impulseBuffer, "Imp: %d, ",
                                 chanImpulse);
            derivativelen = sprintf(derivativeBuffer, "Change: %d, ",
                                    chanChangeCount);
    
            break;
    
          case 1:
          case 2:
          case 3:
            impulselen += sprintf(impulseBuffer + impulselen, "%d, ",
                                  chanImpulse);
            derivativelen += sprintf(derivativeBuffer + derivativelen, "%d, ",
                                     chanChangeCount);
            break;
    
          default:
            break;
        }
      }
   
    }
    if (whoaConfig.ENABLE_logging) {
      snprintf(logBuffer, PrintLineSize, "%20s %23s %20s %20s %s",
               signalBuffer,
               impulseBuffer, 
               derivativeBuffer,
               processSenseBuffer, "");
    }

    
    return;
}
