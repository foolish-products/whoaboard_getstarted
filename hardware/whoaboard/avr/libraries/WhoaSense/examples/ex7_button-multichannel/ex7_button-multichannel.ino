/* This example is more involved, and represents the code which comes loaded on
 *  a Whoa Board.
 *  
 *  The sketch begins with a number of levers which can be tuned in order to 
 *  increase the sensitivity and responsiveness of this sketch by specializing it 
 *  to your particular usecase.  
 *  
 *  Note that there is quite a bit of room to do this (we had to make a number of 
 *  comprimises in order to ship a script that performs robustly with a wide diversity 
 *  of EL loads), and with a bit of work it should be possible to make your 
 *  installation/wearable quite responsive.  
 *  
 *  Copyright (C) 2017 Josh Vekhter
 *  This example code released under the Apache License
 */
#include <whoasense.h>

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

// If you want to add some custom functionality, to this script, put it in this function
void doSomeOtherNonsense() { return; }

bool ENABLE_logOnTransition;
void setup() {
  Serial.begin(9600); 
  initWhoaBoard();

//////////////////////////////////////////////////////////////////
// Logging Levers!
//////////////////////////////////////////////////////////////////
   ENABLE_logOnTransition = true;
   whoaConfig.ENABLE_logging = true;
   whoaConfig.ENABLE_rawLogging = true;
   whoaConfig.rawLoggingChannel = 1;

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


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// Implementation dragons below!
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


char impulseBuffer[50];
char derivativeBuffer[50];

#define PrintLineSize 200
char logBuffer[PrintLineSize];

bool isTouched = false;
bool whereTouched[] = {false, false, false, false};

bool shouldGlow = true;

int switchedCount = 0;

int chanImpulse; 
int chanChangeCount;

int* rawSenseResults;
bool isPastThreshold;

void loop() {
    rawSenseResults = senseAll(1700, true);
    
    doSomeOtherNonsense();
    
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
          snprintf(logBuffer, PrintLineSize, "Switched channel: %d Measurement: %d Impulse: %d Adj transition count: %d",
            channel, senseHistory[channel][senseHistoryIter], chanImpulse, chanChangeCount);
          Serial.println(logBuffer);
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
    
      Serial.println(logBuffer);
    }

    if (whoaConfig.ENABLE_rawLogging) {
      if (rawSenseHistoryIter == 0) { 
        Serial.println(rawSenseBuffer);
      }
    }

    if (isTouched && switchedCount == 0) { 
      isTouched = false;
      for (int chan = 0; chan < channelCount; chan++) {
        if (whereTouched[chan]) { 
        if (whoaConfig.ENABLE_logging) { 
          Serial.println("SWITCHED CHANNEL!");
          Serial.println(chan);
        }
          whereTouched[chan] = false;
          glow[chan] = !glow[chan];
        }
      }
      switchedCount = whoaConfig.rawSenseSize + 
                      whoaConfig.senseHistorySize + 
                      whoaConfig.sortedRawWindowSize;
    }



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
