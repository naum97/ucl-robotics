/*
  Created by Matthew Bell and Wayne Tsui.
  This programme sets the ActivityBot to go down a narrow passage, stopping at the end, rotating and retracing its steps
  The robot navigates to the end of the passage with sensors, but the movement back to the start point is without sensors.
*/



/*
//Test IR Detectors for Distance.c
#include "simpletools.h"

int irLeft, irRight;

int main()
{
  low(26);                                   
  low(27);                                   

  while(1)
  {
    irLeft = 0;                                     // <- add
    irRight = 0;                                    // <- add

    for(int dacVal = 0; dacVal < 160; dacVal += 8)  // <- add
    {                                               // <- add
      dac_ctr(26, 0, dacVal);                       // <- add
      freqout(11, 1, 38000);                        // <- add
      irLeft += input(10);                          // <- modify
  
      dac_ctr(27, 1, dacVal);                       // <- add
      freqout(1, 1, 38000);
      irRight += input(2);                          // <- modify
    }                                               // <- add

    print("%c irLeft = %d, irRight = %d%c",         // <- modify
           HOME,   irLeft, irRight, CLREOL);        // <- modify
    pause(100);
  }
}
*/


#include "simpletools.h"
#include "abdrive.h"
#include "librobot.h"
#include "ping.h"


int main() {
  int distance, stoppingDist = 5;
  int errorVal, prevErrorVal, totalErrorVal = 0, errorDiff = 0;
  int kp = -8, ki = -4, kd = -4;
  int baseSpd = 86, correctionSpd;
  int irLeft = 0, irRight = 0;

  low(26);
  low(27);

  while(1) {

    // Check front distance.
    distance = ping_cm(8);

    // No obstacle within 5cm, can move forward.
    if (distance > stoppingDist) {

      // Get left wall distance, irLeft. 0<=irLeft<=20. Higher irLeft means object further from left side.
      // Get right wall distance, irRight. 0<=irRight<=20. Higher irRight means object further from right side.
      for (int dacVal = 0; dacVal < 160; dacVal += 8) {
        dac_ctr(26, 0, dacVal);
        freqout(11, 1, 38000);
        irLeft += input(10);                          // Left side IR-sensor

        dac_ctr(27, 1, dacVal);
        freqout(1, 1, 38000);
        irRight += input(2);                          // Right side IR-sensor
      }

      errorVal = irRight - irLeft;
      //print("%d\n",errorVal);

      // If robot moves from too far left/right to too far right/left, without hitting optimal setPoint, reset totalErrorVal.
      if ((prevErrorVal > 0 && errorVal <= 0) || (prevErrorVal < 0 && errorVal >= 0)) {
         totalErrorVal = 0;
      }

      // Measure difference in error values to turn more responsively at sharp turns.
      errorDiff = errorVal - prevErrorVal;

      prevErrorVal = errorVal;
      //print("%d\n", errorDiff);

      // Reset distance measurement.
      irLeft = 0;
      irRight = 0;
      //print("%d\n", errorVal);


      // Robot is 10cm away from left wall, optimal.
      if (errorVal == 0) {

        // Move straight.
        drive_speed(baseSpd, baseSpd);

      }
      else {

        correctionSpd = (kp * errorVal) + (ki * totalErrorVal) + (kd * errorDiff);

        if (correctionSpd > 30) {
          correctionSpd = 30;
        }
        if (correctionSpd < -30) {
          correctionSpd = -30;
        }

        totalErrorVal += errorVal;
        //print("%d\n",totalErrorVal);

        // Robot is too near to right wall.
        if (errorVal < 0) {
          // Move left, correctionSpd is positive.
          drive_speed(baseSpd-correctionSpd, baseSpd);
        }

        // Robot is too near to left wall.
        else if (errorVal > 0) {
          // Move right, correctionSpd is negative.
          drive_speed(baseSpd, baseSpd+correctionSpd);
        }
      }
    }

    // Obstacle within 5cm, cannot move forward, stop.
    else {
      drive_speed(0, 0);
      turn_pivot_function(180);
    }
  }
}
