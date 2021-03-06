 /**
 * LIBROBOT
 *
 * A library for automation
 *
 * Matthew Bell and Wayne Tsui
 */

#include "simpletools.h"
#include "abdrive.h"
#include "ping.h"
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define _WHEEL_BASE 105.8
#define _TICK_LENGTH 3.25
#define _WHEEL_RADIUS 33.0 // Need to change
#define _ENCODER_CLICKS_PER_FULL_TURN 64
// #define _PARALLEL_ALIGN_ANGLE (int)15
#define _PARALLEL_ALIGN_ANGLE (int)8

// case of 40x40
#define _MOVE_UNIT (int)(407/3.25)
#define _FRONT_DIST (int)35
#define _MID_SPOT (int)17

// case of 60x60
// #define _MOVE_UNIT (int)(608/3.25)
// #define _FRONT_DIST (int)52
// #define _MID_SPOT (int)27
// #define _MID_SPOT (int)23

// #define DO 22
// #define CLK 23
// #define DI 24
// #define CS 25

// typedef struct node {
//     int val;
//     struct node * next;
// } node_correctionSpd;

int calculate_arc_length(int angle);

int calculate_pivot_length(int angle);

void turn_function(int angle);

void turn_pivot_function(int angle);

// double* distance_wheels_travelled();

// double angle_change(double* distanceWheelsTravelled);

// double radius_middle(double* distanceWheelsTravelled, double angleChange);

// double* position_change(double* distanceWheelsTravelled, double currentAngle);

// double distance_travelled(double* positionCoordinates);

//void log_write(FILE* fp, double* position_coords);

//double *log_read(FILE* fp);

// void push(node_correctionSpd ** head, int val);

int int_in_arr(int x, int *a, int l);

void el_del(int *a, int index, int l);

void rev_array(int *l, int *a);

void shortest_path(int *l, int *a);

void parallel_align_left(int pingDist, int counter);

void parallel_align_right(int pingDist);

int move_shortLeftCurve();

int move_shortRightCurve();

int count_path_corners(int *pathRace, int pathRaceLength);

int move_shortLeftSwerve();

int move_shortRightSwerve();

int count_path_swerves(int *pathRace, int pathRaceLength);
