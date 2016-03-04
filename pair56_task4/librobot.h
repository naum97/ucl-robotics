/**
 * LIBROBOT
 *
 * A library for automation
 *
 * Matthew Bell and Wayne Tsui
 */

#include "simpletools.h"
#include "abdrive.h"
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define _WHEEL_BASE 105.8
#define _TICK_LENGTH 3.25
#define _WHEEL_RADIUS 33.0 // Need to change
#define _ENCODER_CLICKS_PER_FULL_TURN 64

int calculate_arc_length(int angle);

int calculate_pivot_length(int angle);

void turn_function(int angle);

void turn_pivot_function(int angle);

double* distance_wheels_travelled();

double angle_change(double* distanceWheelsTravelled);

double radius_middle(double* distanceWheelsTravelled);

double* position_change(double* distanceWheelsTravelled, double currentAngle);

double distance_travelled(double* positionCoordinates);
