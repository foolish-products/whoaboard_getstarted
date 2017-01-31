/* This is the main implementation file for the the WhoaSense library.  
 *
 *  Copyright (C) 2017 Josh Vekhter
 *  This code is released under the Apache License
 */

#include "Arduino.h"

#include "whoasense.h"

#define hvDigitalIn 19 // pf6 -> A1   D19
#define hvClock 18     // pf7 -> A0   D18

#define elEnable 20    // pf5 -> A2   D20

#define clearPin 6     // pd7 -> A7   D6#
#define sense1 4       // pd4 -> A6   D4
#define sense2 12      // pd6 -> A11  D12
#define sense3 8       // pb4 -> A8   D8
#define sense4 9       // pb5 -> A9   D9

// This is hacky.
// Work around from this changing when the program is loaded from the IDE.
int capSenseFrequency = 1;

void ledsOff() {
    digitalWrite(rxled, LOW);
    digitalWrite(txled, LOW);
    digitalWrite(rstled, LOW);
}

void ledsOn() {
    digitalWrite(rxled, HIGH);
    digitalWrite(txled, HIGH);
    digitalWrite(rstled, HIGH);
}

int switched[] = {0, 0, 0, 0};
bool glow[] = {true, true, true, true};
bool allGlow[] = {true, true, true, true};
bool noGlow[] = {false, false, false, false};

int adcSensePorts[] = {B00000000, B00000001, B00000011, B00000100};

// Switch Bitbang
int tempGlow[channelCount];
byte tempGlow_unroll[channelCount * 2];
void switchOutputs(bool* glow) {
    // Make pins on the layout correspond to the pins on the switch
    tempGlow[2] = glow[0];
    tempGlow[3] = glow[1];
    tempGlow[1] = glow[2];
    tempGlow[0] = glow[3];
    
    // Process bit flips ahead of sending serial signal
    PORTF = PORTF | B10000000;
    for (int i = 0; i < 8; i++) {
        if (tempGlow[i / 2] == 1) {
            tempGlow_unroll[i] = PORTF | B01000000;
        }
        else {
            tempGlow_unroll[i] = PORTF & B10111111;
        }
    }
    for (int i = 0; i < 8; i++) {
        PORTF = tempGlow_unroll[i];
        //   digitalWrite(hvClock, LOW);
        PORTF = PORTF & B01111111;
        //   digitalWrite(hvClock, HIGH);
        PORTF = PORTF | B10000000;
    }
    PORTF = PORTF | B10000000;
}

// Switch Orchestration
void enableELSupply() { PORTF = PORTF | B00100000; }

int spinner = 0;
void disableELSupply_withSync() {
    PORTF = PORTF | B00100000;
    delayMicroseconds(100);
    
    spinner = 0;
    while(spinner < 10) {
        spinner += bitRead(PINC, 6);
    }
    
    spinner = 0;
    while(spinner < 10) {
        spinner += !bitRead(PINC, 6);
    }
    
    // digitalWrite(elEnable, LOW);
    PORTF = PORTF & B11011111;
    
    switchOutputs(glow);
    delayMicroseconds(100);
}

int sampleVal = 0;
bool toSense_internal[] = {true, true, true, true};
int senseResults_internal[] = {0, 0, 0, 0};

void senseChannels_internal(int chargeDelay_micros) {
    noInterrupts();
    
    // make sure pwm oscillator is running.
    pwm613configure(capSenseFrequency);
    pwmSet13(127);
    
    // digitalWrite(clearPin, HIGH);
    PORTD = PORTD | B10000000;
    
    if (chargeDelay_micros < 1) {
        chargeDelay_micros = 1000;
    }
    
    // Set ADC initialization bits - make sure things haven't gotten misconfigured.
    // 6: right adjust bits // last 5 bits select ADC.
    ADMUX = adcSensePorts[3];
    // high speed mode / 0 / analog selection, extra bit.
    ADCSRB = B10100000;
    // disable adc
    ADCSRA = B00000110;
    
    disableELSupply_withSync();
    
    switchOutputs(noGlow);
    delayMicroseconds(30);
    
    //   digitalWrite(rstled, LOW);
    
    // Begin measurement sequence
    // digitalWrite(clearPin, LOW);
    PORTD = PORTD & B01111111;
    
    delayMicroseconds(chargeDelay_micros);
    // start measurement
    // Go backwords so that channel 4 has the lowest sensitivity and 1 has the highest.
    for(int i = 3; i >= 0; i--) {
        if (toSense_internal[i]) {
            // enable / start / auto trigger / interrupt flag / interrupt enable /// scale /// p.315
            ADCSRA = B11000110;
            delayMicroseconds(50);
            ADMUX = adcSensePorts[(i + 3) % 4];
            while ((ADCSRA & B01000000));
            sampleVal = ADCL;    // store lower byte ADC
            sampleVal += ADCH << 8;  // store higher bytes ADC
            senseResults_internal[i] = sampleVal;
        }
        else {
            ADMUX = adcSensePorts[(i + 3) % 4];
            senseResults_internal[i] = -1;
        }
    }
    
    //  digitalWrite(elEnable, HIGH);
    PORTF = PORTF | B00100000;
    interrupts();
    //    digitalWrite(clearPin, HIGH);
    PORTD = PORTD | B10000000;
    return;
}

// Sensing interfaces!
// Note that the channel here corresponds to the number written on the board,
// which is shifted up by one from the indicies in the senseResults array.
int senseChannel(int channel, int chargeDelay_micros) {
    if (channel < 0 || channel > 4) {
        ledsOn();
    }
    channel = (channel - 1) % 4; // sanitize your inputs.
    for (int i = 0; i < 4; i++) {
        toSense_internal[i] = false;
    }
    toSense_internal[channel] = true;
    senseChannels_internal(chargeDelay_micros);
    
    switchOutputs(glow);
    
    return senseResults_internal[channel];
}

int* senseAll(int chargeDelay_micros, bool isGlow) {
    for (int i = 0; i < 4; i++) {
        toSense_internal[i] = true;
    }
    senseChannels_internal(chargeDelay_micros);
    
    if (isGlow) {
        switchOutputs(glow);
    }
    processSense(senseResults_internal);
    
    return senseResults_internal;
}

// Frequency modes for TIMER4
#define PWM187k 1   // 187500 Hz
#define PWM94k  2   //  93750 Hz
#define PWM47k  3   //  46875 Hz
#define PWM23k  4   //  23437 Hz
#define PWM12k  5   //  11719 Hz
#define PWM6k   6   //   5859 Hz
#define PWM3k   7   //   2930 Hz

// Direct PWM change variables
#define PWM6        OCR4D
#define PWM13       OCR4A

// Terminal count
#define PWM6_13_MAX OCR4C

void ensureCorrectFrequency() {
    switchOutputs(noGlow);
    ledsOff();
    
    capSenseFrequency = PWM187k;
    int read187k = senseChannel(1, 5000);
    read187k = senseChannel(1, 5000);
    read187k = senseChannel(1, 5000);
    
    
    capSenseFrequency = PWM94k;
    int read94k = senseChannel(1, 5000);
    read94k = senseChannel(1, 5000);
    read94k = senseChannel(1, 5000);
    
    if (read94k > read187k) {
        capSenseFrequency = PWM94k;
        digitalWrite(rxled, HIGH);
        if (read94k > 150) {
            digitalWrite(txled, HIGH);
        }
        delay(200);
    }
    else{
        capSenseFrequency = PWM187k;
        digitalWrite(rstled, HIGH);
        if (read187k > 150) {
            digitalWrite(txled, HIGH);
        }
        delay(200);
    }
    ledsOff();
}


void sort(int *a, int len)
{
    quickSort(a, 0, len);
}

void quickSort( int a[], int l, int r)
{
    int j;
    
    if( l < r )
    {
        // divide and conquer
        j = partition( a, l, r);
        quickSort( a, l, j-1);
        quickSort( a, j+1, r);
    }
    
}

int partition( int a[], int l, int r) {
    int pivot, i, j, t;
    pivot = a[l];
    i = l; j = r+1;
    
    while( 1)
    {
        do ++i; while( a[i] <= pivot && i <= r );
        do --j; while( a[j] > pivot );
        if( i >= j ) break;
        t = a[i]; a[i] = a[j]; a[j] = t;
    }
    t = a[l]; a[l] = a[j]; a[j] = t;
    return j;
}

// Configure the PWM clock
// The argument is one of the 7 previously defined modes
void pwm613configure(int mode)
{
    // TCCR4A configuration
    TCCR4A=0;
    
    // TCCR4B configuration
    TCCR4B=mode;
    
    // TCCR4C configuration
    TCCR4C=0;
    
    // TCCR4D configuration
    TCCR4D=0;
    
    // TCCR4D configuration
    TCCR4D=0;
    
    // PLL Configuration
    // Use 96MHz / 2 = 48MHz
    // PLLFRQ=(PLLFRQ&0xCF)|0x30;
    PLLFRQ=(PLLFRQ&0xCF)|0x10; // Will double all frequencies
    
    // Terminal count for Timer 4 PWM
    OCR4C=255;
}

// Set PWM to D13 (Timer4 A)
// Argument is PWM between 0 and 255
void pwmSet13(int value)
{
    OCR4A=value;   // Set PWM value
    DDRC|=1<<7;    // Set Output Mode C7
    TCCR4A=0x82;  // Activate channel A
}

WhoaConfig whoaConfig;
void initWhoaConfig() {
    //////////////////////////////
    // Measurement Knobs
    //////////////////////////////
    
    whoaConfig.rawSenseSize = 31;
    
    whoaConfig.sortedRawWindowSize = 15;
    whoaConfig.sortedRaw_slackToIncrease = 3;
    whoaConfig.sortedRaw_slackToDecrease = 2;
    
    whoaConfig.senseHistorySize = 21;
    
    //////////////////////////////
    // Logging Config
    //////////////////////////////
    whoaConfig.ENABLE_logging = true;
    whoaConfig.ENABLE_rawLogging = false;
    whoaConfig.rawLoggingChannel = 1;
    
    whoaConfig.isSerial = Serial;
};

char signalBuffer[70];
char processSenseBuffer[70];
char rawSenseBuffer[300];

int senseHistory[channelCount][MaxSenseHistorySize];
int senseHistoryIter = 0;

void initWhoaBoard() {
    initWhoaConfig();
    
    // hvSwitch communication
    pinMode(hvDigitalIn, OUTPUT);
    pinMode(hvClock, OUTPUT);
    
    pinMode(clearPin, OUTPUT);
    digitalWrite(clearPin, LOW);
    
    pinMode(10, OUTPUT);
    
    pinMode(elEnable, OUTPUT);
    digitalWrite(elEnable, HIGH);
    
    pinMode(sense1, INPUT);
    pinMode(sense2, INPUT);
    pinMode(sense3, INPUT);
    pinMode(sense4, INPUT);
    
    pinMode(rxled, OUTPUT);
    pinMode(txled, OUTPUT);
    pinMode(rstled, OUTPUT);
    
    analogReference(DEFAULT);
    
    // this should be called after the clear pin is set low
    ensureCorrectFrequency();
    
    int* results = senseAll(1500, true);
    
    for (int chan = 0; chan < 4; chan++) {
        for (int s = 0; s < whoaConfig.senseHistorySize; s++) {
            senseHistory[chan][s] = results[chan];
        }
    }
}

int sortedRawSenseIndex[channelCount][MaxSortedRawWindowSize];
int lagIter = 0;
int senseLag[] = {0, 0, 0, 0};

int rawSenseHistory[channelCount][MaxRawSenseSize];
int sortedRawSenseHistory[MaxRawSenseSize];
int rawSenseHistoryIter = 0;

void processSense(int* rawSenseResults) {
    senseHistoryIter = (senseHistoryIter + 1) % whoaConfig.senseHistorySize;
    lagIter = (lagIter + 1) % whoaConfig.sortedRawWindowSize;
    rawSenseHistoryIter = (rawSenseHistoryIter + 1) % whoaConfig.rawSenseSize;
    
    // The next "smoothed" value
    // The measurements that come from the sensor are a bit noisy, so we do a
    // bit of signal processing magic to make things more stable
    for (int channel = 0; channel < channelCount; channel++) {
        rawSenseHistory[channel][rawSenseHistoryIter] = rawSenseResults[channel];
        
        for (int i = 0; i < whoaConfig.rawSenseSize; i++) {
            sortedRawSenseHistory[i] = rawSenseHistory[channel][i];
        }
        
        sort(sortedRawSenseHistory, whoaConfig.rawSenseSize);
        
        int prevMeasure = senseHistory[channel][(senseHistoryIter - 1 + whoaConfig.senseHistorySize) % whoaConfig.senseHistorySize];
        
        //      prevMeasure -= 1;
        
        int bestNormError = 10000;
        int startInd = sortedRawSenseIndex[channel][(lagIter + whoaConfig.sortedRawWindowSize - 1) % whoaConfig.sortedRawWindowSize] - 2;
        if (startInd < 0) { startInd = 0; }
        if (startInd > whoaConfig.rawSenseSize - 6) { startInd = whoaConfig.rawSenseSize - 6; }
        int endInd = startInd + 5;
        
        if (endInd > whoaConfig.rawSenseSize / 2) {
            prevMeasure -= 1;
        }
        
        for (int i = startInd + 1; i < endInd; i++) {
            int normError = sortedRawSenseHistory[i] - prevMeasure;
            if (normError < 0) {
                normError = prevMeasure - sortedRawSenseHistory[i] + 2;
            }
            
            if (normError < bestNormError) {
                bestNormError = normError;
                sortedRawSenseIndex[channel][lagIter] = i;
            }
            
        }
        
        senseLag[channel] = sortedRawSenseIndex[channel][lagIter] -
        sortedRawSenseIndex[channel][(lagIter + 1) % whoaConfig.sortedRawWindowSize];
        
        if (senseLag[channel] >= whoaConfig.sortedRaw_slackToIncrease) {
            sortedRawSenseIndex[channel][lagIter] = sortedRawSenseIndex[channel][(lagIter + 1) % whoaConfig.sortedRawWindowSize] + whoaConfig.sortedRaw_slackToIncrease;
        }
        
        if (senseLag[channel] <= whoaConfig.sortedRaw_slackToDecrease && endInd > whoaConfig.rawSenseSize / 2) {
            sortedRawSenseIndex[channel][lagIter] = sortedRawSenseIndex[channel][(lagIter + 1) % whoaConfig.sortedRawWindowSize] - whoaConfig.sortedRaw_slackToDecrease;
        }
        
        senseHistory[channel][senseHistoryIter] = sortedRawSenseHistory[sortedRawSenseIndex[channel][lagIter]];
        
        
        if (whoaConfig.ENABLE_rawLogging) {
            if(rawSenseHistoryIter==0  && channel == (whoaConfig.rawLoggingChannel + 3) % channelCount){
                int rawsenselen = 0;
                for(int i=0; i<whoaConfig.rawSenseSize; i++){
                    rawsenselen += snprintf(rawSenseBuffer + rawsenselen, 200, "%d, ", sortedRawSenseHistory[i]);
                }
                rawsenselen += sprintf(rawSenseBuffer + rawsenselen, " Sel: %d", senseHistory[channel][senseHistoryIter]);
                rawsenselen += sprintf(rawSenseBuffer + rawsenselen, " Sel Index: %d", sortedRawSenseIndex[channel][lagIter]);
            }
        }
        
        if (whoaConfig.ENABLE_logging) {
            int processsenselen;
            int signallen;
            switch (channel) {
                case 0:
                    processsenselen = sprintf(processSenseBuffer, "Raw Index/Lag: %d %d, ", sortedRawSenseIndex[channel][lagIter], senseLag[channel]);
                    signallen = sprintf(signalBuffer, "Signal: %d, ", senseHistory[channel][senseHistoryIter]);
                    break;
                    
                case 1:
                case 2:
                case 3:
                    processsenselen += sprintf(processSenseBuffer + processsenselen, "%d %d, ", sortedRawSenseIndex[channel][lagIter], senseLag[channel]);
                    signallen += sprintf(signalBuffer + signallen, "%d, ", senseHistory[channel][senseHistoryIter]);
                    break;
                    
                default:
                    // Serial.print("blah ");
                    break;
            }
            
        }
        
    }
    
    return;
}

int getProcessedSense(int channel) {
    if (channel < 1 || channel > 4) {
        ledsOn();
    }
    channel = (channel + 3) % 4;
    return senseHistory[channel][senseHistoryIter];
}

