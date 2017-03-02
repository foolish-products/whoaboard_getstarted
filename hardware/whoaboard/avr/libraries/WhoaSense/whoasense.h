/* This is the main header file for the the WhoaSense library.
 *
 * This library is built on the Whoa Board, and takes care of hardware configuration and 
 * does some signal processing to simplify the process of working on projects that incorporate 
 * interactive electroluminescent (EL) elements!  
 *
 *  Main site: http://whoaboard.com
 *  Documentation: http://whoaboard.com/docs
 *
 *  Copyright (C) 2017 Josh Vekhter
 *  This code is released under the Apache License
 */

#ifndef WhoaSense_h
#define WhoaSense_h

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
    
    struct WhoaConfig {
        //////////////////////////////
        // Measurement Knobs
        //////////////////////////////
        
        int rawSenseSize;
        
        int sortedRawWindowSize;
        int sortedRaw_slackToIncrease;
        int sortedRaw_slackToDecrease;
        
        int senseHistorySize;
        
        //////////////////////////////
        // Logging Config
        //////////////////////////////
        bool ENABLE_logging;
        bool ENABLE_rawLogging;
        int rawLoggingChannel;// Select a channel with this (1 - 4), to print a raw sorted output.
        
        bool isSerial;
    };
    
    extern WhoaConfig whoaConfig;
    
    /* -----------------------------------------------------------
     * First we look at the window of previous raw measurements
     * Next we sort them, and from this sorted list we choose an index
     * This value is the measurement at any given time.
     * It might seem a bit confusing, but doing things this way helps
     * to produce a more stable signal.
     *
     * You can tweak the levers that control these windows here or in your arduino script
     */
    
    
#define MaxRawSenseSize 31
#define MaxSortedRawWindowSize 15
#define MaxSenseHistorySize 21

    void processSense(int* rawSenseResults);
    extern int senseHistoryIter;
    extern int rawSenseHistoryIter;
    
    int getProcessedSense(int channel);
    extern int senseHistory[channelCount][MaxSenseHistorySize];
    
    // Write to a buffer instead of repeatedly calling Serial.print to generate logging.
    // If your wire starts flashing when you are logging, this might be why!
    // Select a channel with this (0 - 3), to print a raw sorted output.
    extern char rawSenseBuffer[300];
    
    
    extern char signalBuffer[70];
    extern char processSenseBuffer[70];
    
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
