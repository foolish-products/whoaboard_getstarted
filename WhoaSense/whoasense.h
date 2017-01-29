#ifndef WhoaSense_h
#define WhoaSense_h

#ifndef ENABLE_LOGGING
#define ENABLE_LOGGING 
#endif

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

// LED Pins!

#define rxled 21
#define txled 22
#define rstled 7

void ledsOff();
void ledsOn();

// Glow State

#define channelCount 4
extern int switched[channelCount];
extern bool glow[channelCount];
extern bool allGlow[channelCount];
extern bool noGlow[channelCount];

void switchOutputs(bool* glow);
void disableELSupply_withSync();
void enableELSupply();

 // Sensing interfaces! 
// Note that the channel here corresponds to the number written on the board, 
// which is shifted up by one from the indicies in the senseResults array.
int senseChannel(int channel, int chargeDelay_micros);
int* senseAll(int chargeDelay_micros, bool isGlow);

/* -----------------------------------------------------------
 * First we look at the window of previous raw measurements
 * Next we sort them, and from this sorted list we choose an index 
 * This value is the measurement at any given time.  
 * It might seem a bit confusing, but doing things this way helps 
 * to produce a more stable signal.  
 * 
 * You can tweak the levers that control these windows here or in your arduino script
 */

#ifndef rawSenseSize
#define rawSenseSize 31 // Increase for a more stable (but less responsive signal)
#endif
#ifndef sortedRawWindowSize
#define sortedRawWindowSize 15 // Increase this and decrease the slack to make the signal more responsive (but noisier)
#define sortedRaw_slackToIncrease 3 
#define sortedRaw_slackToDecrease 2
#endif
#ifndef senseHistorySize
#define senseHistorySize 21
#endif

void processSense(int* rawSenseResults);
extern int senseHistoryIter;
extern int rawSenseHistoryIter;

int getProcessedSense(int channel);
extern int senseHistory[channelCount][senseHistorySize];

// Write to a buffer instead of repeatedly calling Serial.print to generate logging. 
// If your wire starts flashing when you are logging, this might be why!
#ifdef ENABLE_RAW_LOGGING_ON_CHAN // Select a channel with this (1 - 4), to print a raw sorted output.
char rawSenseBuffer[200];
#endif


// #ifdef ENABLE_LOGGING
extern char signalBuffer[50];
extern char processSenseBuffer[50];
// #endif

void initWhoaBoard();

// Private - don't mess with this stuff!

// This is a hack to account for some silliness in setting the fast PWM frequency
// that occurs when you program the board from the arduino IDE
void ensureCorrectFrequency();

// Opaque bitwise port manipulation to speed up measurement!
void senseChannels_internal(int chargeDelay_micros);

void sort(int *a, int len);
void quickSort( int[], int, int);
int partition( int[], int, int);

// Configuration 
// Fast PWM config magix
void pwm613configure(int mode);
void pwmSet13(int value);

#ifdef __cplusplus
}
#endif

#endif
