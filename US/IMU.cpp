#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "IMU.h"

Adafruit_BNO055 bno;
double angle ;
sensors_event_t orientationData;
double initial;


void initIMU(void)
{
  bno = Adafruit_BNO055(55, 0x28);
  angle = 0;
  initial = 0;

  if (!bno.begin()) {
    Serial.print("No BNO055 detected");
    while (1) {
      continue;
    }
  }
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  initial = orientationData.orientation.x;
}

double getAngle(void)
{
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  angle = -1 *(orientationData.orientation.x) - initial + 90;
  return angle;
}