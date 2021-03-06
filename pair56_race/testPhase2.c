/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials
*/
#include "simpletools.h"                      // Include simple tools
#include "abdrive.h"
#include "ping.h"
#include <stdlib.h>

#define _WHEEL_BASE 105.8

#define _MOVE_UNIT (int)(407/3.25)


int move_shortLeftCurve() {
  int left_dist = (int)( ((float)_MOVE_UNIT - (_WHEEL_BASE/3.25)) * PI / 4 );
  int right_dist = (int)( ((float)_MOVE_UNIT + (_WHEEL_BASE/3.25)) * PI / 4 );
  // print("LEFT: %d, RIGHT: %d\n", left_dist, right_dist);
  float ratio = (float)left_dist / (float)right_dist;
  // print("RATIO: %f\n", ratio);
  drive_speed((int)( 128 * ratio ),128);
  int tick_acc[2];
  drive_getTicks(&tick_acc[0], &tick_acc[1]);
  int tmp[2] = {tick_acc[0], tick_acc[1]};
  while (( tmp[0] - tick_acc[0] < left_dist) && ( tmp[1] - tick_acc[1] < right_dist)) {
    // pause(10);
    drive_getTicks(&tmp[0], &tmp[1]);
    // print("LEFT: %d, RIGHT: %d\n", tmp[0], tmp[1]);
  }
  drive_speed(128,128);
  return (tmp[0] + tmp[1] - tick_acc[0] - tick_acc[1]) / 2;
}

int move_shortRightCurve() {
  int right_dist = (int)( ((float)_MOVE_UNIT - (_WHEEL_BASE/3.25)) * PI / 4 );
  int left_dist = (int)( ((float)_MOVE_UNIT + (_WHEEL_BASE/3.25)) * PI / 4 );
  // print("LEFT: %d, RIGHT: %d\n", left_dist, right_dist);
  float ratio = (float)right_dist / (float)left_dist;
  // print("RATIO: %f\n", ratio);
  drive_speed(128, (int)( 128 * ratio ));
  int tick_acc[2];
  drive_getTicks(&tick_acc[0], &tick_acc[1]);
  int tmp[2] = {tick_acc[0], tick_acc[1]};
  while (( tmp[0] - tick_acc[0] < left_dist) && ( tmp[1] - tick_acc[1] < right_dist)) {
    // pause(10);
    drive_getTicks(&tmp[0], &tmp[1]);
    // print("LEFT: %d, RIGHT: %d\n", tmp[0], tmp[1]);
  }
  drive_speed(128,128);
  return (tmp[0] + tmp[1] - tick_acc[0] - tick_acc[1]) / 2;
}

int count_path_corners(int *pathRace, int pathRaceLength) {
  int numberOfCorners = 0;
  for (int i = 1; i < pathRaceLength-1; i++) {
    if ((pathRace[i] - pathRace[i-1]) != (pathRace[i+1] - pathRace[i])) {
      numberOfCorners++;
    }
  }
  return numberOfCorners;
}


int main()                                    // Main function
{
  int irLeft = 0, irRight = 0;
  int ticksCounter = 0;
  int errorVal, prevErrorVal, totalErrorVal = 0, errorDiff = 0;
  int kp = -2, ki = -1, kd = -1;
  // int kp = 0, ki = 0, kd = 0;              // For no PID testing
  int baseSpd = 128, raceSpd = 128, correctionSpd;
  int ticks[2];
  ticks[0] = 0, ticks[1] = 0;

  int pathRace[8] = {-3, 1, 5, 9, 13, 14, 15, 16};
  int pathRaceLength = 8;

  // Phase 2 Route Optimisation Variables
  int isCurving = 0;
  int boxNumDiff_Ahead = 0;
  int boxNumDiff_Behind = 0;
  // Reset ticksCounter
  drive_getTicks(&ticks[0], &ticks[1]);
  ticksCounter = (ticks[0] + ticks[1])/2;

  // Test count path corners function.
  int numberOfCorners = count_path_corners(pathRace, 8);
  // print("Number of Corners: %d.\n", numberOfCorners);


  for (int i = 1; i < pathRaceLength-1; i++) {
    // print("Cycle: %d\n", i);

    boxNumDiff_Ahead = pathRace[i+1] - pathRace[i];
    boxNumDiff_Behind = pathRace[i] - pathRace[i-1];

    if ((abs(boxNumDiff_Behind) == 4 && abs(boxNumDiff_Ahead) == 4) ||
        (abs(boxNumDiff_Behind) == 1 && abs(boxNumDiff_Ahead) == 1)) {
      if (isCurving) {
        // print("Straight-Curving\n");
        // Add required ticks to ticksCounter
        ticksCounter += (int)_MOVE_UNIT/2;
        // move HALF standard unit
        while(((ticks[0]+ticks[1])/2) < ticksCounter) {
          for (int dacVal = 0; dacVal < 160; dacVal += 8) {
            dac_ctr(26, 0, dacVal);
            freqout(11,  1, 38000);
            irLeft += input(10);                          // Left side IR-sensor

            dac_ctr(27, 1, dacVal);
            freqout(1, 1, 38000);
            irRight += input(2);                          // Right side IR-sensor
          }

          errorVal = irRight - irLeft;

          if ((prevErrorVal > 0 && errorVal <= 0) || (prevErrorVal < 0 && errorVal >= 0)) {
             totalErrorVal = 0;
          }

          errorDiff = errorVal - prevErrorVal;

          prevErrorVal = errorVal;

          irLeft = 0;
          irRight = 0;

          correctionSpd = (kp * errorVal) + (ki * totalErrorVal) + (kd * errorDiff);

          if (errorVal == 0) {
            drive_speed(raceSpd, raceSpd);
          }

          // Robot is too near to right wall.
          else if (errorVal < 0) {
            // Move left, correctionSpd is positive.
            drive_speed(raceSpd-correctionSpd, raceSpd);
          }
          // Robot is too near to left wall.
          else if (errorVal > 0) {
            // Move right, correctionSpd is negative.
            drive_speed(raceSpd, raceSpd+correctionSpd);
          }
          // pause(10);
          drive_getTicks(&ticks[0], &ticks[1]);
        }
        drive_speed(raceSpd,raceSpd);
        // Robot is centered in box.
        isCurving = 0;
      }
      else {
        // print("Straight-Straight\n");
        // Add required ticks to ticksCounter
        ticksCounter += (int)_MOVE_UNIT;
        // move standard unit
        // TEST
        drive_getTicks(&ticks[0], &ticks[1]);
        // print("Left test ticks: %d. Right test ticks: %d.\n", ticks[0], ticks[1]);
        while(((ticks[0]+ticks[1])/2) < ticksCounter) {
          for (int dacVal = 0; dacVal < 160; dacVal += 8) {
            dac_ctr(26, 0, dacVal);
            freqout(11,  1, 38000);
            irLeft += input(10);                          // Left side IR-sensor

            dac_ctr(27, 1, dacVal);
            freqout(1, 1, 38000);
            irRight += input(2);                          // Right side IR-sensor
          }

          errorVal = irRight - irLeft;

          if ((prevErrorVal > 0 && errorVal <= 0) || (prevErrorVal < 0 && errorVal >= 0)) {
             totalErrorVal = 0;
          }

          errorDiff = errorVal - prevErrorVal;

          prevErrorVal = errorVal;

          irLeft = 0;
          irRight = 0;

          correctionSpd = (kp * errorVal) + (ki * totalErrorVal) + (kd * errorDiff);

          if (errorVal == 0) {
            drive_speed(raceSpd, raceSpd);
          }

          // Robot is too near to right wall.
          else if (errorVal < 0) {
            // Move left, correctionSpd is positive.
            drive_speed(raceSpd-correctionSpd, raceSpd);
          }
          // Robot is too near to left wall.
          else if (errorVal > 0) {
            // Move right, correctionSpd is negative.
            drive_speed(raceSpd, raceSpd+correctionSpd);
          }
          // pause(10);
          drive_getTicks(&ticks[0], &ticks[1]);
        }
        drive_getTicks(&ticks[0], &ticks[1]);
        // print("Left test ticks2: %d. Right test ticks2: %d.\n", ticks[0], ticks[1]);
        drive_speed(raceSpd,raceSpd);
      }
    }
    else if ((boxNumDiff_Behind == 4 && boxNumDiff_Ahead == 1) ||
             (boxNumDiff_Behind == -4 && boxNumDiff_Ahead == -1) ||
             (boxNumDiff_Behind == 1 && boxNumDiff_Ahead == -4) ||
             (boxNumDiff_Behind == -1 && boxNumDiff_Ahead == 4)) {
      if (isCurving) {
        // print("Curving-Right-Short\n");
        // Add required ticks to ticksCounter
        ticksCounter += move_shortRightCurve();
      }
      else {
        // print("Curving-Right-Long\n");
        // Add required ticks to ticksCounter
        ticksCounter += (int)_MOVE_UNIT/2;
        // move HALF standard unit
        while(((ticks[0]+ticks[1])/2) < ticksCounter) {
          for (int dacVal = 0; dacVal < 160; dacVal += 8) {
            dac_ctr(26, 0, dacVal);
            freqout(11,  1, 38000);
            irLeft += input(10);                          // Left side IR-sensor

            dac_ctr(27, 1, dacVal);
            freqout(1, 1, 38000);
            irRight += input(2);                          // Right side IR-sensor
          }

          errorVal = irRight - irLeft;

          if ((prevErrorVal > 0 && errorVal <= 0) || (prevErrorVal < 0 && errorVal >= 0)) {
             totalErrorVal = 0;
          }

          errorDiff = errorVal - prevErrorVal;

          prevErrorVal = errorVal;

          irLeft = 0;
          irRight = 0;

          correctionSpd = (kp * errorVal) + (ki * totalErrorVal) + (kd * errorDiff);

          if (errorVal == 0) {
            drive_speed(raceSpd, raceSpd);
          }

          // Robot is too near to right wall.
          else if (errorVal < 0) {
            // Move left, correctionSpd is positive.
            drive_speed(raceSpd-correctionSpd, raceSpd);
          }
          // Robot is too near to left wall.
          else if (errorVal > 0) {
            // Move right, correctionSpd is negative.
            drive_speed(raceSpd, raceSpd+correctionSpd);
          }
          // pause(10);
          drive_getTicks(&ticks[0], &ticks[1]);
        }
        drive_speed(raceSpd,raceSpd);
        // Add required ticks to ticksCounter
        ticksCounter += move_shortRightCurve();
        isCurving = 1;
      }
    }
    else if ((boxNumDiff_Behind == 4 && boxNumDiff_Ahead == -1) ||
             (boxNumDiff_Behind == -4 && boxNumDiff_Ahead == 1) ||
             (boxNumDiff_Behind == 1 && boxNumDiff_Ahead == 4) ||
             (boxNumDiff_Behind == -1 && boxNumDiff_Ahead == -4)) {
      if (isCurving) {
        // print("Curving-Left-Short\n");
        // Add required ticks to ticksCounter
        ticksCounter += move_shortLeftCurve();
      }
      else {
        // print("Curving-Left-Long\n");
        // Add required ticks to ticksCounter
        ticksCounter += (int)_MOVE_UNIT/2;
        // move HALF standard unit
        while(((ticks[0]+ticks[1])/2) < ticksCounter) {
          for (int dacVal = 0; dacVal < 160; dacVal += 8) {
            dac_ctr(26, 0, dacVal);
            freqout(11,  1, 38000);
            irLeft += input(10);                          // Left side IR-sensor

            dac_ctr(27, 1, dacVal);
            freqout(1, 1, 38000);
            irRight += input(2);                          // Right side IR-sensor
          }

          errorVal = irRight - irLeft;

          if ((prevErrorVal > 0 && errorVal <= 0) || (prevErrorVal < 0 && errorVal >= 0)) {
             totalErrorVal = 0;
          }

          errorDiff = errorVal - prevErrorVal;

          prevErrorVal = errorVal;

          irLeft = 0;
          irRight = 0;

          correctionSpd = (kp * errorVal) + (ki * totalErrorVal) + (kd * errorDiff);

          if (errorVal == 0) {
            drive_speed(raceSpd, raceSpd);
          }

          // Robot is too near to right wall.
          else if (errorVal < 0) {
            // Move left, correctionSpd is positive.
            drive_speed(raceSpd-correctionSpd, raceSpd);
          }
          // Robot is too near to left wall.
          else if (errorVal > 0) {
            // Move right, correctionSpd is negative.
            drive_speed(raceSpd, raceSpd+correctionSpd);
          }
          // pause(10);
          drive_getTicks(&ticks[0], &ticks[1]);
        }
        drive_speed(raceSpd,raceSpd);
        // Add required ticks to ticksCounter
        ticksCounter += move_shortLeftCurve();
        isCurving = 1;
      }
    }
  }

  // Finishing move.
  if (isCurving) {
    // Move HALF standard box to finish
    drive_goto(_MOVE_UNIT/2,_MOVE_UNIT/2);
  }
  else {
    // Move standard box to finish
    drive_goto(_MOVE_UNIT,_MOVE_UNIT);
  }
  drive_speed(0,0);

  return 0;
}
