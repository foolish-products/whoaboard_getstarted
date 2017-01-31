

extern bool isTouched;
extern bool whereTouched[4];

extern bool shouldGlow;
extern int switchedCount;

extern bool ENABLE_logOnTransition;

#define PrintLineSize 200
extern char logBuffer[PrintLineSize];
extern char transitionBuffer[PrintLineSize];

void updateTouchState();
