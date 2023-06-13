#ifndef IMU_H
#define IMU_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

extern Adafruit_BNO055 bno ;
extern double angle;
extern sensors_event_t orientationData;
extern double initial;
 

typedef struct {
  int32_t x;
  int32_t y;
  double orientation;
} Position_t;

void initIMU(void);

double getAngle(void);

#endif 