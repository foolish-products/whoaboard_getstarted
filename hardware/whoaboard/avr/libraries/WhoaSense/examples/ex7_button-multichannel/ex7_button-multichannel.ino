#define ENABLE_LOGGING

#include <whoasense.h>

void setup() {
  Serial.begin(9600); 
  initWhoaBoard();

}

#define subBufferSize 50
char impulseBuffer[subBufferSize];
char derivativeBuffer[subBufferSize];
#endif

#define printLineSize 200
char logBuffer[printLineSize];

int val = 0;

int minimum = 10000;
int maximum = 0;

bool isTouched = false;
bool whereTouched[] = {false, false, false, false};

float change = 0;

bool shouldGlow = true;

bool isOn = true;
bool justSwitched = false;
int switchedCount = 0;

int chanImpulse; 
int chanChangeCount;

// Here are the main levers which control the operation of this program
#define increaseWindow 17
#define pureIncreaseThreshold 7
int impulseThresholdBars[] = {0, 3, 3, 17};
bool isPastThreshold;
//#define glowTime_us 900

int* rawSenseResults;

void loop() {
      rawSenseResults = senseAll(1700, true);
    
 //   delayMicroseconds(glowTime_us);
    


    for (int channel = 0; channel < channelCount; channel++) { 
      chanImpulse =  senseHistory[channel][(senseHistoryIter + 1 + senseHistorySize) % senseHistorySize] 
                     - senseHistory[channel][senseHistoryIter];
      chanChangeCount = 0;

      int senseDiff;
      for (int steps = 0; steps < increaseWindow; steps++) {
        senseDiff = senseHistory[channel][(senseHistoryIter - (steps + 1) + senseHistorySize) % senseHistorySize]
            - senseHistory[channel][(senseHistoryIter - (steps) + senseHistorySize) % senseHistorySize];
        
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
#ifndef ENABLE_LOGGING
        sprintf(logBuffer, "Switched channel: %d Measurement: %d Impulse: %d Adj transition count: %d",
          channel, senseHistory[channel][senseHistoryIter], chanImpulse, chanChangeCount);
        Serial.println(logBuffer);
#endif
      }
#ifdef ENABLE_LOGGING

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
#endif
 
    }
    


#ifdef ENABLE_LOGGING
  snprintf(logBuffer, printLineSize, "%20s %23s %20s %20s %s",
           signalBuffer,
           impulseBuffer, 
           derivativeBuffer,
           processSenseBuffer, "");

  Serial.println(logBuffer);
#endif

    

    if (isTouched && switchedCount == 0) { 
      isTouched = false;
      for (int chan = 0; chan < channelCount; chan++) {
        if (whereTouched[chan]) { 
#ifdef ENABLE_LOGGING
          Serial.println("SWITCHED CHANNEL!");
          Serial.println(chan);
#endif
          whereTouched[chan] = false;
          glow[chan] = !glow[chan];
        }
      }
      switchedCount = rawSenseSize + senseHistorySize + sortedRawWindowSize;
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
