#include <stdio.h>
#include <stdint.h>
#include "Ultrasound.h"
#include <Arduino.h>


void USInit(void)
{
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
  
}





