#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "Ultrasound.h"

#define alpha 1
#define DX 50
#define DY 50
#define DTHETA 1
#define DR 50
#define LENGTH 60

typedef struct {
  int32_t x;
  int32_t y;
  double orientation;
} Position_t;

//Ultrasound
int32_t ECHO = 2;
int32_t TRIG = 3;
int8_t flag = 0;
uint32_t timeSent;
uint32_t timeRec; 
uint32_t dist = 0;
uint32_t distanceUS(void);
void ultraHandler(void);

//Uncertainty
Position_t currentPos = {LENGTH / 2, LENGTH / 2, 0};
int32_t Map[LENGTH][LENGTH];

//IMU
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
double angle = 0;
sensors_event_t orientationData;
double initial = 0;
uint32_t count = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Running");

  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ECHO), ultraHandler, CHANGE);
  USInit();

  initMap(Map);
  if (!bno.begin()) {
    Serial.print("No BNO055 detected");
    while (1);
  }
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  initial = orientationData.orientation.x;
}

void loop() {
  ping(TRIG);

  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  currentPos.orientation = angle;
  angle = -1 *(orientationData.orientation.x) - initial + 90;

  if (flag == 1) {
    uint32_t distance = distanceUS();
    dist = 0.95 * dist + 0.05 * distance;
    setM(dist);
    flag = 0;
  }

  if (count % 20==0) {
    displayMap(Map);  
    count = 0;  
  }

  count++;
  delay(50);
}

//Uncertainty grid Functions
int32_t deltaX(double r, double theta)
{
  int32_t n;
  n = r * cos(theta * 0.01745) / DX;
  return n;
}

int32_t deltaY(double r, double theta)
{
  int32_t n;
  n = r * sin(theta * 0.01745) / DY;
  return n;
}

double probability(int32_t m)
{
  double p;
  p = 1 + exp(-1 * alpha * m);
  return (1 / p);
}

void setM(uint32_t distance)
{
  int32_t x = 0;
  int32_t y = 0;
  for (int32_t i = 0; i < (15 / DTHETA + 1); i++) {
    int32_t dx = deltaX(distance, currentPos.orientation + -7.5 + i * DTHETA) + currentPos.x;
    int32_t dy = deltaY(distance, currentPos.orientation + -7.5 + i * DTHETA) + currentPos.y;
    if ((dx != x || dy != y)) {
      x = dx ;
      y = dy ;
      if (x <= LENGTH - 1 && y <= LENGTH - 1) {
        Map[x][y]++;
      }
    }
  }

  int32_t r = distance;
  for (int32_t j = 1; j < distance / DR; j++) {
    r = r - DR;
    for (int32_t i = 0; i < (15 / DTHETA + 1); i++) {
      int32_t dx = deltaX(r, currentPos.orientation + -7.5 + i * DTHETA) + currentPos.x;
      int32_t dy = deltaY(r, currentPos.orientation + -7.5 + i * DTHETA) + currentPos.y;
      if ((dx != x || dy != y)) {
        x = dx ;
        y = dy ;
        if (x <= LENGTH - 1 && y <= LENGTH - 1) {
          Map[x][y]--;
        }
      }
    }
  }
}

void displayMap(int32_t arr[LENGTH][LENGTH])
{
  double prob;
  Serial.println();
  for (int32_t i = 0; i < LENGTH; i++) {
    Serial.print(i);
    Serial.print(":\t");
    for (int32_t j = 0; j < LENGTH; j++){
      prob = probability(arr[i][j]);
      if (i == currentPos.x && j == currentPos.y) {
        Serial.print("O");
      } else {
        if (prob > 0.9) {
          Serial.print("x");
        } else if (prob < 0.1) {
          Serial.print(" ");
        } else {
          Serial.print("-");
        }
      }
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println();
}

void initMap(int32_t arr[LENGTH][LENGTH])
{
  for (int32_t i = 0; i < LENGTH; i++) {
    for (int32_t j = 0; j < LENGTH; j++){
      arr[i][j] = 0;
    }
  }
}

//Ultrasound functions
uint32_t distanceUS(void)
{
  uint32_t tof = 0;
  if (timeSent < timeRec) {
    tof = (timeRec - timeSent);
  } 
  if ((tof) > 4000) {
    tof = 4000;
  }
  return tof;
}

void ultraHandler(void)
{
  if (flag == 0) {
    if (digitalReadFast(ECHO)) {
      timeSent = GPT1_CNT;
    } else{
      timeRec = GPT1_CNT;
      flag = 1;
    }
  }
}
