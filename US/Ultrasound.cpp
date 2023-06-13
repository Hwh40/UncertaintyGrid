#include <stdio.h>
#include <stdint.h>
#include "Ultrasound.h"
#include <Arduino.h>


uint32_t timeSent;
uint32_t timeRec; 
uint32_t pulseTime;
uint32_t dist;

void USInit(void)
{
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ECHO), ultraHandler, CHANGE);
  GPT1_CR = (GPT1_CR & (~(1 << 6 | 1 << 8))) | (1 << 7) | (1 << 0); 
  GPT1_PR = 140;
}

void ping(int32_t trig)
{
  digitalWriteFast(trig, LOW);
  delayMicroseconds(2);
  digitalWriteFast(trig, HIGH);
  delayMicroseconds(10);
  digitalWriteFast(trig, LOW);
  pulseTime = GPT1_CNT;
}

uint32_t distanceUS(void)
{
  uint32_t tof = 0;
  if (timeSent < timeRec || pulseTime < timeRec) {
    tof = (timeRec - timeSent);
  } 
  if ((tof) > 4000 || pulseTime > timeRec) {
    tof = 4000;
  }
  return tof;
}

void ultraHandler(void)
{
  if (digitalReadFast(ECHO)) {
    timeSent = GPT1_CNT;
  } else{
    timeRec = GPT1_CNT;
    dist = distanceUS();
  }
}

