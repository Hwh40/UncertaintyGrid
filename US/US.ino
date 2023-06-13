#include "Ultrasound.h"
#include "IMU.h"
#include <stdint.h>

#define alpha 1

//Resolution in mm, mm, mm, radian
#define DX 100
#define DY 100
#define DR 100
#define DTHETA 1

//Width (x) and Length (y) of grid in M
#define LENGTH 8
#define WIDTH 8

const int32_t Width = WIDTH * 1000 / DX;
const int32_t Length = LENGTH * 1000 / DY;

Position_t currentPos = {Width / 2, Length / 2, 0};
int32_t Map[Width][Length];

uint32_t count = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Running");
  USInit();
  initMap(Map);
  initIMU();
}

void loop() {
  ping(TRIG);
  currentPos.orientation = getAngle();
  setM(dist);
  if (count % 20 == 0) {
    Serial.print("Distance: \t");
    Serial.println(dist);
    displayMap(Map);  
    count = 0;  
  }
  count++;
  delay(50);
}


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
  for (uint32_t i = 0; i < (15 / DTHETA + 1); i++) {
    int32_t dx = deltaX(distance, currentPos.orientation + -7.5 + i * DTHETA) + currentPos.x;
    int32_t dy = deltaY(distance, currentPos.orientation + -7.5 + i * DTHETA) + currentPos.y;
    if ((dx != x || dy != y)) {
      x = dx ;
      y = dy ;
      if (x <= Width - 1 && y <= Length - 1) {
        Map[x][y]++;
      }
    }
  }

  int32_t r = distance;
  for (uint32_t j = 1; j < distance / DR; j++) {
    r = r - DR;
    for (int32_t i = 0; i < (15 / DTHETA + 1); i++) {
      int32_t dx = deltaX(r, currentPos.orientation + -7.5 + i * DTHETA) + currentPos.x;
      int32_t dy = deltaY(r, currentPos.orientation + -7.5 + i * DTHETA) + currentPos.y;
      if ((dx != x || dy != y)) {
        x = dx ;
        y = dy ;
        if (x <= Width - 1 && y <= Length - 1) {
          Map[x][y]--;
          Map[x][y]--;
          Map[x][y]--;
        }
      }
    }
  }
}

void displayMap(int32_t arr[Width][Length])
{
  double prob;
  Serial.println();
  for (int32_t i = 0; i < Width; i++) {
    Serial.print(i);
    Serial.print(":\t");
    for (int32_t j = 0; j < Length; j++){
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

void initMap(int32_t arr[Width][Length])
{
  for (int32_t i = 0; i < Width; i++) {
    for (int32_t j = 0; j < Length; j++){
      arr[i][j] = 0;
    }
  }
}


