#ifndef ULTRASOUND_H
#define ULTRASOUND_H


#include <stdio.h>
#include <stdint.h>
#include <Arduino.h>

#define ECHO 2
#define TRIG 3

extern uint32_t timeSent;
extern uint32_t timeRec; 
extern uint32_t pulseTime;
extern uint32_t dist;

void USInit(void);

void ping(int32_t);

uint32_t distanceUS(void);

void ultraHandler(void);

#endif